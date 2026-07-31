#include <Arduino.h>



void setup() {
    Serial.begin(115200);

    Serial.println("System online");
    delay(500);
}

void loop() {

    Serial.println("Test ...");
    delay(100);
}