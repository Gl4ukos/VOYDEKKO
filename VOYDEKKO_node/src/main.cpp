#include <Arduino.h>
#include "Display.hpp"
#include "Packet.hpp"
#include <esp_sleep.h>

#include <SPI.h>
#include <LoRa.h>

#define LORA_SS      5
#define LORA_RST     27
#define LORA_DIO0    26

#define B1_pin 17
#define B2_pin 16
#define B3_pin 4

Display_Interface display;
RTC_DATA_ATTR Packet packet;
int wait_for_response = 0;
AckPacket ack_packet;
Reconf_commit reconf_commit_packet;
Reconf_request reconf_request_packet;
PROPOSED_CONFIG curr_config = HI;
PROPOSED_CONFIG requested_config = curr_config;

// Timing variables
uint32_t ack_timeout = 4000;
uint32_t ack_time_start;
uint32_t reconf_commit_time_start;
uint32_t reconf_commit_timeout = 2000;

uint32_t hybernation_seconds = 5;
int MAX_TRANSMISSION_TRIES = 5;
int retransmission_tries = MAX_TRANSMISSION_TRIES;

enum NodeState{
    HYBERNATING,
    TRANSMITTING,
    AWAITING_ACK,
    RETRANSMITTING,
    RECONFIGURING
};
NodeState status = TRANSMITTING;


void update_display_pkt_info(){
    String result = "PKT_ID: ";
    result += String(packet.id);
    result += "  -";
    switch (curr_config)
    {
    case HI:
        result += "HI";
        break;
    case MID:
        result += "MID";
        break;
    case LO:
        result += "LO";
        break;
    }
    result += "-";
    display.update_line(0, result);
}
void update_display_link_info(){
    String result = "RSSI: ";
    result += String(LoRa.packetRssi());
    result += " SNR: ";
    result += String(LoRa.packetSnr());
    display.update_line(1, result);
}
void update_display_action(String action){
    display.update_line(2, action); 
}
void update_display_status(String state){
    display.update_line(3, state);
}

void setup() {
    Serial.begin(115200);
    if(display.setup_display() != 0){
        Serial.println("Display error");
    }
    Serial.println("Display online.");
    display.update_line(0, "Screen online.");
    delay(100);
    pinMode(B2_pin, INPUT_PULLUP);
    pinMode(B1_pin, INPUT_PULLUP);
    pinMode(B3_pin, INPUT_PULLUP);

    Serial.println("Starting LoRa...");

    LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

    if (!LoRa.begin(868E6)) {
        Serial.println("LoRa init failed!");
        display.update_line(1, "LoRa Failed!");
        while (true);
    }
    LoRa.setSpreadingFactor(12);
    LoRa.setSignalBandwidth(125E3);
    LoRa.setCodingRate4(5);
    LoRa.enableCrc();

    delay(100);
    Serial.println("LoRa OK.");
    display.update_line(1,"LoRa Online.");

    LoRa.setTxPower(17); // dBm
    display.update_line(2,"BOOT COMPLETED.");
    delay(250);

    packet.prelude = 0b10101010;
    ack_packet.prop_config = HI;
}




void hybernate(){
    delay(500);
    status = TRANSMITTING;
    // esp_sleep_enable_timer_wakeup((uint64_t)hybernation_seconds * 1000000ULL);
    // esp_deep_sleep_start();
}

