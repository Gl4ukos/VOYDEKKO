#include "Display.hpp"

Display_Interface::Display_Interface(){
    Wire.begin(21,22);
    display = Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
}


int Display_Interface::setup_display(){

    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)){
        return -1;
    }
    display.setRotation(0);
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0,0);
    display.display();

    return 0;
}


void Display_Interface::update(String text){

    display.clearDisplay();
    display.setCursor(0,0);
    display.print(text);
    display.display();

}

void Display_Interface::print(String text){
    display.print(text);
    display.display();
}

void Display_Interface::println(String text){
    display.println(text);
    display.display();
}

void Display_Interface::update_line(int line, String text)
{
    int y = line * 8;

    display.fillRect(0, y, 128, 8, SSD1306_BLACK);

    display.setCursor(0, y);
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
    display.print(text);

    display.display();
}