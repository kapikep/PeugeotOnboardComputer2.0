#include <Ucglib.h>
#include <SPI.h>
#include <EEPROM.h>
#include <math.h>
#include "Bitmaps.h"

Ucglib_ST7735_18x128x160_HWSPI tft(8, 6, 7);  // A0=8, RESET=7, CS=6, SDA=11, SCK=13

#define INJ_PIN 2           //Injector signal
#define SPEED_PIN 3         //Speed sensor
#define ILLUMINATION_PIN 4  //Parking lights, low - on
#define PWR_COMTROL_PIN 5   //Control pwr, hight - on
#define BTN_PIN 9           //Button
#define RADIO_PIN 10        //Radio remote
#define RPM_PIN 12          //Engine speed

#define TANK_PIN A0         //Fuel level sensor
#define TEMP_ENG_PIN A3     //Engine temperature
#define VOLT_PIN A4         //Voltmeter
#define TEMP_INSIDE_PIN A7  //Temp inside salon

#define TERMIST_B 3315  //Temp inside B-coefficient
#define VIN 5.0

#define VOLUP 0x04
#define VOLDOWN 0x05
#define PULSEWIDTH 555
#define ADDRESS 0x47
#define RADIO_VOLUME_STEP 3  //how much to change the volume of multimedia
#define RADIO_SPEED_UP 85    //speed to increase the volume
#define RADIO_SPEED_DOWN 70  //at what speed to reduce the volume

const int fuel_inj_flow = 152;  //injector performance
float volt = 12;
int temp, temp_eng, temp_ins;
int tank, tank_lvl, range;
float delta;
const int idx_array = 200;  //array size for fuel records
int tank_avg_array[idx_array];
byte idx_avg = 0;

byte size_temp_matrix;
long tank_avg;

boolean btnState, btnFlag, long_press, sec_fl_scr, shutdown, isDrawNames, first_start;
unsigned long debounceTimer;

unsigned long rpm;
float liters_trip, l_h, liters_odo;
float liters_last_trip_0, liters_last_trip_1, liters_last_trip_2, liters_last_trip_3;
int distance_last_trip_0, distance_last_trip_1, distance_last_trip_2, distance_last_trip_3; //km
unsigned long fuel_count;
unsigned long motor_operating_millis, motor_operating_millis_trip;
float consump, consump_avg, consump_odo;
int consump_graphical;
byte speed;
int speed_avg;
unsigned long distance_trip, prev_odo, speed_avg_count, distance_odo, count, service_odo;
unsigned int eeprom_write_count;

unsigned long timer0, timer1, timer2;
byte timer3 = 9, low_volt_timer;
byte screen_num = 0, value_radio_count;
byte pos_y, colRed = 0, colGreen = 255, colBlue = 255;
boolean isParkingLiteOn;

volatile unsigned long inj_time, inj_time_count;
volatile unsigned long speed_count;

void setup() {
  TCCR1A = 0;  //timers
  TCCR1B = (1 << CS11) | (1 << CS10);
  pinMode(INJ_PIN, INPUT_PULLUP);
  pinMode(RPM_PIN, INPUT_PULLUP);
  pinMode(SPEED_PIN, INPUT_PULLUP);
  pinMode(BTN_PIN, INPUT_PULLUP);
  pinMode(ILLUMINATION_PIN, INPUT_PULLUP);
  attachInterrupt(0, inj_func, CHANGE);
  attachInterrupt(1, speed_func, FALLING);
  pinMode(RADIO_PIN, OUTPUT);
  digitalWrite(RADIO_PIN, HIGH);
  pinMode(PWR_COMTROL_PIN, OUTPUT);
  digitalWrite(PWR_COMTROL_PIN, HIGH);
  readEEPROM();
  // Serial.begin(9600);
  if (distance_odo != 0) {
    consump_odo = liters_odo * 1e5 / distance_odo;
  } else {
    consump_odo = 0;
  }
  range = tank_lvl / consump_odo * 100;
  size_temp_matrix = sizeof(temp_matrix) / (sizeof(int) * 2) - 1;
  isParkingLiteOn = digitalRead(ILLUMINATION_PIN);
  colorOfDisplay();
  startDisplay();
}

void loop() {
  button();
  if (millis() - timer0 >= 1101) {
    timer0 = millis();
    rpm = 15e6 / pulseIn(RPM_PIN, LOW, 100000);
    display();
    pwr_manager();
  }
  delta = millis() - timer1;
  if (delta >= 1000) {
    timer1 = millis();
    measure();
    lvl();
    radio();
  }
  if (millis() - timer2 >= 10101) {
    timer2 = millis();
    temp_measure();
  }
}

void pwr_manager() {
  if (volt < 5) {
    low_volt_timer++;
    if (low_volt_timer == 4) {
      tft.clearScreen();
      shutdown = true;
    }
    if (low_volt_timer == 15) {
      writeEEPROM();
    }
    if (low_volt_timer > 20) {
      digitalWrite(PWR_COMTROL_PIN, LOW);
    }
  } else {
    if (low_volt_timer > 5) {
      low_volt_timer = 0;
      shutdown = false;
      isDrawNames = false;
    }
  }
}

