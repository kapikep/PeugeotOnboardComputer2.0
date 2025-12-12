void startDisplay() {
  tft.begin(UCG_FONT_MODE_TRANSPARENT);  // It writes a background for the text. This is the recommended option
  tft.clearScreen();
  tft.setRotate270();  // Set display orientation. Put 90, 180 or 270, or comment to leave default
  drawBitmap(22, 0, lion, 116, 128);
  delay(5000);
  tft.clearScreen();
  display();
}

void colorOfDisplay() {
  if (digitalRead(ILLUMINATION_PIN) == LOW && !isParkingLiteOn) {
    colRed = 0, colGreen = 255, colBlue = 255;
    tft.setColor(0, colRed, colGreen, colBlue);  // Set color (0,R,G,B)
    isParkingLiteOn = true;
    isDrawNames = false;
  }
  if (digitalRead(ILLUMINATION_PIN) == HIGH && isParkingLiteOn) {
    colRed = 255, colGreen = 45, colBlue = 0;
    tft.setColor(0, colRed, colGreen, colBlue);
    isParkingLiteOn = false;
    isDrawNames = false;
  }
}

void display() {
  colorOfDisplay();
  if (shutdown) {
    drawBitmap(22, 0, lion, 116, 128);
  } else if (volt < 11.3 && !shutdown) {
    timer3++;
    if (timer3 > 30) {
      lowBattery();
    }
  } else if (rpm > 500 && temp_eng > 105) {
    overheat();
  } else if ((long_press || sec_fl_scr) && screen_num != 0) {
    if (!sec_fl_scr) {
      sec_fl_scr = true;
      isDrawNames = false;
      long_press = false;
      clearScreenAndSetPos(0, 0);
    }
    if (screen_num == 1) {
      screenOther();
    } else if (screen_num == 2) {
      resetOdoScreen();
    } else if (screen_num == 3) {
      rawDataScreen();
    } else if (screen_num == 4) {
      resetOilServiceScreen();
    }
  } else {
    switch (screen_num) {
      case 0:
        screenCurrentTrip();
        break;
      case 1:
        screenConsumption();
        break;
      case 2:
        screenOdometr();
        break;
      case 3:
        screenSensors();
        break;
      case 4:
        screenJoirnal();
        break;
    }
  }
}

void screenCurrentTrip() {
  if (!isDrawNames) {
    drawNames("Liters trip", "Distance trip", "Working time", "Average consump.");
    drawMeasurementUnit(147, "L", 140, "km", 132, "min", 100, "L/100km");
    isDrawNames = true;
  }
  drawConsumpLine();
  button();

  tft.setFont(ucg_font_profont22_mr);

  tft.setPrintPos(64, 32);
  tft.print(liters_trip, 2);
  tft.print(" ");
  button();

  tft.setPrintPos(64, 64);
  tft.print((distance_trip / 1000.0), 1);
  tft.print(" ");
  button();

  tft.setPrintPos(64, 96);
  tft.print(millis() / 60000);
  tft.print(" ");
  button();

  tft.setPrintPos(40, 128);
  tft.print(consump_avg, 1);
  tft.print(" ");
  button();
}

void screenConsumption() {
  if (!isDrawNames) {
    drawNames("", "", "", "Consumpt. cur./avg.");
    isDrawNames = true;
    tft.setFontMode(UCG_FONT_MODE_SOLID);
  }
  drawConsumpLine();
  button();

  tft.setFont(ucg_font_inb57_mn);
  tft.setPrintPos(10, 62);
  tft.print(speed);
  tft.print("  ");
  button();

  tft.setFont(ucg_font_8x13B_mr);
  tft.setPrintPos(5, 90);
  tft.print("Speed avg.");
  button();
  tft.setFont(ucg_font_profont22_mr);
  tft.setPrintPos(100, 93);
  tft.print(speed_avg);
  tft.print("   ");
  button();

  tft.setPrintPos(5, 128);
  tft.print(consump, 1);
  tft.print("   ");
  button();
  tft.setPrintPos(100, 128);
  tft.print(consump_avg, 1);
  tft.print("   ");
  button();
}

void screenOdometr() {
  if (!isDrawNames) {
    drawNames("Liters odometr", "Distance odometr", "Range", "Average consumpt.");
    drawMeasurementUnit(147, "L", 140, "km", 140, "km", 100, "L/100km");
    isDrawNames = true;
  }

  tft.setFont(ucg_font_profont22_mr);
  tft.setPrintPos(64, 32);
  tft.print(liters_odo, 1);
  tft.print(" ");
  button();

  tft.setPrintPos(64, 64);
  tft.print(distance_odo / 1000.0, 1);
  tft.print(" ");
  button();

  tft.setPrintPos(64, 96);
  tft.print(range);
  tft.print(" ");
  button();

  tft.setPrintPos(40, 128);
  tft.print(consump_odo, 1);
  tft.print(" ");
  button();
}

