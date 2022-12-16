void startDisplay() {
  tft.begin(UCG_FONT_MODE_SOLID);  // It writes a background for the text. This is the recommended option
  tft.clearScreen();
  tft.setRotate270();  // Set display orientation. Put 90, 180 or 270, or comment to leave default
  drawBitmap(22, 0, lion, 116, 128, 128);
  delay(3000);
  tft.clearScreen();
  display();
}

void colorOfDisplay() {
  if (digitalRead(ILLUMINATION_PIN) == LOW && !isParkingLiteOn) {
    colRed = 0, colGreen = 255, colBlue = 255;
    isParkingLiteOn = true;
    isDrawNames = false;
  }
  if (digitalRead(ILLUMINATION_PIN) == HIGH && isParkingLiteOn) {
    colRed = 255, colGreen = 45, colBlue = 0;
    isParkingLiteOn = false;
    isDrawNames = false;
  }
}

void display() {
  colorOfDisplay();
  if (shutdown) {
    drawBitmap(22, 0, lion, 116, 128, 128);
  } else if (volt < 11.3 && !shutdown) {
    timer3++;
    if (timer3 > 60) {
      tft.clearScreen();
      tft.setColor(0, 255, 0, 0);  // Set color (0,R,G,B)
      tft.setFont(ucg_font_profont22_mr);
      tft.setPrintPos(5, 60);
      tft.print("LOW BATTERY!!");
      tft.setPrintPos(50, 90);
      tft.print(volt, 1);
      tft.setPrintPos(100, 90);
      tft.print("V");
      isDrawNames = false;
      delay(1000);
    }
  } else if (rpm > 500 && temp_eng > 105) {
    tft.setColor(0, 255, 0, 0);  // Set color (0,R,G,B)
    tft.setFont(ucg_font_profont22_mr);
    tft.setPrintPos(5, 70);
    tft.print("OVERHEAT!!!");
    isDrawNames = false;
  } else if (reset) {
    tft.setColor(0, colRed, colGreen, colBlue);
    tft.setFont(ucg_font_profont22_mr);
    tft.setPrintPos(20, 50);
    tft.print("Reset odo?");
    tft.setPrintPos(10, 80);
    tft.setFont(ucg_font_8x13B_mr);
    tft.print("(press button)");
    tft.setPrintPos(140, 80);
    tft.print(timer3--);
    tft.print("  ");
    button();
    tft.setFont(ucg_font_profont22_mr);
    if (screen_num == 2) {
      tft.clearScreen();  //после отпускания screen++, следущее нажатие 2
      tft.setColor(0, colRed, colGreen, colBlue);
      EEPROM.put(0, distance_odo = 0);
      EEPROM.put(4, liters_odo = 0);  //обнуляем
      reset = false;
      screen_num = 2;
      tft.setPrintPos(50, 70);
      tft.print("Done!");
      delay(1000);
      timer3 = 7;
    }
    if (millis() - debounceTimer > 7000) {
      tft.clearScreen();
      tft.setColor(0, colRed, colGreen, colBlue);
      reset = false;  //fasle прерывает
      screen_num = 0;
      tft.setPrintPos(25, 70);
      tft.print("Time's up");
      delay(1000);
      timer3 = 7;
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
        screenJoirnal();
        break;
      case 4:
        screenSensors();
        break;
      case 5:
        screenOther();
        break;
    }
  }
}

void screenCurrentTrip() {
  if (!isDrawNames) {
    tft.clearScreen();
    tft.setColor(0, colRed, colGreen, colBlue);
    drawNames("Liters trip", "Distance trip", "Working time", "Average consump.");
    pos_y = 0;
    tft.setFont(ucg_font_8x13B_mr);
    tft.setPrintPos(147, (pos_y += 32));
    tft.print("L");
    tft.setPrintPos(140, (pos_y += 32));
    tft.print("km");
    tft.setPrintPos(132, (pos_y += 32));
    tft.print("min");
    tft.setPrintPos(100, (pos_y += 32));
    tft.print("L/100km");
    
    isDrawNames = true;
  }

  tft.setColor(0, colRed, colGreen, colBlue);
  tft.drawHLine(160 - consump_graphical, 0, consump_graphical);  // Start from left pixel (x,y,lenght)
  tft.drawHLine(160 - consump_graphical, 1, consump_graphical);
  tft.setColor(0, 0, 0, 0);
  tft.drawHLine(0, 0, 160 - consump_graphical);
  tft.drawHLine(0, 1, 160 - consump_graphical);
  tft.setColor(0, colRed, colGreen, colBlue);

  pos_y = 0;

  tft.setFont(ucg_font_profont22_mr);
  tft.setPrintPos(64, (pos_y += 32));
  tft.print(liters_trip, 2);
  tft.print(" ");

  tft.setFont(ucg_font_profont22_mr);
  tft.setPrintPos(64, (pos_y += 32));
  tft.print((distance_trip / 1000.0), 1);
  tft.print(" ");
  button();

  tft.setFont(ucg_font_profont22_mr);
  tft.setPrintPos(64, (pos_y += 32));
  tft.print(millis() / 60000);
  tft.print(" ");
  button();

  tft.setFont(ucg_font_profont22_mr);
  tft.setPrintPos(40, (pos_y += 32));
  tft.print(consump_avg, 1);
  tft.print(" ");
}

