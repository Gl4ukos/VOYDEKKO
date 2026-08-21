

#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>
#include <Packet.hpp>
#include <cstdlib>

#define LORA_SS      5
#define LORA_RST     27
#define LORA_DIO0    26

int wait_for_packet = 1;
Packet packet;
AckPacket ack_packet;

uint32_t ack_timeout = 2000;
uint32_t ack_time_start = millis();

void setup() {

    Serial.begin(115200);
    delay(1000);

    Serial.println("Starting LoRa...");

    LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

    if (!LoRa.begin(868E6)) {
        Serial.println("LoRa init failed!");
        while (true);
    }
    LoRa.setSpreadingFactor(12);
    LoRa.setSignalBandwidth(125E3);
    LoRa.setCodingRate4(5);
    LoRa.enableCrc();

    Serial.println("LoRa OK!");

    LoRa.setTxPower(17); // dBm
    ack_packet.prop_config = HI;
}

void loop() {
    if(wait_for_packet == 1){
        // Serial.println("Listening for node...");
        while(1){
            int packetSize = LoRa.parsePacket();
            if (packetSize == sizeof(packet)) {
                char byte;
                if (LoRa.available()){
                    LoRa.readBytes((uint8_t*)&packet, sizeof(packet));
                    Serial.println(packet.to_string());
                    Serial.print("RSSI: ");
                    Serial.println(LoRa.packetRssi());
                    Serial.print("SNR: ");
                    Serial.println(LoRa.packetSnr());
                    Serial.print("RETRIES: ");
                    Serial.println(packet.retries);
                    Serial.println();
                    
                    wait_for_packet = 0;
                    break;
                }
            }    
            delay(10);
        }
    }
    else{
        // Serial.println("Sending ACK");
        ack_time_start = millis();        
        while(millis() - ack_time_start <= ack_timeout){
            ack_packet.id = packet.id;
            ack_packet.status = SOLID;

            LoRa.beginPacket();
            LoRa.write((uint8_t*)&ack_packet, sizeof(ack_packet));
            LoRa.endPacket();

            delay(50);
        }
        LoRa.receive();
        wait_for_packet = 1;
    }
    delay(20);
}