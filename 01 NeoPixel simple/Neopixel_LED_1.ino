#include <Adafruit_NeoPixel.h>

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN 4  // D2 on ESP8266

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 1  // For a single NeoPixel RGB LED

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#define DELAYVAL 10  // Time (in milliseconds) to pause between colors

uint32_t rgbcolor = 0;
uint32_t hsvValue = 0;

void setup() {
  // put your setup code here, to run once:
  pixels.begin();            // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.setBrightness(10);  // 1-255
}

void loop() {
  //pixels.clear();  // Set all pixel colors to 'off'

  if (hsvValue < 65536) {
    hsvValue = hsvValue + 255;
  } else {
    hsvValue = 0;
  }
  rgbcolor = pixels.ColorHSV(hsvValue);
  pixels.fill(rgbcolor);
  pixels.show();
  delay(DELAYVAL);  // Pause before next pass through loop
}
