#include "Adafruit_GFX.h"   // Core graphics library
#include "fonts/DroidSans.h"
#include "RGBmatrixPanel.h" // Hardware-specific library
#include "math.h"
#include "MQTT.h"
#include "MQTT_credentials.h"
#include "Halloween.h"
#include "Pong.h"
#include "ScrollText.h"
#include "Weather.h"
#include "SpaceInvader.h"
#include "LiveTicker.h"

#define SETTINGS_MAGIC 46
#define SETTINGS_START 1

#define CLK	D6
#define OE 	D7
#define LAT	A4
#define A  	A0
#define B  	A1
#define C  	A2

extern String displayText;

// Last parameter = 'true' enables double-buffering, for flicker-free,
// buttery smooth animation.  Note that NOTHING WILL SHOW ON THE DISPLAY
// until the first call to swapBuffers().  This is normal.
RGBmatrixPanel matrix(A, B, C, CLK, LAT, OE, true, 128);

// Display can be switched on and off:
//
bool displayEnabled = true;

#define ONE_DAY_MILLIS (24 * 60 * 60 * 1000)
unsigned long lastSync = millis();

static uint16_t fg_color = 0x0080;
static uint16_t bg_color = 0x0000;
uint16_t brightness = 255;
static uint16_t dispMode = 1;

// Display modes:
// 1: Scrolltext
// 2: Weather
// 3: Bundesliga Live-Ticker
// 4: Pong
// 5: Invader
// 6: Halloween

void mqtt_callback(char *, byte *, unsigned int);
void saveSettings();
void loadSettings();
int setBgColor(String newColor);
int setFgColor(String newColor);
int getBrightness(String command);
int setBrightness(String value);
int setDisplayMode(String command);
int getDisplayMode(String command);
int disableDisplay(String command);
int enableDisplay(String command);

// Timer functions:
//
void publishState();
Timer PublisherTimer(5000, publishState);

MQTT client(MQTT_HOST, 1883, mqtt_callback);

ApplicationWatchdog wd(60000, System.reset);

void mqtt_callback(char *topic, byte *payload, unsigned int length) {
    String myTopic = String(topic);

    bool stateChanged = false;
    char *myPayload = NULL;

    myPayload = (char *) malloc(length + 1);

    memcpy(myPayload, payload, length);
    myPayload[length] = 0;

    String myID = System.deviceID();

    if (!client.isConnected()) {
        client.connect(myID, MQTT_USER, MQTT_PASSWORD);
    }

    client.publish("/" + myID + "/state/LastPayload", String(myPayload));

    if (myTopic == "/"+myID+"/set/DisplayMode") {
        setDisplayMode(String(myPayload));
        stateChanged = true;
    }

    if (myTopic == "/"+myID+"/set/Text") {
        displayText=String(myPayload);
        stateChanged = true;
    }

    if (myTopic == "/"+myID+"/set/DisplayEnable") {
        if (String(myPayload) == "1") {
            enableDisplay("");
        } else {
            disableDisplay("");
        }
        stateChanged = true;
    }

    if (myTopic == "/" + myID + "/set/Text") {
        setScrollText(String(myPayload));
        stateChanged = true;
    }

    if (myTopic == "/" + myID + "/set/ForgroundColor") {
        setFgColor(String(myPayload));
        stateChanged = true;
    }

    if (myTopic == "/" + myID + "/set/BackgroundColor") {
        setBgColor(String(myPayload));
        stateChanged = true;
    }
    
    if (myTopic == "/"+myID+"/set/Brightness") {
        setBrightness(String(myPayload));
        stateChanged = true;
    }
    
    if (stateChanged) {
        publishState();
    }

    free(myPayload);
    myPayload = NULL;
}

int getBrightness(String command)
{
    return brightness;
}

int setBrightness(String value)
{
    brightness = atoi(value);
    brightness = brightness < 50 ? 50 : brightness;
    brightness = brightness > 255 ? 255 : brightness;

    saveSettings();
    return brightness;
}

String getFgColor() {
    return String::format("%0.6x", fg_color);
}

int setFgColor(String newColor) {
    fg_color = atoi(newColor);

    saveSettings();
    return 1;
}

String getBgColor() {
    return String::format("%0.6x", bg_color);
}

int setBgColor(String newColor) {
    bg_color = atoi(newColor);

    saveSettings();
    return 1;
}

int enableDisplay(String command) {

    displayEnabled = true;

    saveSettings();
    return 1;
}

int disableDisplay(String command) {

    displayEnabled = false;

    saveSettings();
    return 1;
}

int getDisplayMode(String command)
{
    return dispMode;
}

int setDisplayMode(String command)
{
    dispMode = atoi(command);

    switch (dispMode) {
    case 1:
        setupScrollText();
        break;
    case 2:
        setupWeather();
        break;
    case 3:
        setupLiveTicker();
        break;
    case 4:
        setupPong();
        break;
    case 5:
        setupSpaceInvader();
        break;
    case 6:
        setupHalloween();
        break;
    default:
        break;
    }
    
    saveSettings();
    return dispMode;
}


