#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>

/* Put your SSID & Password */
const char* ssid = "ESP8266";          // Enter SSID here
const char* password = "1234567859";  //Enter Password here

const int output = 2;

String sliderValue = "0";

const char* PARAM_INPUT = "value";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

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

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>ESP Web Server</title>
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    h2 {font-size: 2.3rem;}
    p {font-size: 1.9rem;}
    body {max-width: 400px; margin:0px auto; padding-bottom: 25px;}
    .slider { -webkit-appearance: none; margin: 14px; width: 360px; height: 25px; 
    background: linear-gradient(to right, red,orange,yellow,green,blue,indigo,violet, red);
      outline: none; -webkit-transition: .2s; transition: opacity .2s;}
    .slider::-webkit-slider-thumb {-webkit-appearance: none; appearance: none; width: 35px; height: 35px; background: #003249; cursor: pointer;}
    .slider::-moz-range-thumb { width: 35px; height: 35px; background: #003249; cursor: pointer; } 
  </style>
</head>
<body>
  <h2>ESP Web Server</h2>
  <p><span id="textSliderValue">%SLIDERVALUE%</span></p>
  <p><input type="range" onchange="updateSliderPWM(this)" id="pwmSlider" min="0" max="65536" value="%SLIDERVALUE%" step="250" class="slider"></p>
<script>
function updateSliderPWM(element) {
  var sliderValue = document.getElementById("pwmSlider").value;
  document.getElementById("textSliderValue").innerHTML = sliderValue;
  console.log(sliderValue);
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/slider?value="+sliderValue, true);
  xhr.send();
}
</script>
</body>
</html>
)rawliteral";

// Replaces placeholder with button section in your web page
String processor(const String& var) {
  //Serial.println(var);
  if (var == "SLIDERVALUE") {
    return sliderValue;
  }
  return String();
}

void setup() {
  Serial.begin(9600);
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

  connectWifi();

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send_P(200, "text/html", index_html, processor);
  });

  // Send a GET request to <ESP_IP>/slider?value=<inputMessage>
  server.on("/slider", HTTP_GET, [](AsyncWebServerRequest* request) {
    String inputMessage;
    // GET input1 value on <ESP_IP>/slider?value=<inputMessage>
    if (request->hasParam(PARAM_INPUT)) {
      inputMessage = request->getParam(PARAM_INPUT)->value();
      sliderValue = inputMessage;
      rgbcolor = pixels.ColorHSV(sliderValue.toInt());
      pixels.fill(rgbcolor);
      pixels.show();
    } else {
      inputMessage = "No message sent";
    }
    Serial.println(inputMessage);
    request->send(200, "text/plain", "OK");
  });

  // Start server
  server.begin();
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

void connectWifi() {
  if (WiFi.status() != WL_CONNECTED) {  // Connect to Wi-Fi network with SSID and password
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    // Print local IP address and start web server
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }
}
