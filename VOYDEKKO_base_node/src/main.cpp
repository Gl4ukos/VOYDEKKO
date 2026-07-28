#include <Arduino.h>
#include "Display.hpp"

Display_Interface display;

void setup() {
    Serial.begin(115200);
    if(display.setup_display() != 0){
        Serial.println("Display error");
    }
    delay(1000);
}

void loop() {
    Serial.println("System online");
    display.update_display();
    delay(100);

}