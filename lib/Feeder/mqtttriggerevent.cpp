#include "mqtttriggerevent.h"

#include <Arduino.h>
#include <ArduinoJson.h>

#include <functional>
#include <type_traits>

#include "callback.h"
#include "../../include/appconfig.h"

static const char* const topic = "homeassistant/switch/" MQTT_NAME "/";

MqttTriggerEvent::MqttTriggerEvent(handler_t cb)
: _cb(cb)
, _mqttClient(1024)
{
    _mqttClient.begin(MQTT_HOST, _netClient);
    _mqttClient.setWill((String(topic) + "switch/available").c_str(), "offline", true, 0);

    auto handler = GETCB(MQTTClientCallbackSimple, MqttTriggerEvent)(std::bind(&MqttTriggerEvent::OnMqttMessage, this, std::placeholders::_1, std::placeholders::_2));
    _mqttClient.onMessage(handler);
    //_mqttClient.setClockSource(ntpClockSource);
}

MqttTriggerEvent::~MqttTriggerEvent()
{
    Disconnect();
}

bool MqttTriggerEvent::Connect()
{
    Serial.print("MQTT trigger checking wifi");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(1000);
    }

    Serial.print("\nMQTT trigger connecting");
    for (auto i = 0u; i < 60; ++i) 
    {
        if (_mqttClient.connect(MQTT_NAME, MQTT_USER, MQTT_PASSWORD))
        {
            Serial.println("\nMQTT trigger client connected!");
            PublishTopics();
            return true;
        }

        Serial.print(".");
        delay(1000);
    }

    return false;
}

void MqttTriggerEvent::PublishTopics()
{
    // Publish the device switch configuration
    StaticJsonDocument<512> config;
    config["name"] = MQTT_NAME;
    config["command_topic"] = String(topic) + "switch";
    config["state_topic"] = String(topic) + "switch/state";
    config["availability_topic"] = String(topic) + "switch/available";

    String configJson;
    //serializeJson(config, Serial);
    serializeJson(config, configJson);
    _mqttClient.publish(String(topic) + "config", configJson, true, 0);

    // Publish the current state
    _mqttClient.publish(String(topic) + "switch/available", "online");
    _mqttClient.publish(String(topic) + "switch", String("OFF"));
    _mqttClient.publish(String(topic) + "switch/state", String("OFF"), true, 0);
    _mqttClient.subscribe(String(topic) + "switch");
}

void MqttTriggerEvent::Disconnect()
{
    if (_mqttClient.connected())
        _mqttClient.disconnect();
}

void MqttTriggerEvent::Update()
{
    _mqttClient.loop();
    if (!_mqttClient.connected())
    {
        Connect();
    }
    else if (_lastCbTime)
    {
        _lastCbTime = 0;
        _mqttClient.publish(String(topic) + "switch", "OFF");
        _mqttClient.publish(String(topic) + "switch/state", "OFF", true, 0);
    }

    auto lastError = _mqttClient.lastError();
    if (lastError != 0)
        Serial.println(String("MQTT last error: ") + _mqttClient.lastError());
}

void MqttTriggerEvent::OnMqttMessage(String& topic, String& payload)
{
    Serial.println("MQTT trigger message: " + topic + " - " + payload);
    if (topic.endsWith(MQTT_NAME "/switch") && payload.equals("ON") && _cb != nullptr)
    {
        _lastCbTime = millis();
        _cb();
    }
}