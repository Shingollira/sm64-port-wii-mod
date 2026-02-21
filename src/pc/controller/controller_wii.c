/* Variables */
#include <ogc/wpad.h>

bool exit_to_menu = false; // Declare global variable for exit to menu

void controller_wii_get_held(struct controller_data *data) {
    WPAD_ScanPads();  // Scanning for button presses
    
    // Check for HOME button
    if (WPAD_ButtonsHeld(0) & WPAD_BUTTON_HOME) {
        exit_to_menu = true;  // Set flag when HOME is pressed
    }

    // (Rest of the original code here...) 
}