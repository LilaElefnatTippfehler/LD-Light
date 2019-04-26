#include "homie.hpp"

bool homieId(String id){
        if(id.front() == '-') return false;
        std::string::iterator it;
        for(it = id.begin(); it != id.end(); ++it) {
                if(!islower(*it) && !isdigit(*it) && !(*it == '-')) return false;
        }
        return true;
}


//-------------------------------HOMIE DEVICE----------------------------------
HomieDevice::HomieDevice(String deviceId, String fName, String localIP, String mac,
                         String fwName, String fwVersion, String implementation,
                         String interval){
        if(homieId(deviceId)) this.deviceId = deviceId;
        this.fName = fName;
        this.localIP = localIP;
        this.mac = mac;
        this.fwName = fwName;
        this.fwVersion = fwVersion;
        this.implementation = implementation;
        this.interval = interval;
}

bool HomieDevice::init(){
        this.sendState(homie::init);
        String topicPrefix = "homie/" + this.deviceId;
        String topic = topicPrefix + "/$homie";
        this.client->publish(topic.c_str(),HOMIE_VERSION, true);
        topic = topicPrefix + "/$name";
        this.client->publish(topic.c_str(),this.fName.c_str(), true);
        topic = topicPrefix + "/$localip";
        this.client->publish(topic.c_str(),this.localIP.c_str(), true);
        topic = topicPrefix + "/$mac";
        this.client->publish(topic.c_str(),this.mac.c_str(), true);
        topic = topicPrefix + "/$fw/name";
        this.client->publish(topic.c_str(),this.fwName.c_str(), true);
        topic = topicPrefix + "/$fw/version";
        this.client->publish(topic.c_str(),this.fwVersion.c_str(), true);
        topic = topicPrefix + "/$implementation";
        this.client->publish(topic.c_str(),this.implementation.c_str(), true);
        topic = topicPrefix + "/$stats";
        this.client->publish(topic.c_str(),this.implementation.c_str(), true);
        topic = topicPrefix + "/$stats/interval";
        this.client->publish(topic.c_str(),this.interval.c_str(), true);

        std::list<HomieNode>::iterator it;
        String nodes = "";
        for(it = this.nodes.begin(); it != this.nodes.end(); ++it) {
                nodes += it->getNodeName();
        }
        topic = topicPrefix + "/$nodes";
        this.client->publish(topic.c_str(),nodes.c_str(), true);

        for(it = this.nodes.begin(); it != this.nodes.end(); ++it) {
                it->init(this.client, topicPrefix);
        }
        this.sendState(homie::ready);
}

void HomieDevice::addNode(HomieNode node){
        this.nodes.push_back(node);
}

void HomieDevice::sendState(homie::state state){
        String payload = "";
        switch (state) {
        case homie::init:
                payload = "init";
                break;
        case homie::ready:
                payload = "ready";
                break;
        case homie::disconnected:
                payload = "disconnected";
                break;
        case homie::sleeping:
                payload = "sleeping";
                break;
        case homie::lost:
                payload = "lost";
                break;
        case homie::alert:
                payload = "alert";
                break;
        }
        String topic = "homie/" + this.deviceId + "/$state";
        this.client->publish(topic.c_str(),payload.c_str(), true);
}

void HomieDevice::sendStats(homie::stats stats, String payload){
        String topic = "homie/" + this.deviceId + "/$stats/";
        switch (stats) {
        case homie::uptime:
                topic += "uptime";
                break;
        case homie::signal:
                topic += "signal";
                break;
        case homie::cputemp:
                topic += "cputemp";
                break;
        case homie::cpuload:
                topic += "cpuload";
                break;
        case homie::battery:
                topic += "battery";
                break;
        case homie::freeheap:
                topic += "freeheap";
                break;
        case homie::supply:
                topic += "supply";
                break;
        }
        this.client->publish(topic.c_str(),payload.c_str(), true);
}

void HomieDevice::announceStats(homie::stats stats){
        String payload = "";
        switch (stats) {
        case homie::uptime:
                payload = "uptime";
                break;
        case homie::signal:
                payload = "signal";
                break;
        case homie::cputemp:
                payload = "cputemp";
                break;
        case homie::cpuload:
                payload = "cpuload";
                break;
        case homie::battery:
                payload = "battery";
                break;
        case homie::freeheap:
                payload = "freeheap";
                break;
        case homie::supply:
                payload = "supply";
                break;
        }
        String topic = "homie/" + this.deviceId + "/$stats";
        this.client->publish(topic.c_str(),payload.c_str(), true);
}
//-----------------------------------------------------------------------------

//-------------------------------HOMIE NODE------------------------------------
HomieNode::HomieNode(String nodeName, String fName, String type){
        if(homieId(nodeName)) this.nodeName = nodeName;
        this.fName = fName;
        this.type = type;
}

