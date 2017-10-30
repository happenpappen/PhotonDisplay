#include "PhotonDisplay.h"
#include "SparkIntervalTimer.h"

#include "RGBAnimation.h"
#include "RGBAnalogClock.h"
#include "RGBNormal.h"
#include "RGBPlasma.h"
#include "RGBPong.h"
#include "RGBRainbow.h"
#include "RGBWeather.h"

#include "RGBSwypePath.h"
#include "WeatherInfo.h"

RGBAnimationClass* rgbAnimation[] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
const int numAnimations = sizeof(rgbAnimation)/sizeof(rgbAnimation[0]);
 
WeatherInfoClass myTestWeather("Dortmund,DE", "weather_hook");
RGBSwypePathClass* swypePath;
 
int setTestCode(String cmd) {
    int code = cmd.toInt();
    for (int i = 0; i < 8; i++)
        myTestWeather.code[i] = code + i;
    return code;
}

void updateTestWeather() {
    myTestWeather.refreshWeather();
    //((RGBWeatherAnimationClass*)rgbAnimation[1])->refreshWeather();
}

Timer weatherTestUpdate(15 * 60 * 1000, updateTestWeather);

int weatherAnimationIndex = -1;

void setupRGBTest() {

    int i = 0;
    
    // how to instantiate the different animations
    //rgbAnimation[i++] = new RGBPongClass(matrix, 16, 0);
    //rgbAnimation[i++] = new RGBPongClass(matrix, 72, 0);
    //rgbAnimation[i++] = new RGBPlasmaClass(matrix);
    //rgbAnimation[i++] = new RGBRainbowClass(matrix, 0, 0, 32, 32, 32, 32, RGBRainbowClass::HORIZONTAL);
    rgbAnimation[i++] = new RGBWeatherClass(matrix,  32, 16, "Dortmund,de", "weather_hook");
    //rgbAnimation[i++] = new RGBRainbowClass(matrix, 0, 0, 8, 8, 150, 20, true, RGBRainbowClass::HORIZONTAL);
    //rgbAnimation[i++] = new RGBRainbowClass(matrix, 24, 0, 8, 8, 150, 20, true,  RGBRainbowClass::VERTICAL);
    //rgbAnimation[i++] = new RGBRainbowClass(matrix, 0, 24, 8, 8, 150, 20, true,  RGBRainbowClass::TOPRIGHT_BOTTOMLEFT);
    //rgbAnimation[i++] = new RGBRainbowClass(matrix, 24, 24, 8, 8, 150, 20, true,  RGBRainbowClass::TOPLEFT_BOTTOMRIGHT);
    //rgbAnimation[i++] = new RGBRainbowClass(matrix, 49, 20, RGBRainbowClass::CENTER_RAY);
    //rgbAnimation[i++] = new RGBRainbowClass(matrix, 7, 7, 18, 18, 100, 20, true, RGBRainbowClass::CENTER_BOX);    
    //rgbAnimation[i++] = new RGBNormalClass(matrix, 0, 0, RGBNormalClass::SMALL);
    weatherAnimationIndex = i++;
    rgbAnimation[weatherAnimationIndex] = new RGBWeatherClass(matrix, 8, 16, &myTestWeather);
    
    //swypePath = new RGBSwypePathClass(rgbAnimation[weatherAnimationIndex], RGBSwypePathClass::DOWN);
    //rgbAnimation[weatherAnimationIndex]->add(swypePath);
    swypePath = (RGBSwypePathClass*) rgbAnimation[weatherAnimationIndex]->add(new RGBSwypePathClass(RGBSwypePathClass::RIGHT));
    
    /*
    rgbAnimation[i++] = new RGBAnalogClockClass(matrix, 0,  0, 32, 32, true
            ,matrix.Color444( 15, 15, 15)
            ,matrix.Color444(  0,  0, 15)
            ,matrix.Color444(  0, 15,  0)
            ,matrix.Color444(  7,  0,  0)  
            ); 
    */
    
   //rgbAnimation[0]->add(RGBSwypePathClass());

    updateTestWeather();
    weatherTestUpdate.start();
}

void loopRGBTest() {
    
     static uint32_t msPublish;
     static uint32_t msAnimation = 0;
     static uint32_t msNextDay = 0;
     static int day;
 
     if (millis() - msAnimation < 30) return;
     msAnimation = millis();

    if (millis() - msNextDay > 5000) {
        int state = swypePath->getState();
        switch (state) {
            case RGBSwypePathClass::STOPPED:
                ((RGBSwypePathClass*) swypePath)->start();
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
    for (int i = 0; i < numAnimations; i++) {
        if (rgbAnimation[i]) rgbAnimation[i]->drawFrame(day);
    }
    
    matrix.swapBuffers(true);
}