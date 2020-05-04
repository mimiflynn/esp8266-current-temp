#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include "WeatherClient.h"

// WIFI
const char *WIFI_SSID = "";
const char *WIFI_PWD = "";

// API Settings
const String WEATHER_STATION = "KNYC";

// Update every 10 minutes
const int UPDATE_INTERVAL_SECS = 10 * 60;

// Flag for timer function
bool readyForWeatherUpdate = false;

void setReadyForWeatherUpdate();
void updateWeather();

// init timer
Ticker ticker;

// set up the Display.
Adafruit_AlphaNum4 alpha4 = Adafruit_AlphaNum4();

// init weather api library
WeatherClient weather;

void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("Setting up");

  alpha4.begin(0x70);
  String greeting = "HIYA";
  scrollString(greeting);

  WiFi.begin(WIFI_SSID, WIFI_PWD);

  Serial.println("Connecting to WiFi");

  // Show status of WiFi connection
  int counter = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    counter++;
  }

  Serial.println("Connected");

  updateWeather();

  // Start timer
  ticker.attach(UPDATE_INTERVAL_SECS, setReadyForWeatherUpdate);
}

void loop()
{
  if (readyForWeatherUpdate)
  {
    updateWeather();
  }
}

void setReadyForWeatherUpdate()
{
  Serial.println("Setting readyForUpdate to true");
  readyForWeatherUpdate = true;
}

void scrollString(String conditions)
{
  char c[50];
  conditions.toCharArray(c, 50);

  for (int i = 0; i <= strlen(c); i++)
  {
    alpha4.writeDigitAscii(0, c[i]);
    alpha4.writeDigitAscii(1, c[i + 1]);
    alpha4.writeDigitAscii(2, c[i + 2]);
    alpha4.writeDigitAscii(3, c[i + 3]);
    alpha4.writeDisplay();

    delay(300);
  }
}

void updateWeather()
{
  Serial.println("Updating weather");
  weather.updateConditions(WEATHER_STATION);
  Serial.println("Weather Updated");
  String conditions = weather.getCurrentConditions();
  Serial.println("Current Conditions:" + conditions);
  scrollString(conditions);
  Serial.println("Setting readyForUpdate to false");
  readyForWeatherUpdate = false;
}