void transmit_packet(){
    
    LoRa.beginPacket();
    // update_display_status("Begin transmit");
    // delay(100);
    LoRa.write((uint8_t*) &packet, sizeof(packet));
    // update_display_status("Transmit write");
    // delay(100);
    LoRa.endPacket();
    // update_display_status("End transmit");
    // delay(100);

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

int listen_for_reconf_commit(){
    int packet_size = LoRa.parsePacket();
    if(packet_size == sizeof(reconf_commit_packet)){
        if(LoRa.available()){
            LoRa.readBytes((uint8_t*) &reconf_commit_packet, sizeof(reconf_commit_packet));
            return 1;
        }
    }
    return 0;
}

void loop() {
    
    
    switch(status){
        case HYBERNATING:
        {
            display.display.clearDisplay();
            display.update_line(2,"  HYBERNATING... ");
            delay(250);
            display.display.clearDisplay();
            display.display.display();
            hybernate();
            break;
        }

        case TRANSMITTING:
        {
            packet.update_packet();
            packet.retries = 0;

            display.display.clearDisplay();
            update_display_pkt_info();
            update_display_action("TRANSMITTING...");

            transmit_packet();
            status = AWAITING_ACK;
            retransmission_tries = MAX_TRANSMISSION_TRIES;
            break;
        }

        case AWAITING_ACK:
        {
            display.display.clearDisplay();
            update_display_pkt_info();
            update_display_action("TRANSMITTING. ("+ String(packet.retries)+ ")");
            update_display_status("AWAITING RESPONSE...");
            ack_time_start = millis();
            int response = 0;
            while(millis() - ack_time_start <= ack_timeout){
                response = listen_for_ack();
                if(response){
                    break;
                }
                delay(25);
            }
            display.display.clearDisplay();
            if(response == 0){
                update_display_status("NO RESPONSE.");
                status = RETRANSMITTING;
            }else{
                if(ack_packet.id == packet.id){
                    if(ack_packet.status == SOLID){
                        update_display_link_info();
                        update_display_action("TRANSMITTING. ("+ String(packet.retries)+ ")");
                        update_display_status("COMMS OK.");
                        if(ack_packet.prop_config != curr_config){
                            requested_config = ack_packet.prop_config;
                            status = RECONFIGURING;
                        }else{
                            status = HYBERNATING;      
                            delay(500);
                        }
                    }else{
                        Serial.println(ack_packet.to_string());
                        update_display_link_info();
                        update_display_action("TRANSMITTING. ("+ String(packet.retries)+ ")");
                        update_display_status("CORRUPT.");
                        status = RETRANSMITTING;
                        delay(1000);
                    }
                }else{
                    update_display_link_info();
                    update_display_action("TRANSMITTING. ("+ String(packet.retries)+ ")");
                    update_display_status("PACKET LOSS!");
                    status = HYBERNATING;
                    delay(1000);
                }
            }
            break;
        }

        case RETRANSMITTING:
        {
            if(retransmission_tries>0){
                display.display.clearDisplay();
                update_display_pkt_info();
                update_display_action("RETRANSMITTING. (" + String(packet.retries) + ")");
                delay(1000);

                transmit_packet();
                status = AWAITING_ACK;
                retransmission_tries -=1;
                packet.retries += 1;
            }else{
                status = HYBERNATING;
            }
            break;
        }
        case RECONFIGURING:
        {
            String conf_type ="";
            switch(ack_packet.prop_config)
            {
            case HI:
                conf_type += "HI";
                break;
            case MID:
                conf_type += "MID";
                break;
            case LO:
                conf_type += "LO";
                break;
            }
            update_display_action("RECONFIGURING. ("+ conf_type+ ")");


            //sending reconfigure request packet
            update_display_status("REQUESTING RECONFIG...");
            LoRa.beginPacket();
            reconf_request_packet.prop_config = requested_config;
            LoRa.write((uint8_t*) &reconf_request_packet, sizeof(reconf_request_packet));
            LoRa.endPacket();
            LoRa.receive();
            // awaiting reconfigure commit packet from base
            reconf_commit_time_start = millis();
            int response = 0;
            int success = 0;
            while(millis() - reconf_commit_time_start <= reconf_commit_timeout){
                response = listen_for_reconf_commit();
                if(response){
                    if(reconf_commit_packet.prop_config == requested_config){
                        curr_config = requested_config;
                        success = 1;
                    }
                    break;
                }
                delay(25);
            }
            if(success == 0){
                update_display_status("RECONFIG FAILED");
            }else{
                update_display_status("RECONFIG COMPLETE");
            }
            delay(500);
            status = HYBERNATING;
            break;
        }
    }
}