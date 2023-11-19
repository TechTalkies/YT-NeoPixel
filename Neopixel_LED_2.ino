#include <Adafruit_NeoPixel.h>

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN 4  // D2 on ESP8266

// Rotary Encoder Inputs
#define inputCLK 13  //D7
#define inputDT 12   //D6
#define inputSW 14   //D5

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 1  // For a single NeoPixel RGB LED

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#define DELAYVAL 10  // Time (in milliseconds) to pause between colors

uint32_t rgbcolor = 0;
uint32_t hsvValue = 0;

int currentStateCLK;
int previousStateCLK;
int increaseStep = 1024;
int brightness = 10;
bool colorMode = true;

void setup() {
  // put your setup code here, to run once:
  pixels.begin();            // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.setBrightness(10);  // 1-255

  // Set encoder pins as inputs
  pinMode(inputCLK, INPUT);
  pinMode(inputDT, INPUT);
  pinMode(inputSW, INPUT_PULLUP);

  // Read the initial state of inputCLK
  // Assign to previousStateCLK variable
  previousStateCLK = digitalRead(inputCLK);
}

void loop() {
  // Read the current state of inputCLK
  currentStateCLK = digitalRead(inputCLK);

  // If the previous and the current state of the inputCLK are different then a pulse has occured
  if (currentStateCLK != previousStateCLK) {
    // If the inputDT state is different than the inputCLK state then
    // the encoder is rotating counterclockwise
    if (digitalRead(inputDT) != currentStateCLK) {
      if (colorMode) {
        updateColor(false);
      } else {
        updateBrightness(false);
      }
    } else {
      if (colorMode) {
        updateColor(true);
      } else {
        updateBrightness(true);
      }
    }
  }
  // Update previousStateCLK with the current state
  previousStateCLK = currentStateCLK;

  if (!digitalRead(inputSW)) {
    colorMode = !colorMode;
    delay(500);  //Giving time so that the input is not read multiple times
  }
}

void updateColor(bool clockWise) {
  if (clockWise) {
    if (hsvValue < 65536) {
      hsvValue = hsvValue + increaseStep;
    } else {
      hsvValue = 0;
    }
  } else {
    if (hsvValue > increaseStep) {
      hsvValue = hsvValue - increaseStep;
    } else {
      hsvValue = 65536;
    }
  }
  rgbcolor = pixels.ColorHSV(hsvValue);
  pixels.fill(rgbcolor);
  pixels.show();
}

void updateBrightness(bool clockWise) {
  if (clockWise) {
    if (brightness <= 10)
      brightness = 10;
    else brightness = brightness - 5;
  } else {
    if (brightness >= 255)
      brightness = 255;
    else brightness = brightness + 5;
  }
  pixels.setBrightness(brightness);  // 1-255
  pixels.show();
}
