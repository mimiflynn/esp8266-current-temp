#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <TM1637Display.h>
#include "WundergroundClient.h"

const int CLK = D2; // Set the CLK pin connection to the display
const int DIO = D3; // Set the DIO pin connection to the display

// WIFI
const char* WIFI_SSID = "";
const char* WIFI_PWD = "";

// Update every 10 minutes
const int UPDATE_INTERVAL_SECS = 10 * 60;

// Wunderground Settings
const boolean IS_METRIC = false;
const String WUNDERGRROUND_API_KEY = "";
const String WUNDERGRROUND_LANGUAGE = "EN";
const String WUNDERGROUND_COUNTRY = "US";
const String WUNDERGROUND_STATE = "NY";
const String WUNDERGROUND_CITY = "New_York_City";


TM1637Display display(CLK, DIO);

WundergroundClient wunderground(IS_METRIC);

bool readyForWeatherUpdate = false;

Ticker ticker;

void setReadyForWeatherUpdate();

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Setting up");

  display.setBrightness(0x0a);
  WiFi.begin(WIFI_SSID, WIFI_PWD);

  Serial.println("Connecting to WiFi");

  int counter = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    counter++;
  }

  Serial.println("Connected");

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
}

