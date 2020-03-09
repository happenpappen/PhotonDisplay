#include "PhotonDisplay.h"
#include "math.h"

// Text to display:
//
String displayText;

extern uint16_t fg_color;
extern uint16_t bg_color;

int    textX   = matrix.width(),
       textMin = displayText.length() * -6;

String getScrollText() {
    return displayText;
}

void setScrollText(String newText) {
    displayText = newText;
    displayText.replace("ä","ae");
    displayText.replace("ü","ue");
    displayText.replace("ö","oe");
    displayText.replace("Ä","Ae");
    displayText.replace("Ü","Ue");
    displayText.replace("Ö","Oe");
    displayText.replace("ß","ss");
    displayText.replace("\n"," ");
    displayText.replace("\r\n"," ");
    textX = matrix.width();
    textMin = displayText.length() * -6;
    saveSettings();
}

void setupScrollText() {

  matrix.setTextColor(matrix.ColorHSV(255, 255, brightness, true));

  if (displayText == "") 
  { 
      displayText = "Wenn Fledermaeuse unruhig flattern, \
Sargdeckel gespenstisch knattern, \
Geister ueber den Friedhof schleichen, \
auferstehen blasse Leichen; \
Wenn feuchtes Laub daemonisch knirscht, \
der Totengraeber lautlos pirscht, \
Hexen mit dem Besen fliegen, \
Vampire ihre Nahrung kriegen; \
Wenn Menschenfressers Maegen knurren, \
schwarze Katzen leise schnurren, \
Woelfe heulen in der Nacht, \
der Tod aus seinem Schlaf erwacht; \
Wenn Skelette klapprig schreiten, \
Kobolde auf Knochen reiten, \
der Satan laut nach Gnade schreit, \
dann ist Halloween nicht weit.";
      textMin = displayText.length() * -6;
  }
}

void loopScrollText() {

  // Clear background
  matrix.fillScreen(bg_color);

  // Draw big scrolly text on top
  matrix.setTextColor(matrix.ColorHSV(255, 255, brightness, true));
  matrix.setCursor(textX, 12);
  matrix.print(displayText);

  // Move text left (w/wrap)
  if((--textX) < textMin) textX = matrix.width();

  // Update display
  matrix.swapBuffers(true);
}
