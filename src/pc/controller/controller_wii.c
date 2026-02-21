// File: src/pc/controller/controller_wii.c

#include <ogcsys.h>

// Global variable to signal exit to Wii menu
int exitToMenu = 0;

void controller_wii_read() {
    // Existing code...

    // Check if HOME button is pressed
    if (PAD_ButtonsDown(0) & PAD_BUTTON_HOME) {
        exitToMenu = 1; // Signal to exit to Wii menu
    }

    // Existing code...
}