#include <Adafruit_NeoPixel.h>

/*
MILCHLICHT -  a rechargeable handsfree nightlight for breastfeeding Dewars

Inductively charges when the light is off and the device is in its holster
Lights fade up gently when picked up

OR

When you pick it up, it stays off but pays attention to its tilt angle: between 90 degrees and vertical we map full brightness to darkness.
(this is so it's still dark when you lift it off, you have to consciously tilt it to make it any brighter. So the subtlest tilt leads to the subtlest light)
This is only in the first three seconds after un-docking. When this time ends, we pulse/buzz or something, and then stay at that brightness

OR
just for ease of prototype, it has a knob that sets brightness (this could be a resistive touch strip:

Arduino Pro Mini
Potentiometer OR membrane/softpot
OR accelerometer, that's to be added later
*/

#define neo 2
#define chg 3
float limit = 10000;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(16, neo, NEO_GRB + NEO_KHZ800);
// digital 3 is charge state; or, I will blow out this pin.
//int accel = A0; // accelerometer
//int tilt; // keep track of tilt
float counter = 0;
int r, g, b;
bool fadeDown = false;
bool instantLifted, justLifted = false;
int liftCount = 0;
int liftCountMax = 1000;

void setup() {
  pinMode(chg, INPUT);
  //  pinMode(accel, INPUT);
  // put your setup code here, to run once:
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

}

void loop() {
  if (counter > limit) {
    counter = 0;
    fadeDown = !fadeDown;
  }

  if (digitalRead(chg)) { // if we're in the dock
    for (int i = 0; i < strip.numPixels(); i++) {
      //if(i/strip.numPixels() < counter/limit) { // slowly fill up the ring with red
      if (i == int(counter / limit * strip.numPixels())) { // one red pixel that moves like a clock
        strip.setPixelColor(i, strip.Color(255, 0, 0));
      } else {
        strip.setPixelColor(i, strip.Color(0, 0, 0));
      }
    }
    instantLifted = true; // get ready for when next undocked
  } else { // if we're in use
    if (instantLifted) { //if this is the first loop in which the light is undocked
      justLifted = true; // start the fade-up-when-lifted routine
      liftCount = 0; // activation timer, runs while undocked
    }
    for (int i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(255, 0, 0));
    }

    if (justLifted) { // we're in the first [liftCountMax] loops while undocked
      //      tilt  = analogRead(accel); // read the y-axis of accelerometer
      //      strip.setBrightness(tilt); // set brightness from tilt if we're in the first three seconds; we should also be low-pas filtering tilt
      strip.setBrightness(int(map(liftCount, 0, liftCountMax, 0, 255)));
    }
    if (liftCount == liftCountMax) {
      justLifted = false;
      liftCount = 0;
    }

    liftCount++;
    instantLifted = false; // so
  }

  strip.show();
  counter++;
}
