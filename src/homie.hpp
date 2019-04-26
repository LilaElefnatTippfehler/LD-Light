#ifndef HOMIE_HPP_
#define HOMIE_HPP_
#include "config.h"
#include <ESP8266WiFi.h>
#include <MQTT.h>
#include <forward_list>

void homiePubSub(MQTT *client);

class node {
private:
std::forward_list<properties> props;
String topicName, name;

public:
void addProp();
void addArray();

};

class properties {
private:
String topicName, name, settable, unit, datatype, format;

public:
properties(String topicName, String name, String settable, String unit, String datatype, String format);

};



#endif
