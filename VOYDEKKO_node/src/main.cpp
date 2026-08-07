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

enum NodeState{
    HYBERNATING,
    TRANSMITTING,
    AWAITING_ACK,
    RETRANSMITTING,
    AWAITING_ACK_AGAIN
};

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


// Timing variables
uint32_t ack_timeout = 5000;
uint32_t ack_time_start;

uint32_t hybernation_duration = 2000;

int display_hybernation_status = 1; // remove

NodeState status = TRANSMITTING;

void hybernate(){
    delay(hybernation_duration); // !!! This is not actual sleep, fix 
}

void transmit_packet(){
    LoRa.beginPacket();
    LoRa.write((uint8_t*) &packet, sizeof(packet));
    LoRa.endPacket();
}

int listen_for_ack(){
    int packet_size = LoRa.parsePacket();
    if(packet_size == sizeof(ack_packet)){
        if(LoRa.available()){
            LoRa.readBytes((uint8_t*) &ack_packet, sizeof(ack_packet));
            return 1;
        }
    }
    return 0;
}

void loop() {
    switch(status){
        case HYBERNATING:
        {
            display.update_display("Hybernating...");
            hybernate();
            status = TRANSMITTING;
            break;
        }


        case TRANSMITTING:
        {
            packet.update_packet();
            transmit_packet();
            display.update_display(packet.to_string());
            status = AWAITING_ACK;
            break;
        }

        case AWAITING_ACK:
        {
            display.print_to_display("\nAwaiting response...");
            ack_time_start = millis();
            int response = 0;
            while(millis() - ack_time_start <= ack_timeout){
                response = listen_for_ack();
                if(response){
                    break;
                }
                delay(50);
            }
            if(response == 0){
                display.update_display(packet.to_string());
                display.print_to_display("No response from Base.");
                status = RETRANSMITTING;
            }else{
                if(ack_packet.id == packet.id){
                    if(ack_packet.status == SOLID){
                        display.update_display(packet.to_string());
                        display.print_to_display("\nTRNSMSSN SCCSSFL.");
                        status = HYBERNATING;
                    }else{
                        display.update_display(packet.to_string());
                        display.print_to_display("\nTRNSMSSN FLTY.");
                        status = HYBERNATING;
                    }
                }else{
                    display.update_display(packet.to_string());
                    display.print_to_display("\nPACKET LOSS!");
                    status = HYBERNATING;
                }
            }
            delay(500);
            break;
        }

        case RETRANSMITTING:
        {
            display.update_display(packet.to_string());
            display.print_to_display("\nRETRANSMITTING...");
            delay(200);
            transmit_packet();
            status = AWAITING_ACK;
            break;
        }
    }
}