void screenJoirnal() {
  if (!isDrawNames) {
    tft.clearScreen();
    tft.setColor(0, colRed, colGreen, colBlue);
    drawNames("    Last trips", "", "", "");
    isDrawNames = true;

    pos_y = 32;
    tft.setFont(ucg_font_8x13B_mr);

    for (byte i = 0; i < 4; i++) {
      tft.setPrintPos(55, pos_y);
      tft.print("km");
      tft.setPrintPos(147, pos_y);
      tft.print("L");
      pos_y += 32;
    }

    tft.setFont(ucg_font_profont22_mr);

    tft.setPrintPos(5, 32);
    tft.print(distance_last_trip_0);

    tft.setPrintPos(90, 32);
    tft.print(liters_last_trip_0, 1);

    tft.setPrintPos(5, 64);
    tft.print(distance_last_trip_1);

    tft.setPrintPos(90, 64);
    tft.print(liters_last_trip_1, 1);

    tft.setPrintPos(5, 96);
    tft.print(distance_last_trip_2);

    tft.setPrintPos(90, 96);
    tft.print(liters_last_trip_2, 1);

    tft.setPrintPos(5, 128);
    tft.print(distance_last_trip_3);

    tft.setPrintPos(90, 128);
    tft.print(liters_last_trip_3, 1);
    tft.setFontMode(UCG_FONT_MODE_SOLID);
  }
}

void screenSensors() {
  if (!isDrawNames) {
    drawNames("Voltmeter", "Tank level", "Eng temp", "Inside temp");
    drawMeasurementUnit(147, "V", 147, "L", 147, "C", 147, "C");
    isDrawNames = true;
  }

  tft.setFont(ucg_font_profont22_mr);

  tft.setPrintPos(80, 32);
  tft.print(volt, 1);
  tft.print(" ");
  button();

  tft.setPrintPos(80, 64);
  tft.print(tank_lvl);
  tft.print(" ");
  button();

  tft.setPrintPos(80, 96);
  tft.print(temp_eng);
  tft.print(" ");
  button();

  tft.setPrintPos(80, 128);
  tft.print(temp_ins);
  tft.print("   ");
  button();
}

void screenOther() {
  if (!isDrawNames) {
    clearScreenAndSetPos(53, 70);
    tft.print("other");
    delay(2000);
    drawNames("Consumption", "Inj. time (us)", "RPM", "");
    drawMeasurementUnit(132, "L/h", 140, "", 140, "", 140, "");
    button();
    isDrawNames = true;
    tft.setFontMode(UCG_FONT_MODE_SOLID);
  }

  tft.setFont(ucg_font_profont22_mr);

  tft.setPrintPos(70, 32);
  tft.print(l_h, 1);
  tft.print(" ");

  tft.setPrintPos(70, 64);
  tft.print(inj_time);
  tft.print("       ");

  tft.setPrintPos(70, 96);
  tft.print(rpm);
  tft.print("     ");

  if (long_press) {
    clearScreenAndSetPos(55, 70);
    tft.print("exit");
    delay(2000);
    sec_fl_scr = false;
  }
}

void rawDataScreen() {
  if (!isDrawNames) {
    clearScreenAndSetPos(15, 70);
    tft.print("engineering");
    delay(2000);
    drawNames("Voltmeter", "Tank level", "Eng temp", "Inside temp");
    button();
    isDrawNames = true;
    tft.setFontMode(UCG_FONT_MODE_SOLID);
  }

  tft.setFont(ucg_font_profont22_mr);

  tft.setPrintPos(0, 32);
  tft.print(analogRead(VOLT_PIN), 1);
  tft.print("   ");
  tft.setPrintPos(100, 32);
  tft.print(volt, 1);
  tft.print("   ");

  tft.setPrintPos(0, 64);
  tft.print(analogRead(TANK_PIN));
  tft.print("   ");
  tft.setPrintPos(100, 64);
  tft.print(60 - (analogRead(TANK_PIN) - 130) / 7.5);
  tft.print("   ");

  tft.setPrintPos(0, 96);
  tft.print(analogRead(TEMP_ENG_PIN));
  tft.print(" ");
  tft.setPrintPos(100, 96);
  tft.print(temp_eng);
  tft.print(" ");

  tft.setPrintPos(0, 128);
  tft.print(analogRead(TEMP_INSIDE_PIN));
  tft.print("   ");
  tft.setPrintPos(100, 128);
  tft.print(temp_ins);
  tft.print("   ");

  if (long_press) {
    clearScreenAndSetPos(55, 70);
    tft.print("exit");
    delay(2000);
    sec_fl_scr = false;
  }
}

void drawNames(String s1, String s2, String s3, String s4) {
  tft.clearScreen();
  tft.setColor(0, colRed, colGreen, colBlue);
  tft.setFontMode(UCG_FONT_MODE_TRANSPARENT);
  tft.setFont(ucg_font_8x13B_mr);

  tft.setPrintPos(5, 13);
  tft.print(s1);
  tft.setPrintPos(5, 45);
  tft.print(s2);
  tft.setPrintPos(5, 77);
  tft.print(s3);
  tft.setPrintPos(5, 109);
  tft.print(s4);
}

