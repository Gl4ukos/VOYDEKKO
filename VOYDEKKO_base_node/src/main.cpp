

#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>
#include <Packet.hpp>
#include <cstdlib>

#define LORA_SS      5
#define LORA_RST     27
#define LORA_DIO0    26

#define DEBUG 1

int wait_for_packet = 1;
Packet packet;
AckPacket ack_packet;
Reconf_request reconfig_request_packet;
PROPOSED_CONFIG requested_config;
Reconf_commit reconfig_commit_packet;
PROPOSED_CONFIG curr_config;

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
    curr_config = HI;
    ack_packet.prop_config = curr_config;
}

void loop() {
    if(wait_for_packet == 1){
        // Serial.println("Listening for node...");
        while(1){
            int packetSize = LoRa.parsePacket();
            if (packetSize == sizeof(packet)) {
                if (LoRa.available()){
                    LoRa.readBytes((uint8_t*)&packet, sizeof(packet));
                    Serial.println(packet.to_string());
                    Serial.print("RSSI: ");
                    Serial.println(LoRa.packetRssi());
                    Serial.print("SNR: ");
                    Serial.println(LoRa.packetSnr());
                    Serial.print("RETRIES: ");
                    Serial.println(packet.retries);
                    Serial.print("CONF: ");
                    if(DEBUG){
                        switch(curr_config){
                            case HI:
                                Serial.println("HI");
                                break;
                            case LO:
                                Serial.println("LO");
                                break;
                            case MID:
                                Serial.println("MID");
                                break;
                        }
                    }else{
                        Serial.println(curr_config);
                    }
                    Serial.println();
                    
                    wait_for_packet = 0;
                    break;
                }
            }else if(packetSize == sizeof(reconfig_request_packet)){
                if(LoRa.available()){
                    if(DEBUG){
                        Serial.println("RECEIVED RECONF REQUEST.");
                    }
                    LoRa.readBytes((uint8_t*) &reconfig_request_packet, sizeof(reconfig_request_packet));
                    if(reconfig_request_packet.prop_config == requested_config){
                        reconfig_commit_packet.prop_config = requested_config;
                        if(DEBUG){
                            Serial.println("RECONFIGURATION COMPLETE.");
                        }
                        LoRa.beginPacket();
                        LoRa.write((uint8_t*)&reconfig_commit_packet, sizeof(reconfig_commit_packet));
                        LoRa.endPacket();
                        curr_config = requested_config;
                    }
                }
            }
            delay(10);
        }
    }
    else{
        Serial.println("Sending ACK");
        ack_packet.id = packet.id;
        
        requested_config = static_cast<PROPOSED_CONFIG>(rand() % 3);

        ack_packet.prop_config = requested_config;

        LoRa.beginPacket();
        LoRa.write((uint8_t*)&ack_packet, sizeof(ack_packet));
        LoRa.endPacket();

        LoRa.receive();
        wait_for_packet = 1;
    }
    delay(20);
}