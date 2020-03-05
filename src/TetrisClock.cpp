#include "PhotonDisplay.h"
#include "math.h"
#include <TetrisMatrixDraw.h>

TetrisMatrixDraw tetris(matrix);

extern uint16_t fg_color;
extern uint16_t bg_color;

// Sets whether the clock should be 12 hour format or not.
bool twelveHourFormat = true;
unsigned long oneSecondLoopDue = 0;
bool showColon = true;
bool forceRefresh = true;
volatile bool finishedAnimating = false;
String lastDisplayedTime = "";

void handleColonAfterAnimation() {

  // It will draw the colon every time, but when the colour is black it
  // should look like its clearing it.
  uint16_t colour =  showColon ? tetris.tetrisWHITE : tetris.tetrisBLACK;
  // The x position that you draw the tetris animation object
  int x = twelveHourFormat ? -6 : 2;
  // The y position adjusted for where the blocks will fall from
  // (this could be better!)
  int y = 26 - (TETRIS_Y_DROP_DEFAULT * tetris.scale);
  tetris.drawColon(x, y, colour);
}

void setupTetrisClock() {
  
  matrix.fillScreen(tetris.tetrisBLACK);
  matrix.swapBuffers(true);
}

/*
// This method is for controlling the tetris library draw calls
void animationHandler()
{
  // Not clearing the display and redrawing it when you
  // dont need to improves how the refresh rate appears
  if (!finishedAnimating) {
    matrix.fillScreen(tetris.tetrisBLACK);
    finishedAnimating = tetris.drawNumbers(2, 26, showColon);
    matrix.showBuffers(true);
  }
}
*/

void loopTetrisClock()
{
  String timeString = "";
  String AmPmString = "";
  unsigned long now = millis();
  if (now > oneSecondLoopDue) {

    timeString = Time.format("%H:%M");

    // Only update Time if its different
    if (lastDisplayedTime != timeString)
    {
      lastDisplayedTime = timeString;
      tetris.setTime(timeString, forceRefresh);

      // Must set this to false so animation knows
      // to start again
      finishedAnimating = false;
    }
    showColon = !showColon;
    // To reduce flicker on the screen we stop clearing the screen
    // when the animation is finished, but we still need the colon to
    // to blink
    if (finishedAnimating)
    {
      handleColonAfterAnimation();
    }
    oneSecondLoopDue = now + 1000;
  }

  // Not clearing the display and redrawing it when you
  // dont need to improves how the refresh rate appears
  if (!finishedAnimating) {
    matrix.fillScreen(tetris.tetrisBLACK);
    finishedAnimating = tetris.drawNumbers(2, 26, showColon);
    matrix.swapBuffers(true);
  }
}
