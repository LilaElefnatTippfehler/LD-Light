// This #include statement was automatically added by the Particle IDE.
//#define ARDUINOJSON_ENABLE_PROGMEM 0
//#include <ArduinoJson.h>

// This #include statement was automatically added by the Particle IDE.
#include <MQTT.h>

// This #include statement was automatically added by the Particle IDE.
#include <neopixel.h>

#include "config.h"

// CONFIGURATION SETTINGS START
// DEBUG SETTINGS:
#define D_WIFI true        //um den Photon virtuell offline zu nehmen
#define DEBUG false
#define DARK false           //if true puts strip pin on D6



//--------------SETUP for more than two Lamps----------------------------
//Above put in your Photons ID
//define more SENSITIVITY and BASELINE_SENSITIVITY for each Lamp. Here below, in touchSampling() and in touchEventCheck()
//          (or find a way to automate it, found it hard as it is realised as preprocessor Comand)
//Add PIN Configurations for strip in setup()

byte server[] = { 192,168,0,220 };
MQTT client(server, 1883, callback);
#define DEVICENAME "LDL"
#define MQTT_USR "andi"
#define MQTT_PW "andi12"
unsigned long lastReconnectAttempt = 0;

#define SOFTWARE "SWPHALL_1.2"


// TWEAKABLE VALUES FOR CAP SENSING. THE BELOW VALUES WORK WELL AS A STARTING PLACE:
#define BASELINE_VARIANCE 512.0 // the higher the number the less the baseline is affected by current readings. (was 512)
#define SENSITIVITY1 8 // Integer. Higher is more sensitive (was 8)
#define SENSITIVITY2 12

#define BASELINE_SENSITIVITY1 16 // Integer. This is a trigger point such that values exceeding this point will not affect the baseline. Higher values make the trigger point sooner. (was 16) worked 20
#define BASELINE_SENSITIVITY2 24

#define SAMPLE_SIZE 512 // 512 // Number of samples to take for one reading. Higher is more accurate but large values cause some latency.(was 32)
#define SAMPLES_BETWEEN_PIXEL_UPDATES 32
#define LOOPS_TO_FINAL_COLOR 150
#define DISCHARGET 100 ///Delay Time to discharge in uSec
const int minMaxColorDiffs[2][2] = {
        {5,20}, // min/Max if color change last color change from same Filimin
        {50,128} // min/Max if color change last color change from different Filimin
};

// CONFIGURATION SETTINGS END
int sPin = D4;
int rPin = D3;


// STATES:
#define PRE_ATTACK 0
#define ATTACK 1
#define DECAY 2
#define SUSTAIN 3
#define RELEASE1 4
#define RELEASE2 5
#define OFF 6

#define END_VALUE 0
#define TIME 1

// END VALUE, TIME
// 160 is approximately 1 second
const long envelopes[6][2] = {
        {0, 0},   // OFF
        {255, 30}, // ATTACK
        {200, 240}, // DECAY
        {200, 1000}, // SUSTAIN
        {150, 60}, // RELEASE1
        {0, 600000} // RELEASE2 (65535 is about 6'45")    original: 1000000 100000=~20min
};

// NEOPIXEL

#define PIXEL_PIN D2
#define PIXEL_COUNT 16
#define PIXEL_TYPE WS2812B


#define tEVENT_NONE 0
#define tEVENT_TOUCH 1
#define tEVENT_RELEASE 2

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);
int currentEvent = tEVENT_NONE;

int finalColor = 0; // 0 to 255
int initColor = 0;
int currentColor = 0; // 0 to 255
int brightness = 0; // 0 to 255
int initBrightness = 0; // 0 to 255
uint32_t colorAndBrightness = 0;
int brightnessDistanceToNextState = 0;
int colorChangeToNextState = 0;
unsigned char state = OFF;
unsigned char prevState = OFF;
unsigned char deviceId = 0;
unsigned char lastColorChangeDeviceId = 0;
long loopCount = 0;
long colorLoopCount = 0;
int touchEvent;

