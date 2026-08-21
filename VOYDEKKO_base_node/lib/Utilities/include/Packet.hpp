#ifndef PACKET_HPP
#define PACKET_HPP

#include <Arduino.h>
#include <stdlib.h>

class Packet{
    public:
    uint8_t prelude;
    uint32_t id;
    float_t temp;
    uint8_t retries;

    String to_string();
    void update_packet();
};

enum STATUS_T{
    SOLID,
    CORRUPT
};

enum PROPOSED_CONFIG{
    LO,
    MID,
    HI
};

class AckPacket{

    public:
    uint32_t id = 0;
    STATUS_T status = SOLID;
    PROPOSED_CONFIG prop_config = HI;

    String to_string();
};

class Reconf_request{
    public:
    PROPOSED_CONFIG prop_config;
    bool request = true;
};

class Reconf_commit{
    public:
    PROPOSED_CONFIG prop_config;
    bool commit = true;
};

#endif