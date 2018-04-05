/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   arp.h
 * Author: ofir123dubi
 *
 * Created on April 5, 2018, 3:29 AM
 */

#include <net/arp.h>
#include <common/coolio.h>
using namespace coolOS;
using namespace coolOS::common;
using namespace coolOS::drivers;
using namespace coolOS::net;

       

AddressResolutionProtocol::AddressResolutionProtocol(EtherFrameProvider* backend)
: EtherFrameHandler(backend, 0x806) //etherType for ARP is 0x806
{
    numCacheEntries = 0;
}
AddressResolutionProtocol::~AddressResolutionProtocol(){
    
}

//when we receive an ARP request
bool AddressResolutionProtocol::OnEtherFrameReceived(common::uint8_t* etherframePayload, common::uint32_t size){
 
    if(size < sizeof(AddressResolutionProtocolMessage)){
        return false;
    }
    
    AddressResolutionProtocolMessage* arp = (AddressResolutionProtocolMessage*)etherframePayload;
    if(arp->hardwareType == 0x0100){ //if it is an ethernet message
        //check if its IPv4
        if(arp->protocol == 0x008
                && arp->hardwareAddressSize ==6
                && arp->protocolAddressSize == 4
                && arp->dstIP == backend->GetIPAddress())
        {
        
            switch(arp->command){
                case 0x0100: //if we ask for MAC address
                    arp->command = 0x200;
                    arp->dstIP = arp->srcIP;
                    arp->dstMAC = arp->srcMAC;
                    arp->srcIP = backend->GetIPAddress();
                    arp->srcMAC = backend->GetMACAddress();
                    return true;
                    break;
                
                case 0x0200: //response to a request that we made
                    if(numCacheEntries < 128){ //TODO:: prevent ARP spoofing , check if the src ip is the ip we requested
                        AddressCache[numCacheEntries].IP = arp->srcIP;
                        AddressCache[numCacheEntries].MAC = arp->srcMAC;
                        numCacheEntries++;
                    }
                    
                    break;
            }
        }
    
    }
    
    
    
    
    return false;
}

//request someone else's MAC Address
void AddressResolutionProtocol::RequestMACAddress(common::uint32_t IP_BE){
    
    AddressResolutionProtocolMessage arp;
    arp.hardwareType = 0x0100; //BE of 1
    arp.protocol = 0x0008; // IPv4
    arp.hardwareAddressSize = 6; //MAC
    arp.protocolAddressSize = 4; //IPv4
    arp.command = 0x0100; //BE of 1, means request
    
    arp.srcMAC = backend->GetMACAddress();
    arp.srcIP = backend->GetIPAddress();
    arp.dstMAC = 0xFFFFFFFFFFFF; //broadcast
    arp.dstIP = IP_BE;
    
    
    this->Send(arp.dstMAC, (uint8_t *)&arp, sizeof(AddressResolutionProtocolMessage));
    
    
}

uint64_t AddressResolutionProtocol::GetMACFromCache(common::uint32_t IP_BE){
    
    for(int i=0; i < numCacheEntries; i++){
        if(AddressCache[i].IP == IP_BE){
            return AddressCache[i].MAC;
        }
    }
    return 0xFFFFFFFFFFFF; //broadcast address
}


uint64_t  AddressResolutionProtocol::Resolve(common::uint32_t IP_BE){
    
    uint64_t result = GetMACFromCache(IP_BE);
    if(result == 0xFFFFFFFFFFFF){
          RequestMACAddress(IP_BE);
    }
    while(result == 0xFFFFFFFFFFFF){ //possible infinite loop, if the machine is not connected
        result = GetMACFromCache(IP_BE);
    }    
    
    
    return result;
}


