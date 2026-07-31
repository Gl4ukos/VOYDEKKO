#include <Arduino.h>
#include "Display.hpp"

#include <SPI.h>
#include <LoRa.h>

#define LORA_SS      5
#define LORA_RST     27
#define LORA_DIO0    26

#define B1_pin 17
#define B2_pin 16
#define B3_pin 4

Display_Interface display;

void setup() {
    Serial.begin(115200);
    if(display.setup_display() != 0){
        Serial.println("Display error");
    }
    Serial.println("System online");
    delay(500);

    pinMode(B2_pin, INPUT_PULLUP);
    pinMode(B1_pin, INPUT_PULLUP);
    pinMode(B3_pin, INPUT_PULLUP);

    Serial.println("Starting LoRa...");

    LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

    if (!LoRa.begin(868E6)) {
        Serial.println("LoRa init failed!");
        while (true);
    }

    delay(500);
    Serial.println("LoRa OK!");

    LoRa.setTxPower(17); // dBm


}

void loop() {
    display.display.clearDisplay();
    display.display.setCursor(0,0);

    if (digitalRead(B1_pin) == LOW) {
        display.display.print("1");
    }
    if (digitalRead(B2_pin) == LOW) {
        display.display.print("2");
    }
    if (digitalRead(B3_pin) == LOW) {
        display.display.print("3");
    }


    Serial.println("Sending packet...");

    LoRa.beginPacket();
    LoRa.print("VoydEkko test packet");
    LoRa.endPacket();

    Serial.println("Packet sent");

    display.display.display();
    delay(100);




}