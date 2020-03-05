#include "PhotonDisplay.h"
#include <SparkIntervalTimer.h>

#include "RGBAnimation.h"
#include "RGBPong.h"

#include "RGBSwypePath.h"

extern const uint32_t image_data_bvb[];

RGBAnimationClass* rgbPongAnim[] = { NULL, NULL };
const int numPongAnims = sizeof(rgbPongAnim)/sizeof(rgbPongAnim[0]);
 
RGBSwypePathClass* swypePathPong;

void setupPong() {

    int i = 0;

    if (rgbPongAnim[i] == NULL) { rgbPongAnim[i++] = new RGBPongClass(matrix, 16, 0); }
    if (rgbPongAnim[i] == NULL) { rgbPongAnim[i++] = new RGBPongClass(matrix, 80, 0); }
    swypePathPong = new RGBSwypePathClass(rgbPongAnim[i], RGBSwypePathClass::DOWN);
}

void loopPong() {
    
    static uint32_t msPublish;
    static uint32_t msAnimation = 0;
    static uint32_t msNextDay = 0;
    static int day;
 
    if (millis() - msAnimation < 30) return;
    msAnimation = millis();

    if (millis() - msNextDay > 5000) {
        int state = swypePathPong->getState();
        switch (state) {
            case RGBSwypePathClass::STOPPED:
                ((RGBSwypePathClass*) swypePathPong)->start();
                break;
            case RGBSwypePathClass::SWYPEIN:
                msNextDay = millis();
                day++;
                day %= 7;
                break;
            default:
                break;
        }
    }

    matrix.fillScreen(0); // ensure blank canvas

    for (int i = 0; i < numPongAnims; i++) {
        if (rgbPongAnim[i]) rgbPongAnim[i]->drawFrame(day);
    }

/*
 *     int x=0, y=0;

    for (y = 0; y < 16; y++) {
        for (int x = 0; x < 16; x++) {
          matrix.drawPixel(64+x, y, image_data_bvb[x+y*16]);
        }
    }   
*/    
    matrix.swapBuffers(true);
}
