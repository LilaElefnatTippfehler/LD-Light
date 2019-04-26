#ifndef HOMIE_HPP_
#define HOMIE_HPP_
#include "config.h"
//#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <list>

#define HOMIE_VERSION "3.0.1"


namespace homie {
enum datatype {integer, float, boolean, string, enum, color};
enum stats {uptime, signal, cputemp, cpuload, battery, freeheap, supply};
enum state {init, ready, disconnected, sleeping, lost, alert};
void homiePubSub(PubSubClient *client);
}


class HomieDevice {
private:
PubSubClient *client;
std::list<HomieNode> nodes;
String deviceId, fName, localIP, mac, fwName, fwVersion, implementation, interval;

public:
HomieDevice(String deviceId, String fName, String localIP, String mac,
            String fwName, String fwVersion, String implementation,
            String interval);
bool init();
void addNode(HomieNode node);
void sendState(homie::state state);
void sendStats(homie::stats stats, String payload);
void announceStats(homie::stats stats);
};

class HomieNode {
protected:
std::list<HomieProperties> props;
String nodeName, fName, type;

public:
HomieNode(String nodeName, String fName, String type);
bool init(PubSubClient *client, String prefix);
void addProp(HomieProperties prop);
String getNodeName();
};

class HomieNodeArray : public HomieNode {
private:
std::list<String> arrayNames;
long arraySize;

public:
HomieNodeArray(String nodeName, String fName, String type, long arraySize);
bool init(PubSubClient *client, String prefix);
void addProp(HomieProperties prop);
void addNames(String name);
String getNodeName();
};

class HomieProperties {
private:
String propName, fName, unit, format;
bool settable, retained;
homie::datatype datatype;

public:
HomieProperties(String propName, String fName = "", bool settable = false,
                bool retained = true, String unit = "",
                homie::datatype datatype = homie::string, String format = "");
bool init(PubSubClient *client, String prefix);
String getPropName();
String getDTString(homie::datatype type);
};



#endif
