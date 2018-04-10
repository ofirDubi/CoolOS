
/* 
 * File:   ipv4.cpp
 * Author: ofir123dubi
 *
 * Created on April 8, 2018, 7:55 AM
 */
#include<net/ipv4.h>
#include<common/coolio.h>

using namespace coolOS;
using namespace coolOS::common;
using namespace coolOS::net;
using namespace coolOS::drivers;





InternetProtocolHandler::InternetProtocolHandler(InternetProtocolProvider* backend ,uint8_t ip_protocol){
    this->backend = backend;
    this->ip_protocol = ip_protocol;
    backend->handlers[ip_protocol] = this;
}

InternetProtocolHandler::~InternetProtocolHandler(){
    if(backend->handlers[ip_protocol] == this){
        backend->handlers[ip_protocol] = 0;
    }
}

bool InternetProtocolHandler::OnInternetProtocolReceived(uint32_t srcIP_BE, uint32_t dstIP_BE,
                                            uint8_t * internetprotocolPayload, uint32_t size){
    return false;
}
void InternetProtocolHandler::Send(uint32_t dstIP_BE,  uint8_t * internetprotocolPayload , uint32_t size){
    backend->Send(dstIP_BE, ip_protocol, internetprotocolPayload, size);
}






InternetProtocolProvider::InternetProtocolProvider(EtherFrameProvider* backend, AddressResolutionProtocol* arp,
        uint32_t gatewayIP, uint32_t subnetMask)
:EtherFrameHandler(backend, 0x800)
{
    //reset handlers
    for(uint8_t i=0; i< 255; i++){
        handlers[i] = 0;
    }
    this->arp = arp;
    this->gatewayIP = gatewayIP;
    this->subnetMask = subnetMask;
}

InternetProtocolProvider::~InternetProtocolProvider(){
    
}

bool InternetProtocolProvider::OnEtherFrameReceived(uint8_t* etherframePayload, uint32_t size){
        
    if(size < sizeof(InternetProocolV4Message)){
        return false;
    }
    
    InternetProocolV4Message* ipmessage = (InternetProocolV4Message*)etherframePayload;
    bool send_back = false;
    
    //check if this ethernet frame is really for us
    if(ipmessage->dstIP == backend->GetIPAddress()){
        
        uint32_t length = ipmessage->totalLength;
        //prevent heartbleed attack
        if(length > size){
            length = size;
        }
        
        if(handlers[ipmessage->protocol] != 0){
            send_back = handlers[ipmessage->protocol]->OnInternetProtocolReceived(ipmessage->srcIP, ipmessage->dstIP,
                    etherframePayload+ 4*ipmessage->headerLength, 
                    length - sizeof(4*ipmessage->headerLength));
        }
    
    }
    if(send_back){
        //switch source and destination ip
        uint32_t temp = ipmessage->dstIP;
        ipmessage->dstIP = ipmessage->srcIP;
        ipmessage->srcIP = temp;
        
        //reset TTL
        ipmessage->timeToLive = 0x40;
        ipmessage->checksum = 0;
        
        //we need to update the checksum
        ipmessage->checksum = Checksum((uint16_t*)ipmessage, 4*ipmessage->headerLength);
    }
    return send_back;
    
}

void InternetProtocolProvider::Send(uint32_t dstIP_BE, uint8_t protocol ,uint8_t* data, uint32_t size){
    
    uint8_t * buffer = MemoryManager::activeMemoryManager->malloc(sizeof(InternetProocolV4Message) + size);
    InternetProocolV4Message *message = (InternetProocolV4Message*) buffer;
    
    message->version = 4; //ipv4
    message->headerLength = sizeof(InternetProocolV4Message)/4;
    message->tos = 0;
    message->totalLength = size + sizeof(InternetProocolV4Message);
    //we need to switch the total length to BE
    message->totalLength = ((message->totalLength & 0xFF00) >>8)
                        |  ((message->totalLength & 0xFF00) >>8);
    
    message->ident = 0x0100;
    //TODO: split to flags and offset
    message->flagsAndOffset = 0x0040;
    message->timeToLive = 0x40;
    message->protocol = protocol;
    
    
    message->dstIP = dstIP_BE;
    message->srcIP = backend->GetIPAddress();
    
    message->checksum = 0;
    message->checksum = Checksum((uint16_t*)message, sizeof(InternetProocolV4Message));
    
    uint8_t * databuffer = buffer + sizeof(InternetProocolV4Message);
    for(uint32_t i = 0; i<size; i++){
        databuffer[i] = data[i];
    }
    
    uint32_t route = dstIP_BE;
    //if the target is not in the local subnet, send the message to the gateway
    if((dstIP_BE & subnetMask) != (message->srcIP & subnetMask)){
        route = gatewayIP;
    }
    backend->Send(arp->Resolve(route), this->etherType_BE, buffer, sizeof(InternetProocolV4Message) + size);
    
    MemoryManager::activeMemoryManager->free(buffer);
}


static uint16_t InternetProtocolProvider::Checksum(uint16_t* data, uint32_t lengthInBytes){
    
    uint32_t temp = 0;
    
    for(uint32_t i=0; i<lengthInBytes/2; i++){
        temp += ((data[i] & 0xFF00) >>8) | ((data[i] & 0x00FF) << 8);
    }
    if(lengthInBytes % 2){
        temp += ((uint16_t)((char*)data)[lengthInBytes-1]) << 8;
    }
    
    //take care of overflow
    while(temp & 0xFFFF0000){
        temp = (temp & 0xFFFF) + (temp >>16);
    }
    
    return ((~temp & 0xFF00) >> 8) | ((~temp & 0x00FF) <<8);
    
}





