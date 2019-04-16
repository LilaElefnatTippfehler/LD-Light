#include "colorChange.h"

void colorChange::init(NeoPixel_wrapper *strip,uint8_t pixelPin){
        strip->setPin(pixelPin);
        strip->begin();
        strip->show();
}
void colorChange::setColor(NeoPixel_wrapper *strip, colorPoint newColor){
        colorPoint current = strip->getPixelColor();
        colorVector deltaColor = colorVector(current, newColor);
        colorVector norm = deltaColor.getNorm();
        float abs = deltaColor.getAbs();
        int32_t i = 0;
        if(SERIAL_COLORCHANGE) {
                Serial.print("abs "); Serial.println(abs);
                Serial.print("delta r/g/b ");
                Serial.print(deltaColor.getR()); Serial.print("/");
                Serial.print(deltaColor.getG()); Serial.print("/");
                Serial.println(deltaColor.getB());
                Serial.print("newColor r/g/b ");
                Serial.print(newColor.getR()); Serial.print("/");
                Serial.print(newColor.getG()); Serial.print("/");
                Serial.println(newColor.getB());
                Serial.print("current r/g/b ");
                Serial.print(current.getR()); Serial.print("/");
                Serial.print(current.getG()); Serial.print("/");
                Serial.println(current.getB());
                Serial.print("norm r/g/b ");
                Serial.print(norm.getR()); Serial.print("/");
                Serial.print(norm.getG()); Serial.print("/");
                Serial.println(norm.getB());
        }
        for(i=0; i<abs; i++) {
                float r = current.getR() + norm.getR();
                float g = current.getG() + norm.getG();
                float b = current.getB() + norm.getB();
                current.setRGBL(r, g, b);
                //for(int j=0; j<strip->numPixels(); j++) {
                strip->setPixelColor(current);
                //}
                strip->show();
                delay(5);
        }
}
void colorChange::setBrightness(NeoPixel_wrapper *strip, colorPoint brightness){
        colorChange::setBrightness(strip,brightness.getIntL());
}
void colorChange::setBrightness(NeoPixel_wrapper *strip, uint8_t brightness){
        int32_t deltaL = brightness - strip->getBrightness();
        if(SERIAL_COLORCHANGE) {
                Serial.print("deltaL "); Serial.println(deltaL);
                Serial.print("brightness "); Serial.println(strip->getBrightness());
        }
        if(deltaL < 0) {
                for(int32_t i = 0; i > deltaL; i--) {
                        strip->setBrightness(strip->getBrightness()-1);
                        strip->show();
                        delay(10);
                }
        }else{
                if(deltaL > 0) {
                        for(int32_t i = 0; i < deltaL; i++) {
                                strip->setBrightness(strip->getBrightness()+1);
                                strip->show();
                                delay(10);
                        }
                }
        }
}
void colorChange::setColorAndBright(NeoPixel_wrapper *strip, colorPoint newColor){
        colorPoint current = strip->getPixelColor();
        current.setL(strip->getBrightness());
        colorVector deltaColor = colorVector(current, newColor);
        colorVector norm = deltaColor.getNormAll();
        float abs = deltaColor.getAbsAll();
        int32_t i = 0;
        if(SERIAL_COLORCHANGE) {
                Serial.print("newColor r/g/b/Br ");
                Serial.print(newColor.getR()); Serial.print("/");
                Serial.print(newColor.getG()); Serial.print("/");
                Serial.print(newColor.getB()); Serial.print("/");
                Serial.println(newColor.getL());
                Serial.print("current r/g/b/Br ");
                Serial.print(current.getR()); Serial.print("/");
                Serial.print(current.getG()); Serial.print("/");
                Serial.print(current.getB()); Serial.print("/");
                Serial.println(current.getL());
                Serial.print("norm r/g/b/Br ");
                Serial.print(norm.getR()); Serial.print("/");
                Serial.print(norm.getG()); Serial.print("/");
                Serial.print(norm.getB()); Serial.print("/");
                Serial.println(norm.getL());
        }
        for(i=0; i<abs; i++) {
                float r = current.getR() + norm.getR();
                float g = current.getG() + norm.getG();
                float b = current.getB() + norm.getB();
                float l = current.getL() + norm.getL();
                current.setRGBL(r, g, b, l);
                //for(int j=0; j<strip->numPixels(); j++) {
                strip->setPixelColor(current);
                //}
                strip->setBrightness(current.getIntL());
                strip->show();
                delay(5);
        }
}
