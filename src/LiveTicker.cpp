#include <ArduinoJson.h>
#include <HttpClient.h>
#include "PhotonDisplay.h"
#include "math.h"

// Text to display:
//
String displayTicker;

extern uint16_t fg_color;

// The type of data that we want to extract from the page
typedef struct {
  char teamname[32];
  uint8_t logoid;
} TeamData;

typedef struct {
  TeamData hometeam, awayteam;
  uint8_t homescore, awayscore;
} Fixture;

TeamData Teams[18];
Fixture Fixtures[9];

HttpClient httpClient;

// Headers currently need to be set at init, useful for API keys etc.
http_header_t headers[] = {
    //  { "Content-Type", "application/json" },
    //  { "Accept" , "application/json" },
    { "X-Auth-Token", "4fec375a62084fd08605f06e21c07035"},
    { "Accept" , "*/*"},
    { NULL, NULL } // NOTE: Always terminate headers will NULL
};

http_request_t request;
http_response_t response;

extern int textX;
extern int textMin;

// Parse the JSON from the input string and extract the interesting values
// Here is the JSON we need to parse
// {
//   "id": 1,
//   "name": "Leanne Graham",
//   "username": "Bret",
//   "email": "Sincere@april.biz",
//   "address": {
//     "street": "Kulas Light",
//     "suite": "Apt. 556",
//     "city": "Gwenborough",
//     "zipcode": "92998-3874",
//     "geo": {
//       "lat": "-37.3159",
//       "lng": "81.1496"
//     }
//   },
//   "phone": "1-770-736-8031 x56442",
//   "website": "hildegard.org",
//   "company": {
//     "name": "Romaguera-Crona",
//     "catchPhrase": "Multi-layered client-server neural-net",
//     "bs": "harness real-time e-markets"
//   }
// }
bool readReponseContent(TeamData* teamData) {
  // Compute optimal size of the JSON buffer according to what we need to parse.
  // See https://bblanchon.github.io/ArduinoJson/assistant/
  const size_t BUFFER_SIZE =
      JSON_OBJECT_SIZE(8)    // the root object has 8 elements
      + JSON_OBJECT_SIZE(5)  // the "address" object has 5 elements
      + JSON_OBJECT_SIZE(2)  // the "geo" object has 2 elements
      + JSON_OBJECT_SIZE(3);  // the "company" object has 3 elements
      //+ MAX_CONTENT_SIZE;    // additional space for strings

  unsigned char responseBody[response.body.length()+1];
  
  response.body.getBytes(responseBody,response.body.length());
  
  StaticJsonDocument<200> jsonBuffer;

  DeserializationError error = deserializeJson(jsonBuffer, (char *)responseBody);

  if (error) {
    Particle.publish("DEBUG","deserializeJson() failed",PRIVATE);
    return false;
  }

  const char* sensor = jsonBuffer["sensor"];
  long time = jsonBuffer["time"];
  double latitude = jsonBuffer["data"][0];
  double longitude = jsonBuffer["data"][1];

  if (jsonBuffer.isNull()) {
    Serial.println("JSON parsing failed!");
    Particle.publish("DEBUG","JSON parsing failed!",PRIVATE);
    return false;
  }

  // Here were copy the strings we're interested in
  strcpy(teamData->teamname, jsonBuffer["name"]);
  // It's not mandatory to make a copy, you could just use the pointers
  // Since, they are pointing inside the "content" buffer, so you need to make
  // sure it's still in memory when you read the string

  return true;
}

String updateTicker() {
    String apiRequest = "/v2/competitions/BL1/fixtures?matchday=11";
    String newText = "";
    textX = matrix.width();
    textMin = displayTicker.length() * -6;

    // Request path and body can be set at runtime or at setup.
    request.hostname = "api.football-data.org";
    request.port = 80;
    request.path = apiRequest;

    // The library also supports sending a body with your request:
    //request.body = "{\"key\":\"value\"}";

    // Get request
    httpClient.get(request, response, headers);

    Particle.publish("DEBUG","Response status: "+response.status,PRIVATE);
    Particle.publish("DEBUG","Response body: "+response.body,PRIVATE);

    TeamData teamData;
    readReponseContent(&teamData);

    return newText;
}

Timer TickerTimer(120000, updateTicker);

void setupLiveTicker() {

  matrix.setTextColor(matrix.ColorHSV(255, 255, brightness, true));

  TickerTimer.start();

  if (displayTicker == "") 
  { 
      displayTicker = updateTicker();
  }
}

void loopLiveTicker() {
  byte i;

  // Clear background
  matrix.fillScreen(0);

  // Draw big scrolly text on top
  matrix.setTextColor(matrix.ColorHSV(fg_color, 255, brightness, true));
  matrix.setCursor(textX, 10);
  matrix.print(displayTicker);

  // Move text left (w/wrap)
  if((--textX) < textMin) textX = matrix.width();

  // Update display
  matrix.swapBuffers(true);
}