void measure() {
  distance_trip = speed_count * 2;  //10 pulses per 1 мeters (5 periods) -> 0.2м per period. *0.96 winter not original tires
  speed = (distance_trip - prev_odo) * 36 * (1000.0 / delta) / 100;
  prev_odo = distance_trip;
  distance_trip /= 10;                              //conversion to meters
  volt = analogRead(VOLT_PIN) * 16 / 1024.0;  //VREF * ((DIV_R1 + DIV_R2) / DIV_R2) (R1 = 10, R2 = 4.7) 15.64?

  if (rpm > 0) {
    motor_operating_millis_trip += delta;
    l_h = inj_time * rpm * 2e-5 * fuel_inj_flow;  //instant consumption
    l_h *= 1e-4;
    fuel_count = inj_time_count * 1e-3 * fuel_inj_flow * 4 / 60;  // мм3, max 43 L per trip. Paired injection, but 1 crank eng - 1 impulse * 2 injectors
    liters_trip = fuel_count * 1e-6;
    speed_avg_count += speed;
    count++;
    speed_avg = speed_avg_count / count;
  } else
    l_h = 0;

  if (distance_trip > 1000) {
    consump_avg = liters_trip * 1e5 / distance_trip;
    consump_odo = (liters_odo + liters_trip) * 1e5 / (distance_odo + distance_trip);
    range = tank_lvl / consump_odo * 100;
  }

  if (speed > 0) {
    consump = l_h * 100 / speed;
    if (consump >= 100) {
      consump = 99;
    }
    consump_graphical = consump * 12;
    if (consump_graphical > 160) {
      consump_graphical = 160;
    }
  } else {
    consump = 0;
    consump_graphical = 0;
  }
  inj_time = 0;
}

void temp_measure() {
  float voltage_ins_temp = analogRead(TEMP_INSIDE_PIN) * VIN / 1024.0;
  float r1 = voltage_ins_temp / (VIN - voltage_ins_temp);
  temp_ins = 1. / (1. / (TERMIST_B)*log(r1) + 1. / (25. + 273.)) - 273;

  if (rpm > 0) {
    temp = analogRead(TEMP_ENG_PIN);
    temp *= 14.5 / volt;
    int i = size_temp_matrix;
    do {
      i--;
    } while (temp > temp_matrix[i][0] && i > 0);
    temp_eng = temp_matrix[i][1];
  }
}

void lvl() {
  if (rpm > 0) {
    if (!first_start) {
      first_start = true;
      tank_avg = 60 - (analogRead(TANK_PIN) - 125) / 7.5;
      if ((tank_avg - tank_lvl) > 5) {
        tank_lvl = tank_avg;
      }
    }
    tank_avg_array[idx_avg] = analogRead(TANK_PIN);
    if (++idx_avg >= idx_array) {
      tank_avg = 0;  // reset average
      for (int i = 0; i < idx_avg; i++) {
        tank_avg += tank_avg_array[i];
      }
      tank_avg /= idx_avg;
      tank_lvl = 60 - (tank_avg - 125) / 7.5;

      if (tank_lvl < 0) {
        tank_lvl = 0;
      }
      idx_avg = 0;  // override oldest value
    }
  }
}

void button() {
  btnState = !digitalRead(BTN_PIN);  // 1 - press
  if (btnState && !btnFlag && (millis() - debounceTimer > 50)) {
    btnFlag = true;            // save press
    debounceTimer = millis();  // save time press
  }
  if (!btnState && btnFlag && (millis() - debounceTimer > 100)) {  // if released and was pressed (btnFlag)
    btnFlag = false;                                               // save release
    debounceTimer = millis();                                      // save time release
    if (!long_press && !sec_fl_scr) {
      screen_num++;
      if (screen_num > 4) screen_num = 0;
    }
    isDrawNames = false;
    long_press = false;
  }
  if (btnState && btnFlag && !long_press && millis() - debounceTimer > 2000) {  //long press
    long_press = true;
  }
}

void writeEEPROM() {
  EEPROM.put(0, distance_odo += distance_trip);
  EEPROM.put(4, liters_odo += liters_trip);
  EEPROM.put(8, tank_lvl);
  // EEPROM.put(12, screen_num); //to save screen number option
  EEPROM.put(48, motor_operating_millis += motor_operating_millis_trip);
  EEPROM.put(52, service_odo += distance_trip);
  EEPROM.put(56, eeprom_write_count++);

  if (liters_trip > 0.2) {
    EEPROM.put(16, liters_trip);
    EEPROM.put(20, distance_trip / 1000);
    EEPROM.put(24, liters_last_trip_0);
    EEPROM.put(28, distance_last_trip_0);
    EEPROM.put(32, liters_last_trip_1);
    EEPROM.put(36, distance_last_trip_1);
    EEPROM.put(40, liters_last_trip_2);
    EEPROM.put(44, distance_last_trip_2);
  }
}

void readEEPROM() {
  EEPROM.get(0, distance_odo);
  EEPROM.get(4, liters_odo);
  EEPROM.get(8, tank_lvl);
  // EEPROM.get(12, screen_num); //to read screen number option
  EEPROM.get(16, liters_last_trip_0);
  EEPROM.get(20, distance_last_trip_0);
  EEPROM.get(24, liters_last_trip_1);
  EEPROM.get(28, distance_last_trip_1);
  EEPROM.get(32, liters_last_trip_2);
  EEPROM.get(36, distance_last_trip_2);
  EEPROM.get(40, liters_last_trip_3);
  EEPROM.get(44, distance_last_trip_3);
  EEPROM.get(48, motor_operating_millis);
  EEPROM.get(52, service_odo);
  EEPROM.get(56, eeprom_write_count);
}

void inj_func() {
  if (bitRead(PIND, INJ_PIN) == LOW) {
    TCNT1H = 0;  //reset timer
    TCNT1L = 0;
  } else {
    inj_time = ((unsigned long)TCNT1L | ((unsigned long)TCNT1H << 8)) * 4;  //processing the value timer
    if (inj_time < 20000) inj_time_count += inj_time;
  }
}

void speed_func() {
  speed_count++;
}