unsigned char myId = 0;
int Touch = tEVENT_NONE;
int cloudColor = 0;
int cloudId = 0;
int einDurchlauf=0;
// timestamps
unsigned long tS;
volatile unsigned long tR;

// reading and baseline
long tReading;
float tBaseline;


long tDelay_e=0;
double tBaseline_e=0;
unsigned char lastColorChangeDeviceId_e = 0;
long finalColor_e = 0;
long cloudTouch_e = 0;

//connectionIssueHandler
int lastConStat = 0;

struct finalColorHold
{
        int id;
        int data;
        int flag;
};

struct finalColorHold toDo;

void setup() {
        SYSTEM_THREAD(DISABLED);

        for (int i = 1; i < (NUM_SPARKS + 1); i++) {
                if (!sparkId[i].compareTo(Spark.deviceID())) {
                        myId = i;
                        break;
                }
                finalColor = random(256);
        }
        if(myId == 1 && !DARK) strip.setPin(D1);     //For different Pin setup
        if(myId == 2 && !DARK) strip.setPin(D2);     //Eigentlich D2
        if(DARK) strip.setPin(D6);                 //for deactivation of LED Strip
        strip.begin();
        strip.show(); // Initialize all pixels to 'off'

        pinMode(sPin,OUTPUT);
        attachInterrupt(rPin,touchSense,RISING);



        if (D_WIFI) {
                Particle.subscribe("finalColor", colorHandler, MY_DEVICES);
                Particle.subscribe("Reset", ResetHandler, MY_DEVICES);
                Particle.subscribe("Google", GoogleHandler, MY_DEVICES);
                Particle.subscribe("Repeat", repeatHandler, MY_DEVICES);

                Particle.variable("tDelay",tDelay_e);
                Particle.variable("tBaseline",tBaseline_e);
                Particle.variable("lastDevice",lastColorChangeDeviceId_e);
                Particle.variable("finalColor",finalColor_e);
                Particle.variable("cloudTouch",cloudTouch_e);

                System.on(cloud_status,connectionIssueHandler);

        }

        for (byte i = 0; i < 1; i++) {
                for (byte j=0; j<strip.numPixels(); j++) {
                        strip.setPixelColor(j, 255, 255, 255);
                }
                strip.show();
                delay(250);
                for (byte j=0; j<strip.numPixels(); j++) {
                        strip.setPixelColor(j, 0,0,0);
                }
                strip.show();
                delay(250);
        }
        // calibrate touch sensor- Keep hands off!!!
        tBaseline = touchSampling(); // initialize to first reading
        tBaseline_e = (double) tBaseline;

        for (int i =0; i < 256; i++) {
                uint32_t color = wheelColor(i,255);
                for (byte j = 0; j < strip.numPixels(); j++) {
                        strip.setPixelColor(j, color);
                        strip.show();
                }
                delay(1);
        }
        for (int j = 255; j >= 0; j--) {
                uint32_t color = wheelColor(255, j);
                for (byte k = 0; k < strip.numPixels(); k++) {
                        strip.setPixelColor(k, color);
                        strip.show();
                }
                delay(1);
        }
}

void loop() {
        if(einDurchlauf==0) {           //Start up routine for Identification
                if(DEBUG) publish("Preprocessing","DEBUG IS TURNED ON");
                if(DARK) publish("Preprocessing","LED STRIP IS TURNED OFF");
                publish("Debugging","Ich bin Nummer "+ (String) myId);
                publish("Software:",SOFTWARE); //SW Identification
                einDurchlauf++;
        }

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

        touchEvent = touchEventCheck();
        if (touchEvent == tEVENT_TOUCH) {
                currentEvent = touchEvent;
                state = PRE_ATTACK;
        } else if (touchEvent == tEVENT_RELEASE) {
                currentEvent = touchEvent;
        }
        stateAndPixelMagic();


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
        } else {
                Serial.print("MQTT conncetion failed,");
                Serial.println(" try again in 5 seconds");
        }
        return client.isConnected();
}

