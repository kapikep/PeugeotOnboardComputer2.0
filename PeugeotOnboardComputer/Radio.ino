void radio() {
  if (speed > RADIO_SPEED_UP) {
    if (value_radio_count < RADIO_VOLUME_STEP) {
      SendCommand(VOLUP);
      delay(2);
      SendCommand(VOLUP);
      delay(20);
      value_radio_count++;
    }
  } else if (speed < RADIO_SPEED_DOWN) {
    if (value_radio_count > 0) {
      SendCommand(VOLDOWN);
      delay(2);
      SendCommand(VOLDOWN);
      delay(20);
      value_radio_count--;
    }
  }
}

// Send a value (7 bits, LSB is sent first, value can be an address or command)
void SendValue(unsigned char value) {
  unsigned char i, tmp = 1;
  for (i = 0; i < sizeof(value) * 8 - 1; i++) {
    if (value & tmp)  // Do a bitwise AND on the value and tmp
      SendOne();
    else
      SendZero();
    tmp = tmp << 1;  // Bitshift left by 1
  }
}

// Send a command to the radio, including the header, start bit, address and stop bits
void SendCommand(unsigned char value) {
  unsigned char i;
  Preamble();
  for (i = 0; i < 1; i++) {           // Repeat address, command and stop bits three times so radio will pick them up properly
    SendValue(ADDRESS);               // Send the address
    SendValue((unsigned char)value);  // Send the command
    Postamble();                      // Send signals to follow a command to the radio
  }
}

// Signals to transmit a '0' bit
void SendZero() {
  digitalWrite(RADIO_PIN, LOW);  // Output LOW for 1 pulse width
  delayMicroseconds(PULSEWIDTH);
  digitalWrite(RADIO_PIN, HIGH);  // Output HIGH for 1 pulse width
  delayMicroseconds(PULSEWIDTH);
}

// Signals to transmit a '1' bit
void SendOne() {
  digitalWrite(RADIO_PIN, LOW);  // Output LOW for 1 pulse width
  delayMicroseconds(PULSEWIDTH);
  digitalWrite(RADIO_PIN, HIGH);  // Output HIGH for 3 pulse widths
  delayMicroseconds(PULSEWIDTH * 3);
}

// Signals to precede a command to the radio
void Preamble() {
  // HEADER: always HIGH (1 pulse width), LOW (16 pulse widths), HIGH (8 pulse widths)
  digitalWrite(RADIO_PIN, HIGH);  // Make sure output is HIGH for 1 pulse width, so the header starts with a rising edge
  delayMicroseconds(PULSEWIDTH * 1);
  digitalWrite(RADIO_PIN, LOW);  // Start of header, output LOW for 16 pulse widths
  delayMicroseconds(PULSEWIDTH * 16);
  digitalWrite(RADIO_PIN, HIGH);  // Second part of header, output HIGH 8 pulse widths
  delayMicroseconds(PULSEWIDTH * 8);
  // START BIT: always 1
  SendOne();
}

// Signals to follow a command to the radio
void Postamble() {
  // STOP BITS: always 1
  SendOne();
  SendOne();
}
