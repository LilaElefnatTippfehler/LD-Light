#ifndef HOMIE_HPP_
#define HOMIE_HPP_
#include <MQTT.h>
#include <list>
#include <string>
#include <cctype>
#include <ctype.h>

#define HOMIE_SERIAL true

#define HOMIE_VERSION "3.0.1"

using namespace std;
namespace homie {
enum stats {uptime, signal, cputemp, cpuload, battery, freeheap, supply};
enum state {init, ready, disconnected, sleeping, lost, alert};
enum datatype {integer_t, float_t, boolean_t, string_t, enum_t, color_t};
bool homieId(string id);
}

class HomieProperties {
private:
string propName, fName, unit, format;
bool settable;
bool retained_t;
homie::datatype datatype;

public:
HomieProperties(string propName, string fName = "", bool settable = false,
                bool retained_t = true, string unit = "",
                homie::datatype datatype = homie::string_t, string format = "");
void init(MQTT *client, string prefix);
string getPropName();
string getDTString(homie::datatype type);
string toString();
};

class HomieNode {
protected:
std::list<HomieProperties> props;
string nodeName, fName, type;

public:
HomieNode(string nodeName, string fName, string type);
void init(MQTT *client, string prefix);
void addProp(HomieProperties prop);
string getNodeName();
string toString();
};

class HomieNodeArray : public HomieNode {
private:
std::list<string> arrayNames;
long arraySize;

public:
HomieNodeArray(string nodeName, string fName, string type, long arraySize);
void init(MQTT *client, string prefix);
void addProp(HomieProperties prop);
void addNames(string name);
string getNodeName();
string toString();
};

class HomieDevice {
private:
std::list<HomieNode> nodes;
string deviceId, fName, localIP, mac, fwName, fwVersion, implementation, interval;

public:
HomieDevice(string deviceId, string fName, string localIP, string mac,
            string fwName, string fwVersion, string implementation,
            string interval);
HomieDevice();
void init(MQTT *client);
void setClient(MQTT * client);
void addNode(HomieNode node);
string getStateString(homie::state state);
void sendStats(MQTT *client, homie::stats stats, string payload);
string getStatsString(homie::stats stats);
string getDeviceId();
string toString();
};

class Homie {
private:
MQTT *client;
HomieDevice device;

public:
Homie(MQTT *client);
boolean connect(String id, String user, String pass);
void disconnect();
boolean connected();
boolean loop();
void setDevice(HomieDevice device);
HomieDevice getDevice();
};

#endif