void callback(char* topic, byte* payload, unsigned int length) {
        client.publish("/actu/LDL/status","received");
        char buffer[100];
        sprintf(buffer, "%s%s%s", "/actu/", DEVICENAME, "/cmd");
        /*if (!strcmp(buffer, topic)) {
            const int capacity = JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(3);
           StaticJsonDocument<capacity> msg;
           DeserializationError err = deserializeJson(msg, payload);
           if(!err) {
            auto cmd = msg["cmd"];
            if(cmd.containsKey("color")){
                auto rgb = cmd["color"];

                for(int i=0; i<strip.numPixels();i++){
                    strip.setPixelColor(i,atol(rgb));
                }
            }
            if(cmd.containsKey("brightness")){
                auto brgt = cmd["brightness"];
                strip.setBrightness(atol(brgt));
            }
            strip.show();
           }else{
            Serial.print("Couldnt parse Json Object from: ");
            Serial.println(topic);
            Serial.print("Error: ");
            Serial.println(err.c_str());

                    }
           } */
        sprintf(buffer, "%s%s%s", "/actu/", DEVICENAME, "/color");
        if (!strcmp(buffer, topic)) {
                char p[length + 1];
                memcpy(p, payload, length);
                p[length] = '/0';
                uint32_t color = atol((char*)p);
                for(int i=0; i<strip.numPixels(); i++) {
                        strip.setPixelColor(i,color);
                }
                strip.show();
                client.publish("/actu/LDL/status","color change");
                client.publish("/actu/LDL/statusB",p);
                client.publish("/actu/LDL/statusC",String(color));
        }
        sprintf(buffer, "%s%s%s", "/actu/", DEVICENAME, "/brightness");
        if (!strcmp(buffer, topic)) {
                char p[length + 1];
                snprintf(p, length + 1, "%s", payload);
                p[length] = NULL;
                uint32_t brightness = atol(p);
                strip.setBrightness(brightness);
                strip.show();
                client.publish("/actu/LDL/status","brightness change");
                client.publish("/actu/LDL/statusp",p);
                client.publish("/actu/LDL/statusb",String(brightness));
        }

}

//============================================================
//	Touch UI
//============================================================
//------------------------------------------------------------
// ISR for touch sensing
//------------------------------------------------------------
void touchSense() {
        tR = micros();
}

//------------------------------------------------------------
// touch sampling
//
// sample touch sensor SAMPLE_SIZE times and get average RC delay [usec]
//------------------------------------------------------------
long touchSampling() {
        long tDelay = 0;
        int mSample = 0;

        for (int i=0; i<SAMPLE_SIZE; i++) {
                if (!(i % SAMPLES_BETWEEN_PIXEL_UPDATES)) {
                        stateAndPixelMagic();
                }
                pinMode(rPin, OUTPUT); // discharge capacitance at rPin
                digitalWrite(sPin,LOW);
                digitalWrite(rPin,LOW);
                delayMicroseconds(DISCHARGET); //extra time to discharge 50 works fine
                pinMode(rPin,INPUT); // revert to high impedance input
                // timestamp & transition sPin to HIGH and wait for interrupt in a read loop
                tS = micros();
                tR = tS;
                digitalWrite(sPin,HIGH);
                do {
                        // wait for transition
                } while (digitalRead(rPin)==LOW);

                // accumulate the RC delay samples
                // ignore readings when micros() overflows
                if (tR>tS) {
                        tDelay = tDelay + (tR - tS);
                        mSample++;
                }
        }

        // calculate average RC delay [usec]
        if ((tDelay > 0) && (mSample>0)) {
                tDelay = tDelay/mSample;
        } else {
                tDelay = 0; // this is an error condition!
        }

        int baseline_sensitivity = 0;
        if(myId == 1) baseline_sensitivity = BASELINE_SENSITIVITY1;
        if(myId == 2) baseline_sensitivity = BASELINE_SENSITIVITY2;
        //autocalibration using exponential moving average on data below specified point
        if (tDelay<(tBaseline + tBaseline/baseline_sensitivity)) {
                tBaseline = tBaseline + (tDelay - tBaseline)/BASELINE_VARIANCE;
        }
        tDelay_e = tDelay;
        tBaseline_e = (double) tBaseline;
        return tDelay;
}

