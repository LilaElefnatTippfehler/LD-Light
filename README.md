# LD-Light
Small project with particles photon board for me and my girlfriend. Lamp that activates on touch in an random color and also activates all other lamps in the network with the same color. /n
Project is adaped from https://www.instructables.com/id/Networked-RGB-Wi-Fi-Decorative-Touch-Lights/ but the code is changed in a way that it can be run without the raspberry server and only over the particel cloude. /n
Adaption for more than 2 lamps is easily possible, but I only built the project for 2 lamps. /n
/n
Also supports Google Assistant now. Use IFTTT to build your own trigger words. /n
To work with the programm these trigger words have to publish Events in the cloud in this form: /n
/n
Event Name      Data/n
finalColor0     color     //the 0 after finalColor is the Identifier that the color comes from Google Assistant/n
                          //Instead of color use the supported colors in English or German as trigger words in IFTTT/n
/n
Google          Reset     //Resets all devices/n
Google          TurnOffx  //Turns off device x/n
