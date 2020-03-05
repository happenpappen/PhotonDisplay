#include "PhotonDisplay.h"
#include "math.h"

float h[]={1,2,13,0,0,0,0,7,0,7,7,0,15,12,0,10};
float t=0.5;

void setupCurve() {

  matrix.begin();
  t=0.5;
}

void loopCurve() {
    float a=0.0;
    uint16_t c=0;

  // Clear background
  matrix.fillScreen(0);

  float x=0.0;
  float y=0.0;

  for (int i=0; i<1024; i++) {
      a=cos(i/256+t*(sin(t/8+i/32)+0.2)/8)*(i/64+1);
      x+=cos(a);
      y+=sin(a);
      c=h[matrix.readPixel(x+64,y+64)+1];
      matrix.drawPixel(x+64,y+64,c);
      matrix.drawPixel(64-x,64-y,c);
  }

  // Update display
  matrix.swapBuffers(true);

  t+=.001;
}