//------------------------------------------------------------
// touch event check
//
// check touch sensor for events:
//      tEVENT_NONE     no change
//      tEVENT_TOUCH    sensor is touched (Low to High)
//      tEVENT_RELEASE  sensor is released (High to Low)
//
//------------------------------------------------------------
int touchEventCheck() {

        int touchSense;               // current reading
        static int touchSenseLast = LOW; // last reading

        static unsigned long touchDebounceTimeLast = 0; // debounce timer
        int touchDebounceTime = 50;               // debounce time

        static int touchNow = LOW; // current debounced state
        static int touchLast = LOW; // last debounced state

        int tEvent = tEVENT_NONE; // default event

        // read touch sensor
        tReading = touchSampling();

        int sensitivity = 0;
        if(myId == 1) sensitivity = SENSITIVITY1;
        if(myId == 2) sensitivity = SENSITIVITY2;
        // touch sensor is HIGH if trigger point some threshold above Baseline
        if (tReading>(tBaseline + tBaseline/sensitivity)) {
                touchSense = HIGH;
        } else {
                touchSense = LOW;
        }

        // debounce touch sensor
        // if state changed then reset debounce timer
        if (touchSense != touchSenseLast) {
                touchDebounceTimeLast = millis();
        }
        touchSenseLast = touchSense;

        // accept as a stable sensor reading if the debounce time is exceeded without reset
        if (millis() > touchDebounceTimeLast + touchDebounceTime) {
                touchNow = touchSense;
        }

        // set events based on transitions between readings
        if (!touchLast && touchNow) {
                tEvent = tEVENT_TOUCH;
        }

        if (touchLast && !touchNow) {
                tEvent = tEVENT_RELEASE;
        }

        // update last reading
        touchLast = touchNow;


        //--------------NEEDS FURTHER TESTING------------------
        //--------------WORKS FOR NOW------------------------
        pinMode(rPin, OUTPUT); // discharge capacitance at rPin
        digitalWrite(sPin,LOW);
        digitalWrite(rPin,LOW);
        //delayMicroseconds(DISCHARGET);  //extra time to discharge 50 works fine
        pinMode(rPin,INPUT); // revert to high impedance input

        return tEvent;
}

//============================================================
//	NEOPIXEL
//============================================================
//------------------------------------------------------------
// Wheel
//------------------------------------------------------------

uint32_t wheelColor(byte WheelPos, byte iBrightness) {
        float R, G, B;
        float brightness = iBrightness / 255.0;

        if (WheelPos < 85) {
                R = WheelPos * 3;
                G = 255 - WheelPos * 3;
                B = 0;
        } else if (WheelPos < 170) {
                WheelPos -= 85;
                R = 255 - WheelPos * 3;
                G = 0;
                B = WheelPos * 3;
        } else {
                WheelPos -= 170;
                R = 0;
                G = WheelPos * 3;
                B = 255 - WheelPos * 3;
        }
        R = R * brightness + .5;
        G = G * brightness + .5;
        B = B * brightness + .5;
        return strip.Color((byte) R,(byte) G,(byte) B);
}

void updateNeoPixels(int color) {                          //uint32_t
        for(char i=0; i<strip.numPixels(); i++) {
                strip.setPixelColor(i, color);
        }
        strip.show();
}

void generateColor() {                              //generating new Color
        int newColor = finalColor;                  //differs between prevState Off or generating a new slightly different color
        if (prevState == OFF) {
                finalColor = newColor = currentColor = random(256);
        } else {
                bool diffId = (lastColorChangeDeviceId != myId);
                finalColor += (random(2)*2-1)*
                              (minMaxColorDiffs[diffId][0] +
                               random(minMaxColorDiffs[diffId][1]-minMaxColorDiffs[diffId][0]+1));
                finalColor = (finalColor + 256) % 256;
                // finalColor = 119; // FORCE A COLOR
        }
        colorChangeToNextState = finalColor - currentColor;
        colorChangeToNextState += ((colorChangeToNextState < 0) * 2 - 1) * (abs(colorChangeToNextState) > 127) * 256;
        initColor = currentColor;
        publish("finalColor" + (String) myId, (String) finalColor);
        checkForGhosts();               //Checks if the Photon is however in a loop generating and publishing. Resets it.
        lastColorChangeDeviceId = myId;
        lastColorChangeDeviceId_e = lastColorChangeDeviceId;
        publish("Debugging","lastDevice " + (String) lastColorChangeDeviceId_e);
}

