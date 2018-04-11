/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include<net/icmp.h>
#include <common/coolio.h>
using namespace coolOS;
using namespace coolOS::common;
using namespace coolOS::net;

InternetControlMessageProtocol::InternetControlMessageProtocol(InternetProtocolProvider* backend)
: InternetProtocolHandler(backend, 0x01)
{
    
}

InternetControlMessageProtocol::~InternetControlMessageProtocol(){
    
}

bool InternetControlMessageProtocol::OnInternetProtocolReceived(common::uint32_t srcIP_BE, common::uint32_t dstIP_BE,
                                            common::uint8_t * internetprotocolPayload, common::uint32_t size){
    
    if(size < sizeof(InternetControlMessageProtocolMessage)){
        return false;
    }
    InternetControlMessageProtocolMessage* msg = (InternetControlMessageProtocolMessage*)internetprotocolPayload;
    
    switch(msg->type){
        case 0:
            printf("ping response from "); printfHex(srcIP_BE & 0xFF);
            printf("."); printfHex((srcIP_BE >>8) & 0xFF);
            printf("."); printfHex((srcIP_BE >>16) & 0xFF);
            printf("."); printfHex((srcIP_BE >>24 )& 0xFF);
            break;
             
        case 8: //if we get a ping request
            msg->type = 0;
            msg->checksum = 0;
            msg->checksum = InternetProtocolProvider::Checksum((uint16_t*)msg, sizeof(InternetControlMessageProtocolMessage));
            return true;
            
            
    }
}



void InternetControlMessageProtocol::RequestEchoReply(common::uint32_t ip_be){
    //check more icmp usage cases
    InternetControlMessageProtocolMessage icmp;
    icmp.type = 8; //ping
    icmp.code = 0;
    icmp.data = 0x3713; //BE version of this is 1337 :)
    icmp.checksum = 0;
    icmp.checksum = InternetProtocolProvider::Checksum((uint16_t*)&icmp, sizeof(InternetControlMessageProtocolMessage));
    
    InternetProtocolHandler::Send(ip_be, (uint8_t*)&icmp, sizeof(InternetControlMessageProtocolMessage));
}


