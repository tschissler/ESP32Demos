#include <Arduino.h>

#define onboardLED 2
#define redLED 27
#define greenLED 14
#define blueLED 16

void setup() {
  Serial.begin(115200);

  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(blueLED, OUTPUT);
}

void loop() {
  digitalWrite(blueLED, HIGH);
  delay(100);
  digitalWrite(blueLED, LOW);
  delay(100);
  digitalWrite(blueLED, HIGH);
  delay(100);
  digitalWrite(blueLED, LOW);
  delay(500);
}