// This #include statement was automatically added by the Particle IDE.
//#define ARDUINOJSON_ENABLE_PROGMEM 0
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <Particle.h>
#include <Arduino.h>
#include <math.h>
#include "colorPoint.h"
#include "colorVector.h"
#include "colorChange.h"
#include "config.h"
#include "homie.hpp"

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

NeoPixel_wrapper *strip = new NeoPixel_wrapper(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);
PubSubClient client(server, 1883, callback);

//connectionIssueHandler
int lastConStat = 0;


void setup() {
        colorChange::init(strip, D1);
        pinMode(sPin,OUTPUT);

        HomieDevice device = HomieDevice(String(DEVICENAME), "Cube", WiFi.localIP()
                                         "00:00:00:00", FW_NAME, FW_VERSION,
                                         "esp8266", "60");
        HomieNode rgbRing = HomieNode("rgb-ring", "RGB Ring", "neoPixel");
        HomieProperties brightness = HomieProperties("brightness", "Brightness",
                                                     true, true, "%",
                                                     homie::integer, "0:100");
        HomieProperties rgb = HomieProperties("rgb", "RGB", true, true, "",
                                              homie::color, "rgb");
        rgbRing.addProp(brightness);
        rgbRing.addProp(rgb);
        device.addNode(rgbRing);
        device.init();
}

void loop() {

        if (!client.isConnected()) {
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


}

boolean reconnect() {
        // Loop until we're reconnected
        if (client.connect(String(DEVICENAME), MQTT_USR, MQTT_PW)) {
                char buffer[100];
                sprintf(buffer, "%s%s%s", "/actu/", DEVICENAME, "/cmd");
                client.subscribe(buffer);
                sprintf(buffer, "%s%s%s", "/actu/", DEVICENAME, "/color");
                client.subscribe(buffer);
                sprintf(buffer, "%s%s%s", "/actu/", DEVICENAME, "/brightness");
                client.subscribe(buffer);
                client.publish("/actu/LDL/status","connected");
                Particle.publish("Connected","successfully");
        } else {
                Particle.publish("MQTT conncetion failed,", " try again in 5 seconds");
                if(SERIAL) Serial.print("MQTT conncetion failed,");
                if(SERIAL) Serial.println(" try again in 5 seconds");
        }
        return client.isConnected();
}

void callback(char* topic, byte* payload, unsigned int length) {

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
        }

}
