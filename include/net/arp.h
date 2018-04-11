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

#ifndef __COOLOS__NET__ARP_H
#define __COOLOS__NET__ARP_H


#include <common/types.h>
#include <net/etherframe.h>


namespace coolOS{
    namespace net{
        
        struct AddressResolutionProtocolMessage{
            common::uint16_t hardwareType;
            common::uint16_t protocol;
            common::uint8_t hardwareAddressSize; //6
            common::uint8_t protocolAddressSize; //4
            common::uint16_t command;
            
            
            common::uint64_t srcMAC : 48;
            common::uint32_t srcIP;
            common::uint64_t dstMAC :48 ;
            common::uint32_t dstIP;

            
        }__attribute__((packed));
        
        struct CacheUnit{
            common::uint32_t IP;
            common::uint64_t MAC :48;
        };
        
        class AddressResolutionProtocol : public EtherFrameHandler{
        
            CacheUnit AddressCache[128];
            common::uint8_t numCacheEntries;
            
        public:
            AddressResolutionProtocol(EtherFrameProvider* backend);
            ~AddressResolutionProtocol();
            
            bool OnEtherFrameReceived(common::uint8_t* etherframePayload, common::uint32_t size);
            
            //request another computer's MAC address
            void RequestMACAddress(common::uint32_t IP_BE);
            common::uint64_t GetMACFromCache(common::uint32_t IP_BE);
            
            //request address and wait for result
            common::uint64_t Resolve(common::uint32_t IP_BE);
            void BroadcastMACAddress(common::uint32_t IP_BE);
        };
        
    }
}

#endif /* ARP_H */

