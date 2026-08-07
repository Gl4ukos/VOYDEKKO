#ifndef PACKET_HPP
#define PACKET_HPP

#include <Arduino.h>
#include <stdlib.h>

class Packet{
    public:
    uint8_t prelude;
    uint8_t id;
    float_t temp;

    String to_string();
    void update_packet();
};

enum STATUS_T{
    SOLID,
    CORRUPT
};

class AckPacket{

    public:
    uint8_t prelude;
    uint8_t id;
    STATUS_T status;

    String to_string();
};

#endif