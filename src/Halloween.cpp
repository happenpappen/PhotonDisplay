#include "Adafruit_GFX.h"
#include "PhotonDisplay.h"
#include "math.h"


void setupHalloween() {

  matrix.begin();
  //matrix.setFont(ARIAL_8);
  matrix.setTextWrap(false); // Allow text to run off right edge
  matrix.setTextColor(matrix.ColorHSV(255, 255, brightness, true));
}

void loopHalloween() {
  byte i;

  // Clear background
  matrix.fillScreen(0);

  // Draw big scrolly text on top
  matrix.setTextColor(matrix.ColorHSV(255, 255, brightness, true));
  matrix.setCursor(0, 4);

  // Update display
  matrix.swapBuffers(false);
}
