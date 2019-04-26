#include "NeoPixel_wrapper.h"

NeoPixel_wrapper::NeoPixel_wrapper(uint16_t n, uint8_t p, uint8_t t) : strip(n,p,t), currentColor(){

}

void NeoPixel_wrapper::begin(void){
        this->strip.begin();
}
void NeoPixel_wrapper::show(void){
        this->strip.show();
}
void NeoPixel_wrapper::setPin(uint8_t p){
        this->strip.setPin(p);
        this->currentColor.setRGBL((float) (rand()%256), (float) (rand()%256), (float) (rand()%256),(float) 255);
        this->strip.show();
        this->setBrightness(0);

}
void NeoPixel_wrapper::setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b){
        this->currentColor.setRGBL(r,g,b,currentColor.getIntL());
        for(int j=0; j<this->strip.numPixels(); j++) {
                this->strip.setPixelColor(j,r,g,b);
        }
}
void NeoPixel_wrapper::setPixelColor(uint8_t r, uint8_t g, uint8_t b){
        this->setPixelColor((uint16_t)0,(uint8_t) r,(uint8_t) g,(uint8_t) b);
}
void NeoPixel_wrapper::setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b, uint8_t w){
        //not supporting this function atm;
}
void NeoPixel_wrapper::setPixelColor(uint8_t r, uint8_t g, uint8_t b, uint8_t w){
        this->setPixelColor((uint16_t)0,(uint8_t)r,(uint8_t)g,(uint8_t)b,(uint8_t)w);
}

void NeoPixel_wrapper::setPixelColor(uint16_t n, uint32_t c){
        uint8_t lum = currentColor.getIntL();
        this->currentColor.setRGBL(c);
        this->currentColor.setL(lum);
        for(int j=0; j<this->strip.numPixels(); j++) {
                this->strip.setPixelColor(j,c);
        }
}
void NeoPixel_wrapper::setPixelColor(uint32_t c){
        this->setPixelColor(0, c);
}

void NeoPixel_wrapper::setPixelColor(colorPoint c){
        this->setPixelColor(c.getIntR(), c.getIntG(), c.getIntB());
}
void NeoPixel_wrapper::setBrightness(uint8_t l){
        if(currentColor.getL() <= 20) {
                this->currentColor.setL(l);
                this->strip.setBrightness(l);
                this->setPixelColor(this->currentColor);
        }else{
                this->currentColor.setL(l);
                this->strip.setBrightness(l);
        }
}
void NeoPixel_wrapper::setColor(uint16_t aLedNumber, byte aRed, byte aGreen, byte aBlue){
        this->setPixelColor(aLedNumber, (uint8_t)aRed, (uint8_t)aGreen, (uint8_t)aBlue);
}
void NeoPixel_wrapper::setColor(uint16_t aLedNumber, byte aRed, byte aGreen, byte aBlue, byte aWhite){
        this->setPixelColor(aLedNumber, (uint8_t)aRed, (uint8_t)aGreen, (uint8_t)aBlue, (uint8_t)aWhite);
}
void NeoPixel_wrapper::setColorScaled(uint16_t aLedNumber, byte aRed, byte aGreen, byte aBlue, byte aScaling){
        //not supporting this function atm;
}
void NeoPixel_wrapper::setColorScaled(uint16_t aLedNumber, byte aRed, byte aGreen, byte aBlue, byte aWhite, byte aScaling){
        //not supporting this function atm;
}
void NeoPixel_wrapper::setColorDimmed(uint16_t aLedNumber, byte aRed, byte aGreen, byte aBlue, byte aBrightness){
        this->setPixelColor(aLedNumber, (uint8_t)aRed, (uint8_t)aGreen, (uint8_t)aBlue);
        this->setBrightness((uint8_t) aBrightness);
}
void NeoPixel_wrapper::setColorDimmed(uint16_t aLedNumber, byte aRed, byte aGreen, byte aBlue, byte aWhite, byte aBrightness){
        this->setPixelColor(aLedNumber, (uint8_t)aRed, (uint8_t)aGreen, (uint8_t)aBlue, (uint8_t)aWhite);
        this->setBrightness((uint8_t) aBrightness);
}
void NeoPixel_wrapper::updateLength(uint16_t n){
        this->strip.updateLength(n);
}
void NeoPixel_wrapper::clear(void){
        this->currentColor.setRGBL((uint8_t) 0,(uint8_t) 0,(uint8_t) 0,(uint8_t) 0);
        this->strip.clear();
}
uint8_t NeoPixel_wrapper::getBrightness(){
        return this->currentColor.getL();
}
uint8_t *NeoPixel_wrapper::getPixels(){
        return this->strip.getPixels();
}
uint16_t NeoPixel_wrapper::numPixels(void){
        return this->strip.numPixels();
}
uint16_t NeoPixel_wrapper::getNumLeds(void){
        return this->strip.getNumLeds();
}
uint32_t NeoPixel_wrapper::Color(uint8_t r, uint8_t g, uint8_t b){
        return this->strip.Color(r,g,b);
}
uint32_t NeoPixel_wrapper::Color(uint8_t r, uint8_t g, uint8_t b, uint8_t w){
        return this->strip.Color(r,g,b,w);
}
uint32_t NeoPixel_wrapper::getPixelColor(uint16_t n){
        return this->currentColor.getRGB();
}
colorPoint NeoPixel_wrapper::getPixelColor() {
        return colorPoint(this->currentColor);
}
byte NeoPixel_wrapper::brightnessToPWM(byte aBrightness){
        return (uint8_t) aBrightness;
}
