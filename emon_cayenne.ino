/*  ***************************
 *  Cayenne MQTT Power Monitor
 *  Giovanni Gentile
 *  Creative Common Licence 2017
 *  www.0lab.it
 *  
 *  Descriptions:
 *  The Arduino UNO with WiFi shield and
 *  STC013 sensor connected to Analog 2 pin,
 *  collect the value of power in Watt, and
 *  put the value to Cayenne Dashboard by
 *  using the MQTT protocol.
 */

//#define CAYENNE_DEBUG
#define CAYENNE_PRINT Serial
#include <CayenneMQTTWiFi.h>
#include "EmonLib.h"


// Sensor Calibrations
const int volt = 220;
const float ct_calibration = 30; // 29 default
const int currentSensorPin = A2;
float Irms = 0;

// Create an Emon instance
EnergyMonitor emon1;

// WiFi network info.
char ssid[] = "ssid";
char wifiPassword[] = "wifiPassword";

// Cayenne authentication info. This should be obtained from the Cayenne Dashboard.
char username[] = "MQTT_USERNAME";
char password[] = "MQTT_PASSWORD";
char clientID[] = "CLIENT_ID";

unsigned long lastMillis = 0;

void setup() {
	Serial.begin(9600);
	Cayenne.begin(username, password, clientID, ssid, wifiPassword);
  emon1.current(currentSensorPin, ct_calibration);
}

void loop() {
	Cayenne.loop();

	//Publish data every 10 seconds (10000 milliseconds). Change this value to publish at a different interval.
	if (millis() - lastMillis > 10000) {
		lastMillis = millis();
		//Write data to Cayenne here. This example just sends the current uptime in milliseconds.
		Cayenne.virtualWrite(0, lastMillis);
		Cayenne.virtualWrite(1, Irms*volt);
	}
}

CAYENNE_CONNECTED()
{
  CAYENNE_LOG("CAYENNE_CONNECTED");
}

CAYENNE_DISCONNECTED()
{
  CAYENNE_LOG("CAYENNE_DISCONNECTED");
}
//Default function for processing actuator commands from the Cayenne Dashboard.
//You can also use functions for specific channels, e.g CAYENNE_IN(1) for channel 1 commands.
CAYENNE_IN_DEFAULT()
{
	CAYENNE_LOG("CAYENNE_IN_DEFAULT(%u) - %s, %s", request.channel, getValue.getId(), getValue.asString());
	if (strcmp(getValue.asStr(), "error") == 0) {
    getValue.setError("There are an error");
  }
}
CAYENNE_OUT_DEFAULT()
{
 CAYENNE_LOG("CAYENNE_OUT_DEFAULT(%u)", request.channel);
}
