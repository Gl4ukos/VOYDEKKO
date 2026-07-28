#include <Arduino.h>
#include <esp_now.h>


void setup() {
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
    Serial.print("Hello world \n");
    delay(20);
}
