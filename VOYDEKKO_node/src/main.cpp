#include <Arduino.h>
#include "Display.hpp"
#include "Packet.hpp"

#include <SPI.h>
#include <LoRa.h>

#define LORA_SS      5
#define LORA_RST     27
#define LORA_DIO0    26

#define B1_pin 17
#define B2_pin 16
#define B3_pin 4

Display_Interface display;
Packet packet;
int wait_for_response = 0;
AckPacket ack_packet;

void setup() {
    Serial.begin(115200);
    if(display.setup_display() != 0){
        Serial.println("Display error");
    }
    Serial.println("Display online.");
    display.print_to_display("Screen online.");
    delay(500);

    pinMode(B2_pin, INPUT_PULLUP);
    pinMode(B1_pin, INPUT_PULLUP);
    pinMode(B3_pin, INPUT_PULLUP);

    Serial.println("Starting LoRa...");

    LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

    if (!LoRa.begin(868E6)) {
        Serial.println("LoRa init failed!");
        display.print_to_display("LoRa Failed!");
        while (true);
    }
    LoRa.setSpreadingFactor(7);
    LoRa.setSignalBandwidth(125E3);
    LoRa.setCodingRate4(5);
    LoRa.enableCrc();

    delay(500);
    Serial.println("LoRa OK.");
    display.print_to_display("LoRa Online.");

    LoRa.setTxPower(17); // dBm
    delay(700);

    display.print_to_display("BOOT COMPLETED.");
    delay(1000);

    packet.prelude = 0b10101010;

}

uint32_t ack_timeout = 4000;
uint32_t ack_time_start;
int display_hybernation_status = 1;

void loop() {
 
    if(wait_for_response == 0){
        packet.update_packet();

        LoRa.beginPacket();
        LoRa.write((uint8_t*)&packet, sizeof(packet));
        LoRa.endPacket();
        display.update_display(packet.to_string());

        wait_for_response = 1;
        display.print_to_display("\nAwaiting response...");     
        LoRa.receive();
        ack_time_start = millis();
    }
    
    while(wait_for_response == 1){
        int packet_size = LoRa.parsePacket();
        if(packet_size == sizeof(ack_packet)){
            if(LoRa.available()){
                LoRa.readBytes((uint8_t*)&ack_packet, sizeof(ack_packet));
                if(ack_packet.id == packet.id){
                    if(ack_packet.status == SOLID){
                        display.update_display(packet.to_string());
                        display.print_to_display("\nARRIVED OK.");
                        delay(1000);
                        wait_for_response = 0;
                    }else if(ack_packet.status == CORRUPT){
                        display.update_display(packet.to_string());
                        display.print_to_display("\nARRIVED CORRUPT.");
                        delay(1000);
                        wait_for_response = 0;
                    }
                }else if(ack_packet.id > packet.id){
                    display.update_display(packet.to_string());
                    display.print_to_display("\nPACKET LOSS!");
                    delay(1000);
                    wait_for_response = 0;
                }
            }
        }
        if((millis() - ack_time_start) >= ack_timeout){
            wait_for_response = 0;
            display.update_display("No response from Base");
        }
        delay(50);
    }
    
    display.print_to_display("Hybernating...");
    delay(5000);
}