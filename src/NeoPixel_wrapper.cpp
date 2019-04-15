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
}
void NeoPixel_wrapper::setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b){
        this->currentColor.setRGBL(r,g,b,currentColor.getIntL());
        for(int j=0; j<this->strip.numPixels(); j++) {
                this->strip.setPixelColor(j,r,g,b);
        }
}
void NeoPixel_wrapper::setPixelColor(uint8_t r, uint8_t g, uint8_t b){
        this->setPixelColor(0, r, g, b);
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
void NeoPixel_wrapper::setBrightness(uint8_t l){
        this->currentColor.setL(l);
        this->strip.setBrightness(l);
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
uint32_t NeoPixel_wrapper::Color(uint8_t r, uint8_t g, uint8_t b){
        return this->strip.Color(r,g,b);
}
colorPoint NeoPixel_wrapper::getPixelColor() {
        return colorPoint(this->currentColor);
}
