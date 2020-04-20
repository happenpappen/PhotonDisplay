#include "PhotonDisplay.h"
#include "SparkIntervalTimer.h"

#include "RGBAnimation.h"
#include "RGBNormal.h"
#include "RGBWeather.h"
#include "RGBWeatherAnimation.h"

#include "RGBSwypePath.h"

RGBAnimationClass* rgbWeatherAnim[] = { NULL, NULL, NULL, NULL };
const int numWeatherAnims = sizeof(rgbWeatherAnim)/sizeof(rgbWeatherAnim[0]);
 
WeatherInfoClass myWeather("Dortmund,DE", "weather_hook");
RGBSwypePathClass* swypePathWeather;

int setCode(String cmd) {
    int code = cmd.toInt();
    for (int i = 0; i < 8; i++)
        myWeather.code[i] = code + i;
    return code;
}

void updateWeather() {
    myWeather.refreshWeather();
    ((RGBWeatherAnimationClass*)rgbWeatherAnim[1])->refreshWeather();
    ((RGBWeatherAnimationClass*)rgbWeatherAnim[3])->refreshWeather();
}

Timer weatherUpdate(15 * 60 * 1000, updateWeather);

void setupWeather() {

    int i = 0;

    if (rgbWeatherAnim[i] == NULL) { rgbWeatherAnim[i++] = new RGBWeatherAnimationClass(matrix, 0, 0, &myWeather); }
    if (rgbWeatherAnim[i] == NULL) { rgbWeatherAnim[i++] = new RGBNormalClass(matrix, 16, 0); }
    if (rgbWeatherAnim[i] == NULL) { rgbWeatherAnim[i++] = new RGBWeatherAnimationClass(matrix, 64, 0, &myWeather); }
    if (rgbWeatherAnim[i] == NULL) { rgbWeatherAnim[i++] = new RGBNormalClass(matrix, 80, 0); }
    swypePathWeather = new RGBSwypePathClass(rgbWeatherAnim[i], RGBSwypePathClass::DOWN);
    
    updateWeather();
    weatherUpdate.start();
}

void loopWeather() {
    
    static uint32_t msPublish;
    static uint32_t msAnimation = 0;
    static uint32_t msNextDay = 0;
    static int day;
 
    if (millis() - msAnimation < 30) return;
    msAnimation = millis();

    if (millis() - msNextDay > 5000) {
        int state = swypePathWeather->getState();
        switch (state) {
            case RGBSwypePathClass::STOPPED:
                ((RGBSwypePathClass*) swypePathWeather)->start();
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
    
    for (int i = 0; i < numWeatherAnims; i++) {
        if (rgbWeatherAnim[i]) rgbWeatherAnim[i]->drawFrame(day);
    }
    
    matrix.swapBuffers(true);
}