void changeState(unsigned char newState) {          //For changing states
        prevState = state;
        state = newState;
        loopCount = 0;
        initBrightness = brightness;
        brightnessDistanceToNextState = envelopes[newState][END_VALUE] - brightness;
}

void updatePixelSettings() {                        //Pixelmagic works, didnt change anything
        brightness =  min(255, max(0, initBrightness + loopCount *  brightnessDistanceToNextState / envelopes[state][TIME]));
        if (colorLoopCount > LOOPS_TO_FINAL_COLOR) {
                currentColor = finalColor;
        }
        if (currentColor != finalColor) {
                currentColor = (initColor + 256 + colorLoopCount * colorChangeToNextState / LOOPS_TO_FINAL_COLOR ) % 256;
        }
}

void stateAndPixelMagic() {
        switch (state) {
        case PRE_ATTACK:                        //First stage for RL Touch
                generateColor();                //Only point where a new color is calculated
                cloudTouch_e = 0;               //for Debug, to see the color online without monitoring
                changeState(ATTACK);
                publish("Debugging","Touch per Hand");
                colorLoopCount = 0;
                break;
        case ATTACK:                            //Second stage where cloudTouches start from
                updatePixelSettings();          //Update pixels
                currentEvent = tEVENT_NONE;     //Disables event
                if (loopCount >= envelopes[ATTACK][TIME]) {
                        publish("Debugging","STATE");
                        changeState(DECAY);
                }
                break;
        case DECAY:
                updatePixelSettings();
                if ((loopCount >= envelopes[DECAY][TIME]) || (currentEvent == tEVENT_RELEASE))  {
                        changeState(SUSTAIN);
                }
                break;
        case SUSTAIN:
                updatePixelSettings();
                if ((loopCount >= envelopes[SUSTAIN][TIME]) || (currentEvent == tEVENT_RELEASE)) {
                        changeState(RELEASE1);
                        currentEvent = tEVENT_NONE;
                }
                break;
        case RELEASE1:
                updatePixelSettings();
                if (loopCount >= envelopes[RELEASE1][TIME]) {
                        changeState(RELEASE2);
                }
                break;
        case RELEASE2:
                updatePixelSettings();
                if (loopCount >= envelopes[RELEASE2][TIME]) {
                        publish("Device OFF", (String) myId);
                        changeState(OFF);
                }
                break;
        case OFF:
                brightness = 0;
                break;
        default:
                changeState(PRE_ATTACK);
                publish("ERROR","ERROR: STATE DEFAULT TRIGGERED");
                break;
        }
        colorAndBrightness = wheelColor(currentColor, brightness);
        updateNeoPixels(colorAndBrightness);
        loopCount++;
        colorLoopCount++;
        finalColor_e = finalColor;      //for Debug, to see the color online without monitoring
}


void googleAssistTouch(String color){

        //Basicly Imitating the Pre-Attack State
        finalColor = getColorfromString(color);
        colorChangeToNextState = finalColor - currentColor;
        colorChangeToNextState += ((colorChangeToNextState < 0) * 2 - 1) * (abs(colorChangeToNextState) > 127) * 256;
        initColor = currentColor;
        changeState(ATTACK);
        colorLoopCount = 0;
        stateAndPixelMagic();

}



int cloudTouch(int color){
        int cloudColor = color;
        publish("Debugging","Cloud Touch triggered");
        if((cloudColor != finalColor) && (myId != cloudId)) {    //Color from Cloud is not the same as own color
                cloudTouch_e = 1;                       //for Debug, to see the color online without monitoring
                getColorFromCloud(cloudColor);     //Define cloudColor as new final color
                changeState(ATTACK);             //Jump to Attack State
                stateAndPixelMagic();       //Magic
        }

        if(cloudColor == finalColor) {};


}

