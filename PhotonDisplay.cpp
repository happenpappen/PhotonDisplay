#include "Adafruit_mfGFX.h"   // Core graphics library
#include "RGBmatrixPanel.h" // Hardware-specific library
#include "math.h"
#include "MQTT.h"
#include "MQTT_credentials.h"

//        Parameters: A, B, C, CLK, LAT, OE, <double buffer>, <width>
//
RGBmatrixPanel matrix(A0, A1, A2, D6, A4, D7, true, 128);

// Display can be switched on and off:
//
bool displayEnabled = true;

// Text to display:
//
String displayText ="Wenn Fledermaeuse unruhig flattern, \
Sargdeckel gespenstisch knattern, \
Geister ueber den Friedhof schleichen, \
auferstehen blasse Leichen; \
 \
Wenn feuchtes Laub daemonisch knirscht, \
der Totengraeber lautlos pirscht, \
Hexen mit dem Besen fliegen, \
Vampire ihre Nahrung kriegen; \
 \
Wenn Menschenfressers Maegen knurren, \
schwarze Katzen leise schnurren, \
Woelfe heulen in der Nacht, \
der Tod aus seinem Schlaf erwacht; \
 \
Wenn Skelette klapprig schreiten, \
Kobolde auf Knochen reiten, \
der Satan laut nach Gnade schreit, \
dann ist Halloween nicht weit.";

int    textX   = matrix.width(),
       textMin = displayText.length() * -12,
       hue     = 0;

int8_t ball[3][4] = {
  {  3,  0,  1,  1 }, // Initial X,Y pos & velocity for 3 bouncy balls
  { 17, 15,  1, -1 },
  { 27,  4, -1,  1 }
};

static const uint16_t ballcolor[3] = {
  0x0080, // Green=1
  0x0002, // Blue=1
  0x1000  // Red=1
};

static uint16_t fg_color = 0x0080;
static uint16_t bg_color = 0x0000;

void mqtt_callback(char *, byte *, unsigned int);
void saveSettings();
void loadSettings();
void setText(String newText);
int setBgColor(String newColor);
int setFgColor(String newColor);

// Timer functions:
//
void publishState();
Timer PublisherTimer(5000, publishState);

MQTT client(MQTT_HOST, 1883, mqtt_callback);

ApplicationWatchdog wd(60000, System.reset);

void mqtt_callback(char *topic, byte *payload, unsigned int length)
{
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

    client.publish("/"+myID+"/state/LastPayload", "Last Payload: " + String(myPayload));

    if (myTopic == "/"+myID+"/set/Text") {
        setText(String(myPayload));
        stateChanged = true;
    }

    if (myTopic == "/"+myID+"/set/ForgroundColor") {
        setFgColor(String(myPayload));
        stateChanged = true;
    }

    if (myTopic == "/"+myID+"/set/BackgroundColor") {
        setBgColor(String(myPayload));
        stateChanged = true;
    }

    if (stateChanged) {
        publishState();
    }

    free(myPayload);
    myPayload = NULL;
}

String getText()
{
  return displayText;
}

void setText(String newText)
{
  displayText = newText;
  textX   = matrix.width();
  textMin = displayText.length() * -12;
}

String getFgColor()
{
    return String::format("%0.6x,", fg_color);
}

int setFgColor(String newColor)
{
    fg_color = atoi(newColor);

    saveSettings();
    return 1;
}

String getBgColor()
{
    return String::format("%0.6x", bg_color);
}

int setBgColor(String newColor)
{
  bg_color = atoi(newColor);

  saveSettings();
  return 1;
}

int enableDisplay(String command)
{
    displayEnabled = true;

    saveSettings();
    return 1;
}

int disableDisplay(String command)
{
    displayEnabled = false;

    saveSettings();
    return 1;
}

void publishState()
{

    String myID = System.deviceID();

    if (!client.isConnected()) {
        client.connect(myID, MQTT_USER, MQTT_PASSWORD);
    }

    if (client.isConnected()) {
        client.publish("/"+myID+"/state/Text", String(getText()));
        client.publish("/"+myID+"/state/ForgroundColor", getFgColor());
        client.publish("/"+myID+"/state/BackgroundColor", getBgColor());
        client.publish("/"+myID+"/state/FirmwareVersion", System.version());
    }
}

void loadSettings()
{
    int address = 1;
    fg_color = EEPROM.read(address++);
    bg_color = EEPROM.read(address++);
    displayEnabled = EEPROM.read(address++);
}

void saveSettings()
{
    int address = 1;
    EEPROM.write(address++, fg_color);
    EEPROM.write(address++, bg_color);
    EEPROM.write(address++, displayEnabled);
}

void setup() {

  loadSettings();

  matrix.begin();
  matrix.setFont(COMICS_8);
  matrix.setTextWrap(false); // Allow text to run off right edge
  //matrix.setTextSize(2);
  matrix.setTextColor(matrix.ColorHSV(255, 255, 100, true));
  
  client.connect(System.deviceID(), MQTT_USER, MQTT_PASSWORD); // uid:pwd based authentication

  if (client.isConnected()) {
      PublisherTimer.start();
      client.subscribe("/"+System.deviceID()+"/set/+");
  }
}

void loop() {
  byte i;

  // Clear background
  matrix.fillScreen(0);

  // Draw big scrolly text on top
  matrix.setTextColor(matrix.ColorHSV(hue, 255, 100, true));
  matrix.setCursor(textX, 4);
  matrix.print(displayText);
  //matrix.setCursor(textX, 9);
  //matrix.print(displayText);

  // Move text left (w/wrap), increase hue
  if((--textX) < textMin) textX = matrix.width();
  hue += 7;
  if(hue >= 1536) hue -= 1536;

  // Update display
  matrix.swapBuffers(false);
  delay(16);

  if (client.isConnected()) {
      client.loop();
  }
}
