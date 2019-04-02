#define NUM_SPARKS 2 // number of Filimins in your group
String sparkId[] = {
        "",                                 // 0
        "111111111111111111111111",          // number each Filimin starting at 1. Replace the number in the quotes with Spark ID for Filimin 1
        "222222222222222222222222",          // Filimin 2
};

//--------------SETUP for more than two Lamps----------------------------
//Above put in your Photons ID
//define more SENSITIVITY and BASELINE_SENSITIVITY for each Lamp. Here below, in touchSampling() and in touchEventCheck()
//          (or find a way to automate it, found it hard as it is realised as preprocessor Comand)
//Add PIN Configurations for strip in setup()

byte server[] = { 111,222,333,444 };
MQTT client(server, 1883, callback);
#define DEVICENAME "DEVICENAME"
#define MQTT_USR "username"
#define MQTT_PW "password"