void getColorFromCloud(int color) {
        finalColor = color;
        publish("Debugging","FinalColor is:" + (String) finalColor);
        colorChangeToNextState = finalColor - currentColor;                                             //Color calculating magic
        colorChangeToNextState += ((colorChangeToNextState < 0) * 2 - 1) * (abs(colorChangeToNextState) > 127) * 256;
        initColor = currentColor;
        //if (D_SERIAL) {Serial.print("get Color From Server Final color: "); Serial.print(finalColor); Serial.print(", "); Serial.println(colorChangeToNextState);}
        //Particle.publish("Id", (String) myId, 60, PRIVATE);
        //Particle.publish("finalColor", (String) finalColor, 60, PRIVATE);
        lastColorChangeDeviceId = cloudId;                              //track who changed color
        lastColorChangeDeviceId_e = lastColorChangeDeviceId;
        publish("Debugging","lastDevice " + (String) lastColorChangeDeviceId_e);
        publish("finalColor" + (String) myId, (String) finalColor);
        colorLoopCount = 0;
}

void checkForGhosts(){
        static int touchPerSec = 0;
        static int touchPerMin = 0;
        static int lastTouchTSec = 0;
        static int lastTouchTMin = 0;

        if(lastTouchTSec == 0) lastTouchTSec = millis();    //Initialisation
        if(lastTouchTMin == 0) lastTouchTSec = millis();

        if((millis()-lastTouchTSec)  < 0) return;           //after 49 days millis() will reset
        if((millis()-lastTouchTMin)  < 0) return;
        if((millis()-lastTouchTSec) >= 1000 ) {
                lastTouchTSec = millis();                   //if timed out, reset timer
                touchPerSec = 1;
        }
        else{
                touchPerSec++;                              //if still in range, inkrement
        }

        if((millis()-lastTouchTMin) >= 60000) {
                lastTouchTMin = millis();                   //if timed out, reset timer
                touchPerMin = 1;
        }
        else{
                touchPerMin++;                              //if still in range, inkrement
        }

        if((touchPerSec > 5) || (touchPerMin > 40)) {       //max per Sec and per Min
                publish("Reset", "0");                      //Global Reset for all Lamps
                delay(1);
                System.reset();                             //resets System
        }
}

void publish(String eventName, String data)
{

        if(eventName == "Debugging") {
                if (DEBUG) Particle.publish(eventName,data, 60, PRIVATE);
        }else{
                if(eventName == ("finalColor" + (String) myId)) {
                        if(Particle.connected()) {
                                Particle.publish(eventName, data, 60, PRIVATE); //publishing finalColor + myId for identification
                        }else
                        {
                                toDo.id = myId;
                                toDo.data = finalColor;
                                toDo.flag = 1;
                        }
                }else{
                        Particle.publish(eventName, data, 60, PRIVATE);
                }
        }


}

int getColorfromString(String colorAss){

        //cause of german the first letter might be Uppercase
        String color = colorAss;
        if(isupper(color[0])) {
                color[0] = tolower(color[0]);
        }

        String red =    "red rot";
        String green =  "green grün";
        String blue =   "blue blau";
        String yellow = "yellow gelb";
        String orange = "orange";
        String purple = "purple lila";
        String cyan =   "cyan türkis";

        int translatedColor = 0;

        if(strstr(red, color)) {
                translatedColor = 85; //Pure Red
        }                       //R=255 G=000 B=000
        if(strstr(green, color)) {
                translatedColor = 0; //Pure Green
        }                       //R=000 G=255 B=000
        if(strstr(blue, color)) {
                translatedColor = 170; //Pure Blue
        }                       //R=000 G=000 B=255
        if(strstr(yellow, color)) {
                translatedColor = 42; //Yellow
        }                       //R=126 G=129 B=000
        if(strstr(orange, color)) {
                translatedColor = 60; //Orange
        }                       //R=180 G=75  B=000
        if(strstr(purple, color)) {
                translatedColor = 127; //Purple
        }                       //R=129 G=000 B=126
        if(strstr(cyan, color)) {
                translatedColor = 212; //Cyan
        }                       //R=000 G=126 B=129

        /*
           Just add colors that you like here
           the string are the words that you have to say to google assistant
           to get the colors you want use the code from wheeleColor(); or look here:

           if (Color < 85) {
            R = Color * 3;
            G = 255 - Color * 3;
            B = 0;
           }else if (Color < 170) {
            Color -= 85;
            R = 255 - Color * 3;
            G = 0;
            B = Color * 3;
           } else {
            Color -= 170;
            R = 0;
            G = Color * 3;
            B = 255 - Color * 3;
           }
         */

        return translatedColor;
}


