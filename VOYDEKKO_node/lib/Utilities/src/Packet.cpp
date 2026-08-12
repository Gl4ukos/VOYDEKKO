#include <Packet.hpp>

String Packet::to_string() {
    String result = "PKT_ID: ";
    result += String(id);
    result += "  TMP: ";
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



String AckPacket::to_string(){
    String result = "ACKPKT_ID: ";
    result += String(id);
    result += "\nSTATUS: ";
    if(status == SOLID){
        result += "OK";
    }else if(status == CORRUPT){
        result += "CORRUPT";
    }else{
        result += "UNKNOWN";
    }
    return result;
}