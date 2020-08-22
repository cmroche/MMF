#pragma once

#include <unity.h>
#include <ESP8266WiFi.h>
#include <MQTT.h>

#include <type_traits>
#include <functional>

#include "appconfig.h"
#include "mqtttriggerevent.h"

#include "test_util.hpp"

const String getClientId()
{
  String clientId = "MMF-" + String(ESP.getChipId(), HEX);
  return clientId;
}

void connect_mqtt_client(MQTTClient &client)
{
  Serial.print("MQTT test checking wifi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(1000);
  }

  Serial.print("\nMQTT test connecting");
  while (!client.connect("meowmeowtest", MQTT_USER, MQTT_PASSWORD))
  {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nMQTT test connected!");
}

void should_be_available(void)
{
  String clientId = getClientId();
  MqttTriggerEvent e(nullptr, nullptr);
  struct MqttHandler_t
  {
    bool available = false;
    void cb(String &t, String &p)
    {
      Serial.println(String("Received message: ") + t + " - " + p);
      available = p.equals("online");
    }
  } MqttHandler;

  WiFiClient net;
  MQTTClient client(1024);
  client.begin(MQTT_HOST, net);

  auto mqttHandler = GETCB(MQTTClientCallbackSimple, MqttHandler_t)(std::bind(&MqttHandler_t::cb, &MqttHandler, std::placeholders::_1, std::placeholders::_2));
  client.onMessage(mqttHandler);

  connect_mqtt_client(client);

  // Join to the MQTT service and send an event
  auto topic = String("homeassistant/switch/") + clientId + String("_FEEDER/switch/available");
  client.subscribe(topic);

  // Give the system time to round trip
  auto start = millis();
  while ((millis() - start < 30000) && !MqttHandler.available)
  {
    e.Update();
    client.loop();
    yield();
  }

  client.disconnect();

  TEST_ASSERT_TRUE(MqttHandler.available);
}

void should_fire_event_when_triggered(void)
{
  String clientId = getClientId();

  struct Handler_t
  {
    bool fire = false;
    void cb() { fire = true; }
  } Handler;
  auto handler = GETCB(MqttTriggerEvent::handler_t, Handler_t)(std::bind(&Handler_t::cb, &Handler));

  MqttTriggerEvent e(handler, nullptr);
  e.Update(); // update loop checks and handles connection

  WiFiClient net;
  MQTTClient client(1024);
  client.begin(MQTT_HOST, net);
  connect_mqtt_client(client);

  // Join to the MQTT service and send an event
  auto topic = String("homeassistant/switch/") + clientId + String("_FEEDER/switch");
  client.publish(topic, "ON");

  // Give the system time to round trip
  auto start = millis();
  while ((millis() - start < 30000) && !Handler.fire)
  {
    e.Update();
    client.loop();
    yield();
  }

  client.disconnect();

  TEST_ASSERT_TRUE(Handler.fire);
}

void should_be_momentary_when_triggered(void)
{
  String clientId = getClientId();

  MqttTriggerEvent e(nullptr, nullptr);
  struct MqttHandler_t
  {
    bool fired = false;
    bool reset = false;
    void cb(String &t, String &p)
    {
      Serial.println(String("Received message: ") + t + " - " + p);
      fired = fired || p.equals("ON");
      reset = reset || p.equals("OFF");
    }
  } MqttHandler;

  WiFiClient net;
  MQTTClient client(1024);
  client.begin(MQTT_HOST, net);

  auto mqttHandler = GETCB(MQTTClientCallbackSimple, MqttHandler_t)(std::bind(&MqttHandler_t::cb, &MqttHandler, std::placeholders::_1, std::placeholders::_2));
  client.onMessage(mqttHandler);

  connect_mqtt_client(client);

  // Join to the MQTT service and send an event
  auto topic = String("homeassistant/switch/") + clientId + String("_FEEDER/switch");
  client.subscribe(topic);

  // Give the system time to round trip
  auto start = millis();
  while ((millis() - start < 30000) && !MqttHandler.reset)
  {
    e.Update();
    client.loop();
    yield();
  }

  client.disconnect();

  TEST_ASSERT_FALSE_MESSAGE(MqttHandler.fired, "Duplicate Fired");
  TEST_ASSERT_TRUE_MESSAGE(MqttHandler.reset, "Reset");
}

void RUN_MQTTTRIGGEREVENT_TESTS()
{
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.write("Start MqttTriggerEvent unit tests\n");
  RUN_TEST(should_be_available);
  RUN_TEST(should_fire_event_when_triggered);
  RUN_TEST(should_be_momentary_when_triggered);
  ;
}