// This #include statement was automatically added by the Particle IDE.
//#define ARDUINOJSON_ENABLE_PROGMEM 0
#include <ArduinoJson.h>
#include <MQTT.h>
#include <Particle.h>
#include <Arduino.h>
#include <math.h>
#include "colorPoint.h"
#include "colorVector.h"
#include "colorChange.h"
#include "config.h"
#include "homie.h"

// CONFIGURATION SETTINGS START
#define SERIAL false

using namespace colorChange;

unsigned long lastReconnectAttempt = 0;

int sPin = D4;
int rPin = D3;

// NEOPIXEL
#define PIXEL_PIN D2
#define PIXEL_COUNT 16
#define PIXEL_TYPE WS2812B

uint16_t port = 1883;
TCPClient tcpClient;
NeoPixel_wrapper *strip = new NeoPixel_wrapper(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);
MQTT client(MQTT_SERVER, port, callback);
Homie homieCTRL = Homie(&client);


Timer pubTimer(60000, &callbackPubTimer);
uint8_t flag_pubTimer = 0;

//connectionIssueHandler
int lastConStat = 0;


void setup() {
        colorChange::init(strip, D1);
        pinMode(sPin,OUTPUT);

        HomieDevice homieDevice = HomieDevice(DEVICENAME, "Cube", "192.168.0.1",
                                              "AA:BB:CC:DD",FW_NAME, FW_VERSION,
                                              "esp8266", "60");
        HomieNode rgbRing = HomieNode("rgb-ring", "RGB Ring", "neoPixel");
        HomieProperties brightness = HomieProperties("brightness", "Brightness",
                                                     true, true, "%",
                                                     homie::integer_t, "0:100");
        HomieProperties rgb = HomieProperties("rgb", "RGB", true, true, "",
                                              homie::color_t, "rgb");
        HomieProperties color = HomieProperties("color", "Color",
                                                true, true, "",
                                                homie::integer_t, "");
        rgbRing.addProp(brightness);
        rgbRing.addProp(rgb);
        rgbRing.addProp(color);
        homieDevice.addNode(rgbRing);
        homieCTRL.setDevice(homieDevice);

        pubTimer.start();
}

void loop() {

        if (!homieCTRL.connected()) {
                unsigned long now = millis();
                if (now - lastReconnectAttempt > 5000) {
                        lastReconnectAttempt = now;
                        // Attempt to reconnect
                        if (reconnect()) {
                                lastReconnectAttempt = 0;
                        }
                }
        }
        client.loop();
        if(flag_pubTimer == 1) {
                long time = millis() / 1000;
                string topic = "homie/" + string(DEVICENAME) + "/$stats/uptime";
                char payload[20];
                sprintf(payload, "%ld", time);
                client.publish(topic.c_str(), payload,true);
                flag_pubTimer = 0;
        }


}

boolean reconnect() {
        // Loop until we're reconnected
        return homieCTRL.connect("Cube", String(MQTT_USR), String(MQTT_PW));
}

void callback(char* topic, byte* payload, unsigned int length) {
        string topicString = string(topic);
        if(SERIAL) Serial.println(topicString.c_str());

        std::size_t found = topicString.find("rgb-ring/brightness/set");
        if(found!=std::string::npos) {
                char p[length + 1];
                snprintf(p, length + 1, "%s", payload);
                p[length] = 0;
                uint32_t brightness = atol(p);
                colorChange::setBrightness(strip, brightness);
        }

        found = topicString.find("rgb-ring/rgb/set");
        if(found!=std::string::npos) {
                char p[length + 1];
                memcpy(p, payload, length);
                p[length] = 0;
                char *r;
                char *g;
                char *b;
                r = &p[0];
                g = strchr(p, ',') + 1;
                b = strrchr(p, ',') + 1;
                *(g-1) = 0;
                *(b-1) = 0;
                colorPoint newColor = colorPoint((uint8_t)atol(r),(uint8_t)atol(g),(uint8_t)atol(b));
                colorChange::setColor(strip,newColor);
        }

        found = topicString.find("rgb-ring/color/set");
        if(found!=std::string::npos) {
                char p[length + 1];
                memcpy(p, payload, length);
                p[length] = 0;
                uint32_t colorN = atol(&p[0]);
                colorPoint newColor = colorPoint(colorN);
                colorChange::setColor(strip,newColor);
        }

/*
        char buffer[100];
        sprintf(buffer, "%s%s%s", "/actu/", DEVICENAME, "/status");
        Particle.publish(buffer,"received");
        sprintf(buffer, "%s%s%s", "/actu/", DEVICENAME, "/payload");
        Particle.publish(buffer, (char *) payload);
        if(SERIAL) Serial.print("payload");
        if(SERIAL) Serial.println((char *) payload);
        sprintf(buffer, "%s%s%s", "/actu/", DEVICENAME, "/cmd");
        if (!strcmp(buffer, topic)) {
                const int capacity = JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(3);
                StaticJsonDocument<capacity> msg;
                DeserializationError err = deserializeJson(msg, payload);
                if(!err) {
                        auto cmd = msg["cmd"];
                        if(cmd.containsKey("color") && cmd.containsKey("brightness")) {
                                uint32_t rgb = cmd["color"];
                                uint8_t brgt = cmd["brightness"];
                                colorPoint newColor = colorPoint(rgb);
                                newColor.setL(brgt);
                                colorChange::setColorAndBright(strip, newColor);
                        }else{
                                if(cmd.containsKey("color")) {
                                        uint32_t rgb = cmd["color"];
                                        if(SERIAL) Serial.print("color ");
                                        if(SERIAL) Serial.println(String((long)rgb));
                                        colorPoint newColor = colorPoint(rgb);
                                        colorChange::setColor(strip, newColor);
                                }
                                if(cmd.containsKey("brightness")) {
                                        uint8_t lum = cmd["brightness"];
                                        if(SERIAL) Serial.print("brightness ");
                                        if(SERIAL) Serial.println(String((long)lum));
                                        colorChange::setBrightness(strip, lum);
                                }
                        }

                }else{
                        if(SERIAL) Serial.print("Couldnt parse Json Object from: ");
                        if(SERIAL) Serial.println(topic);
                        if(SERIAL) Serial.print("Error: ");
                        if(SERIAL) Serial.println(err.c_str());

                }
        }
        sprintf(buffer, "%s%s%s", "/actu/", DEVICENAME, "/color");
        if (!strcmp(buffer, topic)) {
                char p[length + 1];
                memcpy(p, payload, length);
                p[length] = NULL;
                uint32_t colorN = atol((char*)p);
                colorPoint newColor = colorPoint(colorN);
                colorChange::setColor(strip,newColor);
                sprintf(buffer, "%s%s%s", "/actu/", DEVICENAME, "/status");
                client.publish(buffer, "color change");
                if(SERIAL) Serial.print("statusp");
                if(SERIAL) Serial.println(p);
        }
        sprintf(buffer, "%s%s%s", "/actu/", DEVICENAME, "/brightness");
        if (!strcmp(buffer, topic)) {
                char p[length + 1];
                snprintf(p, length + 1, "%s", payload);
                p[length] = NULL;
                uint32_t brightness = atol(p);
                colorChange::setBrightness(strip, brightness);
                sprintf(buffer, "%s%s%s", "/actu/", DEVICENAME, "/status");
                client.publish(buffer,"brightness change");
                if(SERIAL) Serial.print("statusp");
                if(SERIAL) Serial.println(p);
        }*/

}

void callbackPubTimer(){
        flag_pubTimer = 1;
}