void screenConsumption() {
  if (!isDrawNames) {
    tft.clearScreen();
    tft.setColor(0, colRed, colGreen, colBlue);
    drawNames("Consumption", "Average consumpt.", "Speed", "Avgerage speed");
    isDrawNames = true;
    pos_y = 0;
    tft.setFont(ucg_font_8x13B_mr);
    tft.setPrintPos(100, (pos_y += 32));
    tft.print("L/100km");
    tft.setPrintPos(100, (pos_y += 32));
    tft.print("L/100km ");
    tft.setPrintPos(125, (pos_y += 32));
    tft.print("km/h");
    tft.setPrintPos(125, (pos_y += 32));
    tft.print("km/h");

    isDrawNames = true;
  }

  tft.setColor(0, colRed, colGreen, colBlue);
  tft.drawHLine(160 - consump_graphical, 0, consump_graphical);  // Start from left pixel (x,y,lenght)
  tft.drawHLine(160 - consump_graphical, 1, consump_graphical);
  tft.setColor(0, 0, 0, 0);
  tft.drawHLine(0, 0, 160 - consump_graphical);
  tft.drawHLine(0, 1, 160 - consump_graphical);
  tft.setColor(0, colRed, colGreen, colBlue);

  pos_y = 0;
  tft.setFont(ucg_font_profont22_mr);

  tft.setFont(ucg_font_profont22_mr);
  tft.setPrintPos(40, (pos_y += 32));
  tft.print(consump, 1);
  tft.print(" ");

  tft.setFont(ucg_font_profont22_mr);
  tft.setPrintPos(40, (pos_y += 32));
  tft.print(consump_avg, 1);
  tft.print(" ");

  tft.setFont(ucg_font_profont22_mr);
  tft.setPrintPos(64, (pos_y += 32));
  tft.print(speed);
  tft.print(" ");
  button();

  tft.setPrintPos(64, (pos_y += 32));
  tft.print(speed_avg);
  tft.print(" ");
}

void screenOdometr() {
  if (!isDrawNames) {
    tft.clearScreen();
    tft.setColor(0, colRed, colGreen, colBlue);
    drawNames("Liters odometr", "Distance odometr", "Refuel", "Average consumpt.");
    isDrawNames = true;
    tft.setFont(ucg_font_8x13B_mr);
    pos_y = 0;

    tft.setPrintPos(147, (pos_y += 32));
    tft.print("L");

    tft.setPrintPos(140, (pos_y += 32));
    tft.print("km");

    tft.setPrintPos(140, (pos_y += 32));
    tft.print("km");

    tft.setPrintPos(100, (pos_y += 32));
    tft.print("L/100km");
  }
  pos_y = 0;
  tft.setFont(ucg_font_profont22_mr);

  tft.setPrintPos(64, (pos_y += 32));
  tft.print(liters_odo, 1);
  tft.print(" ");

  tft.setPrintPos(64, (pos_y += 32));
  tft.print(distance_odo / 1000.0, 1);
  tft.print(" ");
  button();

  tft.setPrintPos(64, (pos_y += 32));
  tft.print(refuel);
  tft.print(" ");

  tft.setPrintPos(40, (pos_y += 32));
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

    for (i = 0; i < 4; i++) {
      tft.setPrintPos(55, pos_y);
      tft.print("km");
      tft.setPrintPos(147, pos_y);
      tft.print("L");
      pos_y += 32;
    }
    pos_y = 0;
    tft.setFont(ucg_font_profont22_mr);

    tft.setPrintPos(5, (pos_y + 32));
    tft.print(distance_last_trip_0 / 1000);

    tft.setPrintPos(90, (pos_y + 32));
    tft.print(liters_last_trip_0, 1);
    pos_y += 32;
    button();

    tft.setPrintPos(5, (pos_y + 32));
    tft.print(distance_last_trip_1 / 1000);

    tft.setPrintPos(90, (pos_y + 32));
    tft.print(liters_last_trip_1, 1);
    pos_y += 32;

    tft.setPrintPos(5, (pos_y + 32));
    tft.print(distance_last_trip_2 / 1000);

    tft.setPrintPos(90, (pos_y + 32));
    tft.print(liters_last_trip_2, 1);
    pos_y += 32;

    tft.setPrintPos(5, (pos_y + 32));
    tft.print(distance_last_trip_3 / 1000);

    tft.setPrintPos(90, (pos_y + 32));
    tft.print(liters_last_trip_3, 1);
    pos_y += 32;
  }
}

