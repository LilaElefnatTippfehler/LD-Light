#ifndef NEOPIXELWRAPPER_H_
#define NEOPIXELWRAPPER_H_

#include <neopixel.h>
#include "colorPoint.h"

/**
   This is a wrapper for Adafruit NeoPixel library when you only want to use the
   whole strip in one color.
 */
class NeoPixel_wrapper {
private:
colorPoint currentColor;
Adafruit_NeoPixel strip;
public:
NeoPixel_wrapper(uint16_t n, uint8_t p, uint8_t t);
void begin(void);
void show(void);
void setPin(uint8_t p);
void setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b);
void setPixelColor(uint8_t r, uint8_t g, uint8_t b);
void setPixelColor(uint16_t n, uint32_t c);
void setPixelColor(uint32_t c);
void setBrightness(uint8_t);
uint8_t getBrightness();
uint8_t *getPixels();
uint16_t numPixels(void);
uint32_t Color(uint8_t r, uint8_t g, uint8_t b);
colorPoint getPixelColor();
};

#endif