bool HomieNode::init(PubSubClient *client, String prefix){
        String topicPrefix = prefix + "/" + this.nodeName;
        String topic = topicPrefix + "/$name";
        this.client->publish(topic.c_str(),this.fName.c_str(), true);
        topic = topicPrefix + "/$type";
        this.client->publish(topic.c_str(),this.type.c_str(), true);

        std::list<HomieProperties>::iterator it;
        String properties = "";
        for(it = this.props.begin(); it != this.props.end(); ++it) {
                properties += it->getPropName();
        }
        topic = topicPrefix + "/$properties";
        this.client->publish(topic.c_str(),properties.c_str(), true);

        for(it = this.props.begin(); it != this.props.end(); ++it) {
                it->init(client, topicPrefix);
        }
}

void HomieNode::addProp(HomieProperties prop){
        this.props.push_back(prop);
}

String HomieNode::getNodeName(){
        retrun this.nodeName;
}
//-----------------------------------------------------------------------------

//------------------------------HOMIE NODE ARRAY-------------------------------
HomieNodeArray::HomieNodeArray(String nodeName, String fName, String type, long arraySize) : HomieNode(nodeName, fName, type){
        this.arraySize = arraySize;
}

bool HomieNodeArray::init(PubSubClient *client, String prefix){
        String topicPrefix = prefix + "/" + this.nodeName;
        String topic = topicPrefix + "/$name";
        this.client->publish(topic.c_str(),this.fName.c_str(), true);
        topic = topicPrefix + "/$type";
        this.client->publish(topic.c_str(),this.type.c_str(), true);

        std::list<HomieProperties>::iterator it;
        String properties = "";
        for(it = this.props.begin(); it != this.props.end(); ++it) {
                properties += it->getPropName();
        }
        topic = topicPrefix + "/$properties";
        this.client->publish(topic.c_str(),properties.c_str(), true);

        topic = topicPrefix + "/$array";
        String arrayString = "0-" + std::to_string(this.arraySize - 1);
        this.client->publish(topic.c_str(),arrayString.c_str(), true);

        for(it = this.props.begin(); it != this.props.end(); ++it) {
                it->initArray(client, topicPrefix);
        }

        int j = 0;
        std::list<String>::iterator it2;
        for(it2 = this.arrayNames.begin(); it2 != this.arrayNames.end(); ++it2) {
                topic = topicPrefix + "_" + std::to_string(j) + "/$name";
                this.client->publish(topic.c_str(),it->c_str(), true);
        }
}

void HomieNodeArray::addProp(HomieProperties prop){
        this.props.push_back(prop);
}

void HomieNodeArray::addNames(String name){
        this.arrayNames.push_back(name);
}

String HomieNode::getNodeName(){
        retrun this.nodeName + "[]";
}
//-----------------------------------------------------------------------------

//-----------------------------HOMIE PROPERTIES--------------------------------
HomieProperties::HomieProperties(String propName, String fName, bool settable,
                                 bool retained, String unit,
                                 homie::datatype datatype, String format){
        if(homieId(propName)) this.propName = propName;
        this.fName = fName;
        this.settable = settable;
        this.retained = retained;
        this.unit = unit;
        this.datatype = datatype;
        this.format = format;
}

bool HomieProperties::init(PubSubClient *client, String prefix){
        String topicPrefix = prefix + "/" + this.propName;
        String topic = "";
        if(this.fName != "") {
                topic = topicPrefix + "/$name";
                this.client->publish(topic.c_str(),this.fName.c_str(), true);
        }
        if(this.settable) {
                topic = topicPrefix + "/$settable";
                this.client->publish(topic.c_str(),"true", true);
        }
        if(!this.retained) {
                topic = topicPrefix + "/$retained";
                this.client->publish(topic.c_str(),"false", true);
        }
        if(this.unit != "") {
                topic = topicPrefix + "/$unit";
                this.client->publish(topic.c_str(),this.unit.c_str(), true);
        }
        if(this.datatype != homie::string) {
                topic = topicPrefix + "/$datatype";
                String payload = this.getDTString(this.datatype);
                this.client->publish(topic.c_str(),payload.c_str(), true);
        }
        topic = topicPrefix + "/$format";
        if((this.datatype == homie::enum) || (this.datatype == homie::color)) {
                this.client->publish(topic.c_str(),this.format.c_str(), true);
        }else{
                if(this.format != "") {
                        this.client->publish(topic.c_str(),this.format.c_str(), true);
                }
        }
}

String HomieProperties::getPropName(){
        return this.propName;
}

String HomieProperties::getDTString(homie::datatype type){
        String typeStr = "";
        switch (type) {
        case homie::integer:
                typeStr = "integer";
                break;
        case homie::float:
                typeStr = "float";
                break;
        case homie::boolean:
                typeStr = "boolean";
                break;
        case homie::string:
                typeStr = "string";
                break;
        case homie::enum:
                typeStr = "enum";
                break;
        case homie::color:
                typeStr = "color";
                break;
        }
        return typeStr;
}
