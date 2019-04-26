#include "homie.hpp"
void writeToBuffer(char **buffer, String topic);
void homieDeviceAtt(String deviceAttr);
void homieNodes(String deviceAttr);


void homiePubSub(MQTT *client){
        String deviceAttr = "/homie/" + String(DEVICE_NAME) + "/";

        homieDeviceAtt(deviceAttr);
        homieNodes(deviceAttr);

        har **buffer;
        *buffer = NULL;
        String payload, topic;

        topic = deviceAttr + "$state";
        writeToBuffer(buffer, topic);
        *client.publish(buffer,"ready", true);
}

void writeToBuffer(char **buffer, String topic){
        char *bufferNew = (char*) realloc(*buffer,(topic.length()+1) * sizeof(char));
        if(bufferNew != NULL) {
                *buffer = bufferNew;
                topic.toCharArray(buffer, topic.length()+1);
        }else{
                free(*buffer);
                *buffer = NULL;
        }
}

void homieDeviceAtt(String deviceAttr){
        //Implementing Homie device attributes
        char **buffer;
        *buffer = NULL;
        String payload, topic;

        topic = deviceAttr + "$state";
        writeToBuffer(buffer, topic);
        *client.publish(buffer,"init", true);

        topic = deviceAttr + "$homie";
        writeToBuffer(buffer, topic);
        *client.publish(buffer, "3.0.1", true);

        topic = deviceAttr + "$name";
        writeToBuffer(buffer, topic);
        *client.publish(buffer, DEVICE_NAME, true);

        topic = deviceAttr + "$localip";
        writeToBuffer(buffer, topic);
        *client.publish(buffer, WiFi.localIP(), true);

        topic = deviceAttr + "$mac";
        writeToBuffer(buffer, topic);
        *client.publish(buffer, WiFi.macAddress(), true);

        topic = deviceAttr + "$fw/name";
        writeToBuffer(buffer, topic);
        *client.publish(buffer, FW_NAME, true);

        topic = deviceAttr + "$fw/version";
        writeToBuffer(buffer, topic);
        *client.publish(buffer, FW_VERSION, true);

        //there are two seperate lights
        topic = deviceAttr + "$nodes";
        writeToBuffer(buffer, topic);
        *client.publish(buffer, "rgb-ring", true);

        topic = deviceAttr + "$implementation";
        writeToBuffer(buffer, topic);
        *client.publish(buffer, IMPLEMENTATION, true);

        topic = deviceAttr + "$stats";
        writeToBuffer(buffer, topic);
        *client.publish(buffer, "uptime", true);

        topic = deviceAttr + "$stats/interval";
        writeToBuffer(buffer, topic);
        *client.publish(buffer, "60", true);

        free(*buffer);
}

void homieNodes(String deviceAttr){
        /*Implementing Homie node attributes for the array of two nodes
           Will look like this:
           homie/cube/rgb-ring/$name → "Lights"
           homie/cube/rgb-ring/$properties → "power,brightness"
           homie/cube/rgb-ring/$array → "0-1"

           homie/cube/rgb-ring/brightness/$name → "Brightness"
           homie/cube/rgb-ring/brightness/$settable → "true"
           homie/cube/rgb-ring/brightness/$unit → "%"
           homie/cube/rgb-ring/brightness/$datatype → "integer"
           homie/cube/rgb-ring/brightness/$format → "0:100"

           homie/cube/rgb-ring/power/$name → "Power"
           homie/cube/rgb-ring/power/$settable → "true"
           homie/cube/rgb-ring/power/$datatype → "boolean"

           homie/cube/rgb-ring_0/$name → NODE_DIMMER
           homie/cube/rgb-ring_1/$name → NODE_STRING
         */
        char **buffer;
        *buffer = NULL;
        String payload, topic;

        String nodeAttr = deviceAttr + "rgb-ring/";

        topic = nodeAttr + "$name";
        writeToBuffer(buffer, topic);
        *client.publish(buffer, NODE_RGB, true);

        topic = nodeAttr + "$properties";
        writeToBuffer(buffer, topic);
        *client.publish(buffer, "rgb,brightness", true);

//--------------------PROPERTY 1---------------------------------------
        String propertyAtt = nodeAttr + "brightness/";

        topic = propertyAtt + "$name";
        writeToBuffer(buffer, topic);
        *client.publish(buffer, "Brightness", true);

        topic = propertyAtt + "$settable";
        writeToBuffer(buffer, topic);
        *client.publish(buffer, "true", true);

        topic = propertyAtt + "$unit";
        writeToBuffer(buffer, topic);
        *client.publish(buffer, "%", true);

        topic = propertyAtt + "$datatype";
        writeToBuffer(buffer, topic);
        *client.publish(buffer, "integer", true);

        topic = propertyAtt + "$format";
        writeToBuffer(buffer, topic);
        *client.publish(buffer, "0:100", true);
//------------------------PROPERTY 2------------------------------------
        propertyAtt = nodeAttr + "rgb/";

        topic = propertyAtt + "$name";
        writeToBuffer(buffer, topic);
        *client.publish(buffer, "RGB", true);

        topic = propertyAtt + "$settable";
        writeToBuffer(buffer, topic);
        *client.publish(buffer, "true", true);

        topic = propertyAtt + "$datatype";
        writeToBuffer(buffer, topic);
        *client.publish(buffer, "color", true);

        topic = propertyAtt + "$format";
        writeToBuffer(buffer, topic);
        *client.publish(buffer, "rgb", true);

//--------------------------SUBSCRIBE TO SETTER----------------------
        topic = deviceAttr + "rgb-ring/brightness/set";
        writeToBuffer(buffer, topic);
        *client.subscribe(buffer,1);
        topic = deviceAttr + "rgb-ring/rgb/set";
        writeToBuffer(buffer, topic);
        *client.subscribe(buffer,1);

        free(*buffer);
}
