#include <Packet.hpp>

String Packet::to_string() {
    String result = "PKT ID: ";
    result += String(id);
    result += "\nTMP: ";
    result += String(temp);
    return result;
}

void Packet::update_packet(){
    id += 1;
    if(id == 1000){
        id=0;
    }

    temp = 30 + (random(3) - 1.5);
}