void publishState() {

    String myID = System.deviceID();
    
    if (!client.isConnected()) {
        client.connect(myID, MQTT_USER, MQTT_PASSWORD);
    }

    if (client.isConnected()) {
        client.publish("/"+myID+"/state/DisplayMode", String(getDisplayMode("")));
        if (displayEnabled) {
            client.publish("/" + myID + "/state/DisplayEnabled", "true");
        } else {
            client.publish("/" + myID + "/state/DisplayEnabled", "false");
        }
        client.publish("/" + myID + "/state/Text", getScrollText());
        client.publish("/" + myID + "/state/TextLength", String(getScrollText().length()));
        client.publish("/" + myID + "/state/ForgroundColor", getFgColor());
        client.publish("/" + myID + "/state/BackgroundColor", getBgColor());
        client.publish("/" + myID + "/state/Brightness", String(getBrightness("")));
        client.publish("/" + myID + "/state/FirmwareVersion", System.version());
    }
}

void loadSettings() {

    uint16_t maxSize = EEPROM.length();
    uint16_t address = 1;
    uint8_t version = 0;
    char loadArray[2048];

    EEPROM.get(address++, version);

    if (version == SETTINGS_MAGIC) { // Valid settings in EEPROM?
        EEPROM.get(address, dispMode);
        address = address + sizeof (dispMode);
        EEPROM.get(address, fg_color);
        address = address + sizeof (fg_color);
        EEPROM.get(address, bg_color);
        address = address + sizeof (bg_color);
        EEPROM.get(address, brightness);
        address = address + sizeof (brightness);
        EEPROM.get(address, displayEnabled);
        address = address + sizeof (displayEnabled);
        EEPROM.get(address,loadArray);
        displayText=String(loadArray);
    } else {
        dispMode = 1;
        fg_color=0x0080;
        bg_color=0x0000;
        brightness=255;
        displayEnabled=1;
        displayText="Hello World!";
        saveSettings();
    }
}

void saveSettings() {

    uint16_t address = SETTINGS_START;
    uint16_t maxlength = EEPROM.length();
    char saveArray[2048];

    if (displayText.length() < 2048) {
        displayText.toCharArray(saveArray,displayText.length());
    } else {
        String("String to long! (a) (L:"+String(displayText.length())+")").toCharArray(saveArray,displayText.length());
    }

    EEPROM.write(address++, SETTINGS_MAGIC);
    EEPROM.put(address, dispMode);
    address = address + sizeof(dispMode);
    EEPROM.put(address, fg_color);
    address = address + sizeof(fg_color);
    EEPROM.put(address, bg_color);
    address = address + sizeof(bg_color);
    EEPROM.put(address, brightness);
    address = address + sizeof(brightness);
    EEPROM.put(address, displayEnabled);
    address = address + sizeof(displayEnabled);
    if (maxlength-address-1 < displayText.length()) {
        EEPROM.put(address, saveArray);
    } else {
        EEPROM.put(address, "String to long! (b) (L:"+String(maxlength-address-1)+")");
    }
}

void setup() {
  matrix.begin();
  matrix.setTextWrap(false); // Allow text to run off right edge
  matrix.setFont(&DroidSans6pt8b);
  matrix.setTextSize(1);

  loadSettings();

  switch (dispMode) {
    case 1:
        setupScrollText();
        break;
    case 2:
        setupWeather();
        break;
    case 3:
        setupLiveTicker();
        break;
    case 4:
        setupPong();
        break;
    case 5:
       setupSpaceInvader();
        break;
    case 6:
        setupHalloween();
        break;
      default:
          break;
  }
  
  randomSeed(analogRead(A6) + micros());
  Time.zone(+1);
  Time.setDSTOffset(0);
  Time.beginDST();
  
  client.connect(System.deviceID(), MQTT_USER, MQTT_PASSWORD); // uid:pwd based authentication
  if (client.isConnected()) {
      PublisherTimer.start();
      client.subscribe("/" + System.deviceID() + "/set/+");
  }
}

void loop() {

    // once a day sync time with cloud:
    if (millis() - lastSync > ONE_DAY_MILLIS) {
        // Request time synchronization from the Particle Cloud
        Particle.syncTime();
        lastSync = millis();
    }

    if (displayEnabled) {
        switch (dispMode) {
        case 1:
            loopScrollText();
            break;
        case 2:
            loopWeather();
            break;
        case 3:
            loopLiveTicker();
            break;
        case 4:
            loopPong();
            break;
        case 5:
            loopSpaceInvader();
            break;
        case 6:
            loopHalloween();
            break;
        default:
            break;
        }
    } else {
        matrix.fillScreen(0);
    }
    
  delay(10);	// Slow down animation!

  if (client.isConnected()) {
     client.loop();
  }

}
