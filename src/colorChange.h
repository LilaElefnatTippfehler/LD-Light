#ifndef COLORCHANG_H_
#define COLORCHANG_H_

#include <stdint.h>
#include "colorVector.h"
#include "colorPoint.h"
#include "NeoPixel_wrapper.h"

#define SERIAL_COLORCHANGE true

namespace colorChange
{
void init(NeoPixel_wrapper *strip,uint8_t pixelPin);
void setColor(NeoPixel_wrapper *strip, colorPoint newColor);
void setBrightness(NeoPixel_wrapper *strip, colorPoint brightness);
void setBrightness(NeoPixel_wrapper *strip, uint8_t brightness);
void setColorAndBright(NeoPixel_wrapper *strip, colorPoint newColor);
}

#endif
