# LD-Light
Small project with particles photon board for me and my girlfriend. Lamp that activates on touch in an random color and also activates all other lamps in the network with the same color.

Project is adaped from https://www.instructables.com/id/Networked-RGB-Wi-Fi-Decorative-Touch-Lights/ but the code is changed in a way that it can be run without the raspberry server and only over the particel cloude.

Adaption for more than 2 lamps is easily possible, but I only built the project for 2 lamps.



Also supports Google Assistant now. Use IFTTT to build your own trigger words.

To work with the programm these trigger words have to publish Events in the cloud in this form:



Event Name     Data

finalColor0    color     //the 0 after finalColor is the Identifier that the color comes from Google Assistant

//Instead of color use the supported colors in English or German as trigger words in IFTTT
                          


Google         Reset     //Resets all devices

Google         TurnOffx  //Turns off device x

