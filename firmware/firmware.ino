char inputByte;

int nCoilPins = 3;
int coilPins[] = { 2, 4, 6 };

int pwmValue = 0;
int pwmIncrement = 1;
int pwmPin = 4;

void setup() {
  Serial.begin(9600);
  
  for (int i = 0; i < nCoilPins; i++) {
    pinMode(coilPins[i], OUTPUT);
  }
}

void loop() {
  //*
  while (Serial.available() > 0) {
    // parse serial input
    inputByte = Serial.read();
    // ping
    Serial.print(inputByte);
  }
  //*/

  // smooth movement (yay, working ! and coils are singing !):

  analogWrite(pwmPin, pwmValue);
  
  pwmValue += pwmIncrement;
  
  if (pwmValue < 0 || pwmValue > 255) {
    pwmIncrement = -1 * pwmIncrement;
    pwmValue += pwmIncrement;
  }
  
  delay(10);
  
}
