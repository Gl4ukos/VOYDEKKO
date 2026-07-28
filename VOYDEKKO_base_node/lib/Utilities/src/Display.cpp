#include "Display.hpp"

Display_Interface::Display_Interface(){
    Wire.begin(21,22);
    display = Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
}


int Display_Interface::setup_display(){

    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)){
        return -1;
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0,0);

    display.println("Screen online.");
    display.display();

    return 0;
}


void Display_Interface::update_display(){

    display.clearDisplay();
    display.setCursor(0,0);
    display.println("...");
    display.display();

}