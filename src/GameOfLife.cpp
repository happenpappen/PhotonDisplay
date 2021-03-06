#include "PhotonDisplay.h"

#define DELAY 1000
#define DENSITY 40

const uint8_t kMatrixWidth  = 128;
const uint8_t kMatrixHeight  = 16;

extern uint16_t fg_color;
extern uint16_t bg_color;

byte world[kMatrixWidth][kMatrixHeight][2];
uint8_t living_cells = 0;
uint8_t living_cells_old = 0;

int neighbours(int x, int y)
{
    return world[(x + 1) % kMatrixWidth][y][0] +
           world[x][(y + 1) % kMatrixHeight][0] +
           world[(x + kMatrixWidth - 1) % kMatrixWidth][y][0] +
           world[x][(y + kMatrixHeight - 1) % kMatrixHeight][0] +
           world[(x + 1) % kMatrixWidth][(y + 1) % kMatrixHeight][0] +
           world[(x + kMatrixWidth - 1) % kMatrixWidth][(y + 1) % kMatrixHeight][0] +
           world[(x + kMatrixWidth - 1) % kMatrixWidth][(y + kMatrixHeight - 1) % kMatrixHeight][0] +
           world[(x + 1) % kMatrixWidth][(y + kMatrixHeight - 1) % kMatrixHeight][0];
}

void init_matrix()
{

    for (int x = 0; x < kMatrixWidth; x++) {
        for (int y = 0; y < kMatrixHeight; y++) {
            if (random(100) < DENSITY) {
                world[x][y][0] = 1;
            } else {
                world[x][y][0] = 0;
            }
            world[x][y][1] = 0;
        }
    }
}

void setupGameOfLife()
{
    randomSeed(analogRead(0));                               // Zufallsfunktion initialisieren

    matrix.begin();
    // Fill matrix with initial values:
    init_matrix();
    // Clear background
    matrix.fillScreen(0);
    matrix.swapBuffers(true);
}

void loopGameOfLife()
{
    int count = 0;
    int x = 0;
    int y = 0;

    // Birth and death cycle
    living_cells_old = living_cells;
    living_cells = 0;

    for (x = 0; x < kMatrixWidth; x++) {
        for (y = 0; y < kMatrixHeight; y++) {
            // Default is for cell to stay the same
            world[x][y][1] = world[x][y][0];
            count = neighbours(x, y);
            if (count == 3 && world[x][y][0] == 0) {
                // A new cell is born
                world[x][y][1] = 1;
            }
            if ((count < 2 || count > 3) && world[x][y][0] == 1) {
                // Cell dies
                world[x][y][1] = 0;
            }
            if (world[x][y][1]) {
                living_cells++;
            }
        }
    }

    if (living_cells == 0) {
        init_matrix();
    } else {
        // Copy next generation into place
        for (x = 0; x < kMatrixWidth; x++) {
            for (y = 0; y < kMatrixHeight; y++) {
                world[x][y][0] = world[x][y][1];
                if (world[x][y][1] != 0) {
                    matrix.writePixel(x, y, fg_color);
                } else {
                    matrix.writePixel(x, y, bg_color);
                }
            }
        }
    }

    matrix.swapBuffers(true);
    delay(DELAY);
}