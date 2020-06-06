#ifndef UNIT_TEST

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

#include "appconfig.h"
#include "timetriggerevent.h"
#include "mqtttriggerevent.h"
#include "feeder.h"

WiFiUDP udpClient;

// -5H for Eastern Time, we could adjust the offset automatically in the future
NTPClient ntpClient(udpClient, "http://www.pool.ntp.org/zone/north-america", -5*60);

Feeder feeder;
TimeTriggerEvent timeTrigger([](){ feeder.Feed(); });
MqttTriggerEvent mqttTrigger([](){ feeder.Feed(); });

void setup() 
{
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Setting up WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(1000);
  }

  ntpClient.begin();
}

void loop() 
{
  ntpClient.update();
  
  mqttTrigger.Update();
  timeTrigger.Update();
  //feeder.Update();

  // This requires a hardware setup, we'll look at it soon ...
  // ESP.deepSleep(10e6);
}

#endif