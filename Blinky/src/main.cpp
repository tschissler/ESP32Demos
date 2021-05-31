#include <Arduino.h>

#define onboardLED 2
#define redLED 27
#define greenLED 14
#define blueLED 16

void setup() {
  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(blueLED, OUTPUT);
}

void loop() {
  digitalWrite(redLED, HIGH);
  delay(100);
  digitalWrite(redLED, LOW);
  delay(500);
}