void drawMeasurementUnit(int16_t pos1, String unit1, int16_t pos2, String unit2,
                         int16_t pos3, String unit3, int16_t pos4, String unit4) {
  tft.setFont(ucg_font_8x13B_mr);

  tft.setPrintPos(pos1, 32);
  tft.print(unit1);

  tft.setPrintPos(pos2, 64);
  tft.print(unit2);

  tft.setPrintPos(pos3, 96);
  tft.print(unit3);

  tft.setPrintPos(pos4, 128);
  tft.print(unit4);

  tft.setFontMode(UCG_FONT_MODE_SOLID);
}

void drawConsumpLine() {
  tft.setFontMode(UCG_FONT_MODE_SOLID);
  tft.drawHLine(160 - consump_graphical, 0, consump_graphical);  // Start from left pixel (x,y,lenght)
  tft.drawHLine(160 - consump_graphical, 1, consump_graphical);
  tft.setColor(0, 0, 0, 0);
  tft.drawHLine(0, 0, 160 - consump_graphical);
  tft.drawHLine(0, 1, 160 - consump_graphical);
  tft.setColor(0, colRed, colGreen, colBlue);
}

void drawBitmap(int16_t x, int16_t y,
                const uint8_t *bitmap, int16_t w, int16_t h) {
  int16_t i, j, byteWidth = (w + 7) / 8;

  tft.setColor(0, colRed, colGreen, colBlue);

  for (j = 0; j < h; j++) {
    for (i = 0; i < w; i++) {
      if (pgm_read_byte(bitmap + j * byteWidth + i / 8) & (128 >> (i & 7))) {
        tft.drawPixel(x + i, y + j);
      }
    }
  }
}

void resetOdoScreen() {
  tft.setFont(ucg_font_profont22_mr);
  tft.setPrintPos(20, 50);
  tft.print("Reset odo?");
  tft.setPrintPos(10, 80);
  tft.setFont(ucg_font_8x13B_mr);
  tft.print("(hold button)");
  tft.setPrintPos(140, 80);
  tft.print(timer3--);
  tft.print("  ");
  tft.setFont(ucg_font_profont22_mr);

  if (long_press && timer3 < 7) {
    clearScreenAndSetPos(50, 70);
    distance_odo = 0;
    liters_odo = 0;
    consump_avg = 0;
    range = 0;
    tft.print("Done!");
    delay(2000);
    sec_fl_scr = false;
    timer3 = 9;
  } else if (timer3 == 255) {
    clearScreenAndSetPos(25, 70);
    tft.print("Time's up");
    delay(2000);
    sec_fl_scr = false;
    timer3 = 9;
  }
}

void resetOilServiceScreen() {
  tft.setFont(ucg_font_profont22_mr);
  tft.setPrintPos(0, 50);
  tft.print("Reset service");
  tft.setFont(ucg_font_8x13B_mr);
  tft.setPrintPos(10, 80);
  tft.print("(hold button)");
  tft.setPrintPos(140, 80);
  tft.print(timer3--);
  tft.print("  ");
  tft.setPrintPos(10, 108);
  tft.print("hours");
  tft.setPrintPos(110, 108);
  tft.print(motor_operating_millis / 3600000);
  tft.setPrintPos(10, 128);
  tft.print("odo");
  tft.setPrintPos(110, 128);
  tft.print(service_odo / 1000);
  tft.setFont(ucg_font_profont22_mr);

  if (long_press && timer3 < 7) {
    clearScreenAndSetPos(50, 70);
    motor_operating_millis = 0;
    service_odo = 0;
    tft.print("Done!");
    delay(2000);
    sec_fl_scr = false;
    timer3 = 9;
  } else if (timer3 == 255) {
    clearScreenAndSetPos(25, 70);
    tft.print("Time's up");
    delay(2000);
    sec_fl_scr = false;
    timer3 = 9;
  }
}

void clearScreenAndSetPos(ucg_int_t x, ucg_int_t y) {
  tft.clearScreen();
  tft.setColor(0, colRed, colGreen, colBlue);
  tft.setPrintPos(x, y);
}

void lowBattery() {
  tft.clearScreen();
  tft.setColor(0, 255, 0, 0);
  tft.setFont(ucg_font_profont22_mr);
  tft.setPrintPos(5, 60);
  tft.print("LOW BATTERY!!");
  tft.setPrintPos(50, 90);
  tft.print(volt, 1);
  tft.setPrintPos(100, 90);
  tft.print("V");
  isDrawNames = false;
}

void overheat() {
  tft.setColor(0, 255, 0, 0);
  tft.setFont(ucg_font_profont22_mr);
  tft.setPrintPos(5, 70);
  tft.print("OVERHEAT!!!");
  isDrawNames = false;
}
