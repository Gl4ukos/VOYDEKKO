

#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>
#include <Packet.hpp>
#include <cstdlib>

#define LORA_SS      5
#define LORA_RST     27
#define LORA_DIO0    26

void setup() {

    Serial.begin(115200);
    delay(1000);

    Serial.println("Starting LoRa...");

    LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

    if (!LoRa.begin(868E6)) {
        Serial.println("LoRa init failed!");
        while (true);
    }
    LoRa.setSpreadingFactor(7);
    LoRa.setSignalBandwidth(125E3);
    LoRa.setCodingRate4(5);
    LoRa.enableCrc();

    Serial.println("LoRa OK!");

    LoRa.setTxPower(17); // dBm
}

void loop() {

    int packetSize = LoRa.parsePacket();

    if (packetSize) {

        String msg = "";

        while (LoRa.available())
            msg += (char)LoRa.read();

        Serial.println(msg);

        Serial.print("RSSI: ");
        Serial.println(LoRa.packetRssi());

        Serial.print("SNR: ");
        Serial.println(LoRa.packetSnr());
    }

}