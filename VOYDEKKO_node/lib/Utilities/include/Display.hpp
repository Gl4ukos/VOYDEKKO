#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

class Display_Interface {
public:
    Adafruit_SSD1306 display;

    Display_Interface();
    int setup_display();
    void update_display(String text);
    void print_to_display(String text);
};


#endif