#ifndef NEOPIXELWRAPPER_H_
#define NEOPIXELWRAPPER_H_

#include <stdint.h>
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
void setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b, uint8_t w);
void setPixelColor(uint8_t r, uint8_t g, uint8_t b, uint8_t w);
void setPixelColor(uint16_t n, uint32_t c);
void setPixelColor(uint32_t c);
void setPixelColor(colorPoint c);
void setBrightness(uint8_t);
void setColor(uint16_t aLedNumber, byte aRed, byte aGreen, byte aBlue);
void setColor(uint16_t aLedNumber, byte aRed, byte aGreen, byte aBlue, byte aWhite);
void setColorScaled(uint16_t aLedNumber, byte aRed, byte aGreen, byte aBlue, byte aScaling);
void setColorScaled(uint16_t aLedNumber, byte aRed, byte aGreen, byte aBlue, byte aWhite, byte aScaling);
void setColorDimmed(uint16_t aLedNumber, byte aRed, byte aGreen, byte aBlue, byte aBrightness);
void setColorDimmed(uint16_t aLedNumber, byte aRed, byte aGreen, byte aBlue, byte aWhite, byte aBrightness);
void updateLength(uint16_t n);
void clear(void);

uint8_t getBrightness();
uint8_t *getPixels();
uint16_t numPixels(void);
uint16_t getNumLeds(void);
uint32_t Color(uint8_t r, uint8_t g, uint8_t b);
uint32_t Color(uint8_t r, uint8_t g, uint8_t b, uint8_t w);
uint32_t getPixelColor(uint16_t n);
colorPoint getPixelColor();

byte brightnessToPWM(byte aBrightness);
};

#endif
