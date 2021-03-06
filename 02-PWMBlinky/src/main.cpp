#include <Arduino.h>

#define onboardLED 2
#define redLED 27
#define greenLED 14
#define blueLED 16

#define redChannel 0
#define greenChannel 1
#define blueChannel 2

#define freq 5000
#define resolution 8
// How long should one cycle take in ms
// A cycle is defined as the LED going from off to full brightness and back
#define cycletime 3000

const int delaytime = cycletime / pow(2, resolution) / 2;

void setup() {
  ledcSetup(redChannel, freq, resolution);
  ledcSetup(greenChannel, freq, resolution);
  ledcSetup(blueChannel, freq, resolution);

  ledcAttachPin(redLED, redChannel);  
  ledcAttachPin(greenLED, greenChannel);  
  ledcAttachPin(blueLED, blueChannel);  
}

void loop() {
  for (int channel=0; channel<=2; channel++)
  {
    for (int brightness=0; brightness<=pow(2, resolution); brightness++)
    {
      ledcWrite(channel, brightness);
      delay(delaytime);
    }
    for (int brightness=pow(2, resolution); brightness>=0; brightness--)
    {
      ledcWrite(channel, brightness);
      delay(delaytime);
    }
  }
}