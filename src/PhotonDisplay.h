#ifndef __PHOTONLAMP_H__
#define __PHOTONLAMP_H__

#include "Adafruit_GFX.h"   // Core graphics library
#include "RGBmatrixPanel.h" // Hardware-specific library

extern RGBmatrixPanel matrix;
extern uint16_t brightness;

void loadSettings();
void saveSettings();

#endif