void colorHandler(const char *event, const char *data)
{
        String data1 = data;
        String event1 = event;
        char Id = 0;

        for(int i=0; i<=strlen(event1); i++) {               //searches for digit in string wich is the DeviceId
                if(isdigit(event1[i])) Id = event1[i];
        }

        cloudColor = data1.toInt();                         //extractes color
        cloudId = (int) Id;                                 //converting Id to Int
        cloudId = cloudId - 48;                             //ASCII Offset
        publish("Debugging","Device identified as:" + (String) cloudId);

        if(cloudId == 0) {                               //including the Google Assist functions into an existing Handler
                googleAssistTouch(data1);               //data1 will be strings like "red" "green" "orange" in english and german
                return;
        }

        if(myId != cloudId) {

                cloudTouch(cloudColor);
        }

}

void ResetHandler(const char *event, const char *data)
{
        String data1 = data;
        if(data1.toInt() == myId) System.reset();           //resets if correct Id
        if(data1.toInt() == 0) System.reset();              //Global Reset
}

void GoogleHandler(const char *event, const char *data)
{
        String event1 = event;
        String data1 = data;
        char Id;
        int googleId;

        for(int i=0; i<=strlen(data1); i++) {          //searches for digit in string wich is the DeviceId
                if(isdigit(data1[i])) Id = data1[i];
        }
        googleId = (int) Id - 48;
        publish("Debugging","Identified Id for TurnOff " + (String) googleId);

        if(data1 == "Reset") {                          //Reset if data is reset
                publish("Debugging","Reseting system");
                System.reset();
        }

        if(googleId == myId && !(strncmp(data1,"TurnOff",strlen("TurnOff")))) {
                publish("Debugging","Turning off system"); //Turning off the system if id and command are correct
                changeState(RELEASE2);
                loopCount = 99999999999;                //Also set the loopcount as high as possible so that Release2 is already finished
        }
}

void connectionIssueHandler(system_event_t event, int data){
        int data1 = data;
        if(data1 != lastConStat) {
                if(data1 == cloud_status_connected) {        //checks if connection has been reastablished
                        if(toDo.flag == 1) Particle.publish("finalColor" + (String) toDo.id, (String) toDo.data, 60, PRIVATE); //if it had been touched and was not connected to the Cloud, it retries.
                        toDo.id = 0;
                        toDo.data = 0;
                        toDo.flag = 0;
                        Particle.publish("Repeat",(String) myId, 60, PRIVATE); //asks for the finalColor of ALL other devices. only lastColorChangeDevice answers.
                                                                               //if they all have the same color (wich should be), nothing happens, devices with other colors get updated.
                                                                               //BUG POTENTIAL!! when there are many devices and there are two or more connection Issues and they ask for
                                                                               //repeat at the same time and got touched while offline, there might be chaos.
                }
        }

        lastConStat = data1;
}

void repeatHandler(const char *event, const char *data){    //publishes the color currently displayed if the lamp is the source of the color.
        String data1 = data;                                //and if it is not OFF or in PRE_ATTACK state
        if(data1.toInt() != myId && lastColorChangeDeviceId == myId && state != PRE_ATTACK && state != OFF) {
                publish("finalColor" + (String) myId, (String) finalColor);
        }
}
