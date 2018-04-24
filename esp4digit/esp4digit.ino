#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <TM1637Display.h>
#include "WundergroundClient.h"

// Set the CLK pin connection to the display
const int CLK = D2;
// Set the DIO pin connection to the display
const int DIO = D3;

// WIFI
const char* WIFI_SSID = "";
const char* WIFI_PWD = "";

// Wunderground Settings
const boolean IS_METRIC = false;
const String WUNDERGRROUND_API_KEY = "";
const String WUNDERGRROUND_LANGUAGE = "EN";
const String WUNDERGROUND_COUNTRY = "US";
const String WUNDERGROUND_STATE = "NY";
const String WUNDERGROUND_CITY = "New_York_City";

// Update every 10 minutes
const int UPDATE_INTERVAL_SECS = 10 * 60;

// Flag for timer function
bool readyForWeatherUpdate = false;

void setReadyForWeatherUpdate();
void updateWeather();

// init timer
Ticker ticker;

// set up the 4-Digit Display.
TM1637Display display(CLK, DIO);

// init wunderground api library
WundergroundClient wunderground(IS_METRIC);


void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Setting up");

  // Set the diplay to maximum brightness
  display.setBrightness(0x0a);
  WiFi.begin(WIFI_SSID, WIFI_PWD);

  Serial.println("Connecting to WiFi");

  // Show status of WiFi connection
  int counter = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    counter++;
  }

  Serial.println("Connected");

  // Start timer
  ticker.attach(UPDATE_INTERVAL_SECS, setReadyForWeatherUpdate);

  updateWeather();
}

void loop() {
  if (readyForWeatherUpdate) {
    updateWeather();
  }
}

void setReadyForWeatherUpdate() {
  Serial.println("Setting readyForUpdate to true");
  readyForWeatherUpdate = true;
}

void updateWeather() {
  Serial.println("Updating weather");
  wunderground.initMetric(IS_METRIC);
  wunderground.updateConditions(WUNDERGRROUND_API_KEY, WUNDERGRROUND_LANGUAGE, WUNDERGROUND_COUNTRY, WUNDERGROUND_CITY);
  Serial.println("Weather Updated");
  String temp = wunderground.getCurrentTemp();
  Serial.println("Current Temp:" + temp);
  display.showNumberDec(temp.toInt());
  Serial.println("Setting readyForUpdate to false");
  readyForWeatherUpdate = false;
}
