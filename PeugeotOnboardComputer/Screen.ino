void startDisplay() {
  delay(1000);
  tft.begin(UCG_FONT_MODE_TRANSPARENT);  // It writes a background for the text. This is the recommended option
  tft.clearScreen();
  tft.setRotate270();  // Set display orientation. Put 90, 180 or 270, or comment to leave default
  drawBitmap(22, 0, lion, 116, 128);
  delay(3000);
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
  } else if (rpm > 500 && temp_eng > 105) {
    tft.setColor(0, 255, 0, 0);
    tft.setFont(ucg_font_profont22_mr);
    tft.setPrintPos(5, 70);
    tft.print("OVERHEAT!!!");
    isDrawNames = false;
  } else if ((long_press || sec_fl_scr) && (screen_num == 1 || screen_num == 4)) {
    if (!sec_fl_scr) {
      sec_fl_scr = true;
      tft.clearScreen();
      tft.setColor(0, colRed, colGreen, colBlue);
      isDrawNames = false;
    }
    if (screen_num == 1) {
      resetOdoScreen();
    } else if (screen_num == 4) {
      rawDataSreen();
    }
  } else {
    switch (screen_num) {
      case 0:
        screenCurrentTrip();
        break;
      case 1:
        screenOdometr();
        break;
      case 2:
        screenConsumption();
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
    drawNames("Liters trip", "Distance trip", "Working time", "Average consump.");
    drawMeasurementUnit(147, "L", 140, "km", 132, "min", 100, "L/100km");
    isDrawNames = true;
  }
  drawConsumpLine();
  tft.setFont(ucg_font_profont22_mr);
  pos_y = 0;
  tft.setPrintPos(64, (pos_y += 32));
  tft.print(liters_trip, 2);
  tft.print(" ");

  tft.setPrintPos(64, (pos_y += 32));
  tft.print((distance_trip / 1000.0), 1);
  tft.print(" ");
  button();

  tft.setPrintPos(64, (pos_y += 32));
  tft.print(millis() / 60000);
  tft.print(" ");
  button();

  tft.setPrintPos(40, (pos_y += 32));
  tft.print(consump_avg, 1);
  tft.print(" ");
}

void screenConsumption() {
  if (!isDrawNames) {
    drawNames("Consumption", "Average consumpt.", "Speed", "Avgerage speed");
    drawMeasurementUnit(100, "L/100km", 100, "L/100km", 125, "km/h", 125, "km/h");
    isDrawNames = true;
  }
  drawConsumpLine();
  pos_y = 0;
  tft.setFont(ucg_font_profont22_mr);
  tft.setPrintPos(40, (pos_y += 32));
  tft.print(consump, 1);
  tft.print(" ");

  tft.setPrintPos(40, (pos_y += 32));
  tft.print(consump_avg, 1);
  tft.print(" ");

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
    drawNames("Liters odometr", "Distance odometr", "Range", "Average consumpt.");
    drawMeasurementUnit(147, "L", 140, "km", 140, "km", 100, "L/100km");
    isDrawNames = true;
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
  tft.print(range);
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
    tft.setFontMode(UCG_FONT_MODE_TRANSPARENT);
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
    tft.setFontMode(UCG_FONT_MODE_SOLID);
  }
}

void screenSensors() {
  if (!isDrawNames) {
    drawNames("Voltmeter", "Tank level", "Eng temp", "Inside temp");
    drawMeasurementUnit(147, "V", 147, "L", 147, "C", 147, "C");
    isDrawNames = true;
  }
  pos_y = 0;
  tft.setFont(ucg_font_profont22_mr);
  tft.setPrintPos(80, (pos_y += 32));
  tft.print(volt, 1);
  tft.print(" ");

  tft.setPrintPos(80, (pos_y += 32));
  tft.print(tank_lvl);
  tft.print(" ");
  button();

  tft.setPrintPos(80, (pos_y += 32));
  tft.print(temp_eng);
  tft.print(" ");

  tft.setPrintPos(80, (pos_y += 32));
  tft.print(temp_ins);
  tft.print("   ");
  button();
}

void screenOther() {
  if (!isDrawNames) {
    drawNames("Consumption", "Inj. time (us)", "RPM", "");
    drawMeasurementUnit(132, "L/h", 140, "", 140, "", 140, "");
    isDrawNames = true;
  }
  pos_y = 0;
  tft.setFont(ucg_font_profont22_mr);
  tft.setPrintPos(70, (pos_y += 32));
  tft.print(l_h, 1);
  tft.print(" ");
  button();

  tft.setPrintPos(70, (pos_y += 32));
  tft.print(inj_time);
  tft.print("       ");
  button();

  tft.setPrintPos(70, (pos_y += 32));
  tft.print(rpm);
  tft.print("     ");
  button();

  tft.setPrintPos(70, (pos_y += 32));
  tft.print("");
  tft.print(" ");
  button();
}

