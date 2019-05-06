#include "homie.h"

bool homie::homieId(string id){
        if(id.front() == '-') return false;
        std::string::iterator it;
        for(it = id.begin(); it != id.end(); ++it) {
                if(!islower(*it) && !isdigit(*it) && !(*it == '-')) return false;
        }
        return true;
}

Homie::Homie(MQTT *client){
        this->client = client;
}

boolean Homie::connect(String id, String user, String pass){
        string deviceId = this->device.getDeviceId();
        string lwTopic = "homie/" + deviceId + "/$state";
        if(HOMIE_SERIAL) Serial.print("Last Will Topic: ");
        if(HOMIE_SERIAL) Serial.println(lwTopic.c_str());
        if(this->client->connect(id.c_str(), user.c_str(), pass.c_str(),lwTopic.c_str(), MQTT::QOS1, true, "lost", false)) {
                if(HOMIE_SERIAL) Serial.println("MQTT connected");
                this->device.init(this->client);
        }else{
                if(HOMIE_SERIAL) Serial.print("MQTT conncetion failed, rc=");
                //if(HOMIE_SERIAL) Serial.print(this->client->state());
                if(HOMIE_SERIAL) Serial.println(" try again in 5 seconds");
        }
        return this->connected();
}


void Homie::disconnect(){
        string topic = "homie/" + this->device.getDeviceId() + "/$state";
        this->client->publish(topic.c_str(), "disconnected");
        this->client->disconnect();
}

boolean Homie::connected(){
        return this->client->isConnected();
}

boolean Homie::loop(){
        return this->client->loop();
}

void Homie::setDevice(HomieDevice device){
        this->device = device;
}

HomieDevice Homie::getDevice(){
        return this->device;
}
