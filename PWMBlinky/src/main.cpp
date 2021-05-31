#include <Arduino.h>

#define onboardLED 2
#define redLED 27
#define greenLED 14
#define blueLED 16

#define redChannel 0
#define greenChannel 1
#define bluechannel 2

#define freq 5000
#define resolution 8

void setup() {
  ledcSetup(redChannel, freq, resolution);
  ledcSetup(greenChannel, freq, resolution);
  ledcSetup(bluechannel, freq, resolution);

  ledcAttachPin(redLED, redChannel);  
  ledcAttachPin(greenLED, greenChannel);  
  ledcAttachPin(blueLED, bluechannel);  
}

void loop() {
  for (int channel=0; channel<=2; channel++)
  {
    for (int brightness=0; brightness<=pow(2, resolution); brightness++)
    {
      ledcWrite(channel, brightness);
      delay(5);
    }
    for (int brightness=pow(2, resolution); brightness>=0; brightness--)
    {
      ledcWrite(channel, brightness);
      delay(5);
    }
  }
}