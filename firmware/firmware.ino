// todo ...

int inputByte;

void setup() {
  Serial.begin(115200);
}

void loop() {
  while (Serial.available() > 0) {
    // parse serial input
    inputByte = Serial.read();
    Serial.println(inputByte, DEC);
  }
}
