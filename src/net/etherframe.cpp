/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include<net/etherframe.h>
#include<common/coolio.h>
using namespace coolOS;
using namespace coolOS::common;
using namespace coolOS::net;
using namespace coolOS::drivers;

//constructor for etherFrameHandler
EtherFrameHandler::EtherFrameHandler(EtherFrameProvider* backend ,common::uint16_t etherType){
    
    //convert from etherType from LE to BE
    this->etherType_BE = ((etherType & 0x00FF) << 8) | ((etherType & 0xFF00) >> 8) ;
    this->backend = backend;
    backend->handlers[this->etherType_BE] = this;    
}

EtherFrameHandler::~EtherFrameHandler(){
    if(backend->handlers[etherType_BE] == this){
        backend->handlers[etherType_BE] = 0;
    }
}

bool EtherFrameHandler::OnEtherFrameReceived(common::uint8_t * etherframePayload, common::uint32_t size){
    //default is false
    return false;
}

void EtherFrameHandler::Send(common::uint64_t dstMAC_BE,  common::uint8_t * data , common::uint32_t size){
    backend->Send(dstMAC_BE, etherType_BE, data, size);
}






//constructor - set all handlers to 0 
EtherFrameProvider::EtherFrameProvider(amd_am79c970 * backend)
: RawDataHandler(backend)
{
    
    for(uint32_t i=0; i<65535; i++){
        handlers[i] = 0;
    }
    
}

EtherFrameProvider::~EtherFrameProvider(){
    
}

//handle Ethernet frame
bool EtherFrameProvider::OnRawDataReceived(common::uint8_t* buffer , common::uint32_t size){
    
    if(size < sizeof(EtherFrameHeader)){
        return false;
    }
    
    EtherFrameHeader* frame = (EtherFrameHeader*)buffer;
    bool send_back = false;
    
    //check if this Ethernet frame is really for us
    if(frame->dstMAC_BE == 0xFFFFFFFFFFFF || frame->dstMAC_BE == backend->GetMACAddress()){
        //if there is a handler for this protocol
        if(handlers[frame->etherType_BE] != 0){
            //call the OnEtherFrameReceived of the specified protocol
            send_back = handlers[frame->etherType_BE]->OnEtherFrameReceived( buffer+ sizeof(EtherFrameHeader), 
                    size - sizeof(EtherFrameHeader));
        }
    }
    
    if(send_back){
        //switch MAC addresses in the Ethernet header
        frame->dstMAC_BE = frame->srcMAC_BE;
        frame->srcMAC_BE = backend->GetMACAddress();
    }
    return send_back;
    
}

//Send an Ethernet packet
void EtherFrameProvider::Send(common::uint64_t dstMAC_BE , common::uint16_t etherType_BE , 
        common::uint8_t * buffer, common::uint32_t size){
    
    //for debuging
   // printf(" EtherFrameProvider::Send start\n");

    //allocate more size for the Ethernet header
    uint8_t* send_buffer = MemoryManager::activeMemoryManager->malloc(sizeof(EtherFrameHeader) + size);
    
    EtherFrameHeader* frame = (EtherFrameHeader*)send_buffer;
    
    //set up Ethernet header
    frame->dstMAC_BE = dstMAC_BE;
    frame->srcMAC_BE = GetMACAddress();
    frame->etherType_BE = etherType_BE;
    
    
    //copy the packet data to our new buffer
    uint8_t* src = buffer;
    uint8_t * dst = send_buffer + sizeof(EtherFrameHeader);
    for(uint32_t i = 0; i< size; i++){
        dst[i] = src[i];
    }
    
    //send the new buffer
    backend->Send(send_buffer, size + sizeof(EtherFrameHeader));
    
    //free the original buffer
    MemoryManager::activeMemoryManager->free(buffer);
    
    //for debug
   // printf(" EtherFrameProvider::Send dealocated buffer, end\n");

}


//Get IP
uint32_t EtherFrameProvider::GetIPAddress(){
    return backend->GetIPAddress();
}

//Get MAC
uint64_t EtherFrameProvider::GetMACAddress(){
    return backend->GetMACAddress();
}
        