void screenSensors() {
  if (!isDrawNames) {
    tft.clearScreen();
    tft.setColor(0, colRed, colGreen, colBlue);
    drawNames("Voltmeter", "Tank level", "Eng temp", "Eng temp raw");
    isDrawNames = true;
    pos_y = 0;
    tft.setFont(ucg_font_8x13B_mr);
    tft.setPrintPos(147, (pos_y += 32));
    tft.print("V");
    tft.setPrintPos(147, (pos_y += 32));
    tft.print("L");
    tft.setPrintPos(147, (pos_y += 32));
    tft.print("C");
    tft.setPrintPos(147, (pos_y += 32));
    tft.print("");
  }

  pos_y = 0;
  tft.setFont(ucg_font_profont22_mr);

  tft.setPrintPos(80, (pos_y += 32));
  tft.print(volt, 1);
  tft.print(" ");

  tft.setFont(ucg_font_profont22_mr);
  tft.setPrintPos(80, (pos_y += 32));
  tft.print(tank_lvl);
  tft.print(" ");
  button();

  tft.setFont(ucg_font_profont22_mr);
  tft.setPrintPos(80, (pos_y += 32));
  // tft.print(60 - ((analogRead(TANK_PIN) - 125) / 7.5));
  tft.print(temp_eng);  
  tft.print(" ");
  tft.setFont(ucg_font_profont22_mr);
  tft.setPrintPos(80, (pos_y += 32));
  tft.print(analogRead(TEMP_ENG_PIN));
  tft.print("    ");
  button();
}

void screenOther() {
  if (!isDrawNames) {
    tft.clearScreen();
    tft.setColor(0, colRed, colGreen, colBlue);
    drawNames("Consumption", "Consumption", "Ins temp", "Ins temp raw");
    isDrawNames = true;
    pos_y = 0;
    tft.setFont(ucg_font_8x13B_mr);
    tft.setPrintPos(132, (pos_y += 32));
    tft.print("L/h");
    tft.setPrintPos(140, (pos_y += 64));
    tft.print("us");    
    tft.setPrintPos(140, (pos_y += 32));
    tft.print("");
  }
  pos_y = 0;
  tft.setFont(ucg_font_profont22_mr);

  tft.setPrintPos(70, (pos_y += 32));
  tft.print(l_h, 1);
  tft.print(" ");
  button();

  tft.setFont(ucg_font_profont22_mr);
  tft.setPrintPos(70, (pos_y += 32));
  tft.print(inj_time);
  tft.print("  ");
  button();

  tft.setFont(ucg_font_profont22_mr);
  tft.setPrintPos(70, (pos_y += 32));
  tft.print(temp_ins);
  tft.print("    ");
  button();

  tft.setFont(ucg_font_profont22_mr);
  tft.setPrintPos(70, (pos_y += 32));
  tft.print(analogRead(TEMP_INSIDE_PIN));
  tft.print(" ");
  button();
}

void drawNames(String s1, String s2, String s3, String s4) {
  //128 / 4 = 32
  pos_y = 12;
  tft.setFont(ucg_font_8x13B_mr);
  tft.setPrintPos(5, pos_y);
  tft.print(s1);
  button();
  tft.setPrintPos(5, (pos_y += 32));
  tft.print(s2);
  button();
  tft.setPrintPos(5, (pos_y += 32));
  tft.print(s3);
  button();
  tft.setPrintPos(5, (pos_y += 32));
  tft.print(s4);
}

void drawBitmap(int16_t x, int16_t y,
                const uint8_t *bitmap, int16_t w, int16_t h,
                uint16_t color) {
  int16_t i, j, byteWidth = (w + 7) / 8;
  colorOfDisplay();
  tft.setColor(0, colRed, colGreen, colBlue);

  for (j = 0; j < h; j++) {
    for (i = 0; i < w; i++) {
      if (pgm_read_byte(bitmap + j * byteWidth + i / 8) & (128 >> (i & 7))) {
        tft.drawPixel(x + i, y + j);
      }
    }
  }
}
