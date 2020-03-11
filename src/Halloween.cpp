#include "PhotonDisplay.h"
#include "math.h"
#include "images/osterhasi/osterhasi_00.h"
#include "images/osterhasi/osterhasi_01.h"
#include "images/osterhasi/osterhasi_02.h"
#include "images/osterhasi/osterhasi_03.h"
#include "images/osterhasi/osterhasi_04.h"
#include "images/osterhasi/osterhasi_05.h"
#include "images/osterhasi/osterhasi_06.h"
#include "images/osterhasi/osterhasi_07.h"
#include "images/osterhasi/osterhasi_08.h"
#include "images/osterhasi/osterhasi_09.h"
#include "images/osterhasi/osterhasi_10.h"
#include "images/osterhasi/osterhasi_11.h"
#include "images/osterhasi/osterhasi_12.h"
#include "images/osterhasi/osterhasi_13.h"
#include "images/osterhasi/osterhasi_14.h"
#include "images/osterhasi/osterhasi_15.h"
#include "images/osterhasi/osterhasi_16.h"
#include "images/osterhasi/osterhasi_17.h"
#include "images/osterhasi/osterhasi_18.h"
#include "images/osterhasi/osterhasi_19.h"
#include "images/osterhasi/osterhasi_20.h"
/*#include "images/osterhasi/osterhasi_21.h"
#include "images/osterhasi/osterhasi_22.h"
#include "images/osterhasi/osterhasi_23.h"
#include "images/osterhasi/osterhasi_24.h"
#include "images/osterhasi/osterhasi_25.h"
#include "images/osterhasi/osterhasi_26.h"
#include "images/osterhasi/osterhasi_27.h"
#include "images/osterhasi/osterhasi_28.h"
 */

const uint16_t *osterhasi_images[] {
  osterhasi_00,
  osterhasi_01,
  osterhasi_02,
  osterhasi_03,
  osterhasi_04,
  osterhasi_05,
  osterhasi_06,
  osterhasi_07,
  osterhasi_08,
  osterhasi_09,
  osterhasi_10,
  osterhasi_11,
  osterhasi_12,
  osterhasi_13,
  osterhasi_14,
  osterhasi_15,
  osterhasi_16,
  osterhasi_17,
  osterhasi_18,
  osterhasi_19,
  osterhasi_20,
  /*osterhasi_21,
  osterhasi_22,
  osterhasi_23,
  osterhasi_24,
  osterhasi_25,
  osterhasi_26,
  osterhasi_27,
  osterhasi_28
 */};

int counter;

void setupHalloween() {

  // Clear background
  matrix.fillScreen(0);
  // Update display
  matrix.swapBuffers(true);
  // Clear background
  matrix.fillScreen(0);
  // Update display
  matrix.swapBuffers(true);
  counter = 0;
}

void loopHalloween() {

  // Clear background
  matrix.fillScreen(0);

  matrix.drawRGBBitmap(0, 0, (const uint16_t *)osterhasi_images[counter], 64, 16);
  matrix.drawRGBBitmap(64, 0, (const uint16_t *)osterhasi_images[counter], 64, 16);

  counter = counter+1;
  if (counter > 20) { counter = 0; }
  
  // Update display
  matrix.swapBuffers(true);
  delay(100);
}
