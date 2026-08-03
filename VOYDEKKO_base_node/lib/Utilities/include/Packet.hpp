#ifndef PACKET_HPP
#define PACKET_HPP

#include <Arduino.h>


class Packet{
    uint8_t prelude;
    uint8_t id;
    uint8_t temp;
};

#endif