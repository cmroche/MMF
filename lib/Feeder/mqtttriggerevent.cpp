#include "mqtttriggerevent.h"

#include <Arduino.h>
#include <ArduinoJson.h>
#include <TimeLib.h>

#include <functional>
#include <type_traits>

#include "callback.h"
#include "../../include/appconfig.h"

MqttTriggerEvent::MqttTriggerEvent(handler_t cb, qty_handler_t qty_cb)
    : _qty_cb(qty_cb), _cb(cb), _mqttClient(1024)
{
    _mqttClient.begin(MQTT_HOST, _netClient);
    _mqttClient.setWill((_baseTopic + "switch/available").c_str(), "offline", true, 0);

    auto handler = GETCB(MQTTClientCallbackSimple, MqttTriggerEvent)(std::bind(&MqttTriggerEvent::OnMqttMessage, this, std::placeholders::_1, std::placeholders::_2));
    _mqttClient.onMessage(handler);

    String clientId = "MMF-";
    clientId += String(ESP.getChipId(), HEX);
    SetClientId(clientId);
}

MqttTriggerEvent::~MqttTriggerEvent()
{
    Disconnect();
}

void MqttTriggerEvent::SetClientId(String &clientId)
{
    _clientId = clientId;
    _baseTopic = String("homeassistant/switch/") + _clientId + String("_FEEDER/");
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
        delay(5000); // Rate limit connection attempts

        if (_mqttClient.connect(_clientId.c_str(), MQTT_USER, MQTT_PASSWORD))
        {
            Serial.println("\nMQTT trigger client connected!");
            PublishTopics();
            return true;
        }

        Serial.print(".");
    }

    return false;
}

void MqttTriggerEvent::PublishTopics()
{
    // Publish the device switch configuration
    StaticJsonDocument<1024> config;
    config["name"] = MQTT_NAME " Feed";
    config["uniq_id"] = _clientId + "_FEEDER";
    config["cmd_t"] = _baseTopic + "switch";
    config["stat_t"] = _baseTopic + "switch/state";
    config["avty_t"] = _baseTopic + "switch/available";
    config["device"]["ids"][0] = _clientId;

    auto cnsArr = config["device"]["cns"].createNestedArray();
    cnsArr.add("mac");
    cnsArr.add(WiFi.macAddress());

    config["device"]["name"] = MQTT_NAME;
    config["device"]["mf"] = "Clifford Roche";
    config["device"]["mdl"] = "MM Mk 1";
    config["device"]["sw"] = VERSION;

    String configJson;
    //serializeJson(config, Serial);
    serializeJson(config, configJson);
    _mqttClient.publish(_baseTopic + "config", configJson, true, 0);

    // Publish the current state
    _mqttClient.publish(_baseTopic + "switch/available", "online", true, 0);
    _mqttClient.publish(_baseTopic + "switch", String("OFF"));
    _mqttClient.publish(_baseTopic + "switch/state", String("OFF"), true, 0);
    _mqttClient.subscribe(_baseTopic + "switch");
    _mqttClient.subscribe(_baseTopic + "switch/amount");

    // Diagnostic information
    auto rInfo = ESP.getResetInfoPtr();
    StaticJsonDocument<1024> rstInfo;
    rstInfo["reason"] = rInfo->reason;
    rstInfo["exccause"] = rInfo->exccause;
    rstInfo["epc1"] = rInfo->epc1;
    rstInfo["epc2"] = rInfo->epc2;
    rstInfo["epc3"] = rInfo->epc3;
    rstInfo["excvaddr"] = String("0x") + String(rInfo->excvaddr, HEX);
    rstInfo["depc"] = rInfo->depc;
    String rstPayload;
    serializeJson(rstInfo, rstPayload);
    _mqttClient.publish(_baseTopic + "switch/reset", rstPayload, true, 0);
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
        _mqttClient.publish(_baseTopic + "switch", "OFF");
        _mqttClient.publish(_baseTopic + "switch/state", "OFF", true, 0);
    }

    auto lastError = _mqttClient.lastError();
    if (lastError != 0)
        Serial.println(String("MQTT last error: ") + _mqttClient.lastError());
}

void MqttTriggerEvent::OnMqttMessage(String &topic, String &payload)
{
    Serial.println("MQTT trigger message: " + topic + " - " + payload);
    if (topic.endsWith(_clientId + "_FEEDER/switch") && payload.equals("ON") && _cb != nullptr)
    {
        tmElements_t elems;
        breakTime(now(), elems);
        String timeStr = String(monthShortStr(elems.Month)) + " " + elems.Day + " " + elems.Hour + ":" + elems.Minute + ":" + elems.Second;
        Serial.printf("MQTT trigger: sending feed event %s\n", timeStr.c_str());

        _lastCbTime = millis();

        if (_cb != nullptr)
            _cb();
    }

    else if (topic.endsWith(_clientId + "_FEEDER/switch/amount"))
    {
        Serial.printf("MQTT trigger: sending new feed amount %s\n", payload.c_str());
        long newVal = payload.toInt();
        if (newVal <= 0)
        {
            Serial.printf("MQTT trigger: received invalid amount, ignoring");
            return;
        }

        if (_qty_cb != nullptr)
            _qty_cb((unsigned long)newVal);
    }
}