void resetOdoScreen() {
  tft.setFont(ucg_font_profont22_mr);
  tft.setPrintPos(20, 50);
  tft.print("Reset odo?");
  tft.setPrintPos(10, 80);
  tft.setFont(ucg_font_8x13B_mr);
  tft.print("(press button)");
  tft.setPrintPos(140, 80);
  tft.print(timer3--);
  tft.print("  ");
  tft.setFont(ucg_font_profont22_mr);

  if (long_press && timer3 < 7) {
    tft.clearScreen();
    tft.setColor(0, colRed, colGreen, colBlue);
    EEPROM.put(0, distance_odo = 0);
    EEPROM.put(4, liters_odo = 0);
    consump_avg = 0;
    range = 0;
    tft.setPrintPos(50, 70);
    tft.print("Done!");
    delay(3000);
    sec_fl_scr = false;
    timer3 = 9;
  } else if (timer3 == 255) {
    tft.clearScreen();
    tft.setColor(0, colRed, colGreen, colBlue);
    tft.setPrintPos(25, 70);
    tft.print("Time's up");
    delay(3000);
    sec_fl_scr = false;
    timer3 = 9;
  }
}

void rawDataSreen() {
  if (!isDrawNames) {
    tft.clearScreen();
    tft.setColor(0, colRed, colGreen, colBlue);
    tft.setPrintPos(15, 70);
    tft.print("engineering");
    delay(3000);
    drawNames("Voltmeter", "Tank level", "Eng temp", "Inside temp");
    isDrawNames = true;
    tft.setFontMode(UCG_FONT_MODE_SOLID);
  }
  pos_y = 0;
  tft.setFont(ucg_font_profont22_mr);
  tft.setPrintPos(0, (pos_y += 32));
  tft.print(analogRead(VOLT_PIN), 1);
  tft.print(" ");
  tft.setPrintPos(100, pos_y);
  tft.print(volt, 1);
  tft.print(" ");

  tft.setFont(ucg_font_profont22_mr);
  tft.setPrintPos(0, (pos_y += 32));
  tft.print(60 - (analogRead(TANK_PIN) - 125) / 7.5);
  tft.print(" ");
  tft.setPrintPos(100, pos_y);
  tft.print(tank_lvl);
  tft.print(" ");
  button();

  tft.setFont(ucg_font_profont22_mr);
  tft.setPrintPos(0, (pos_y += 32));
  tft.print(analogRead(TEMP_ENG_PIN));
  tft.print(" ");
  tft.setPrintPos(100, pos_y);
  tft.print(temp_eng);
  tft.print(" ");
  button();

  tft.setFont(ucg_font_profont22_mr);
  tft.setPrintPos(0, (pos_y += 32));
  tft.print(analogRead(TEMP_INSIDE_PIN));
  tft.print(" ");
  tft.setPrintPos(100, pos_y);
  tft.print(temp_ins);
  tft.print(" ");
  button();

  if (long_press) {
    tft.clearScreen();
    tft.setColor(0, colRed, colGreen, colBlue);
    tft.setPrintPos(55, 70);
    tft.print("exit");
    delay(3000);
    sec_fl_scr = false;
  }
}

void drawNames(String s1, String s2, String s3, String s4) {
  //128 / 4 = 32
  tft.clearScreen();
  tft.setColor(0, colRed, colGreen, colBlue);
  tft.setFontMode(UCG_FONT_MODE_TRANSPARENT);
  tft.setFont(ucg_font_8x13B_mr);

  pos_y = 13;
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

void drawMeasurementUnit(int16_t pos1, String unit1, int16_t pos2, String unit2,
                         int16_t pos3, String unit3, int16_t pos4, String unit4) {
  tft.setFont(ucg_font_8x13B_mr);
  pos_y = 0;
  tft.setPrintPos(pos1, (pos_y += 32));
  tft.print(unit1);

  tft.setPrintPos(pos2, (pos_y += 32));
  tft.print(unit2);

  tft.setPrintPos(pos3, (pos_y += 32));
  tft.print(unit3);

  tft.setPrintPos(pos4, (pos_y += 32));
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
