/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ipv4.h
 * Author: ofir123dubi
 *
 * Created on April 8, 2018, 7:55 AM
 */

#ifndef __COOLOS__NET__IPV4_H
#define __COOLOS__NET__IPV4_H

#include<common/types.h>
#include<net/arp.h>
#include <net/etherframe.h>

namespace coolOS{
    namespace net{
        
        struct InternetProocolV4Message{
            common::uint8_t headerLength : 4;
            common::uint8_t version : 4;
            common::uint8_t tos;  //type of service          
            common::uint16_t totalLength;
            
            common::uint16_t ident; //identification
            common::uint16_t flagsAndOffset;
            
            common::uint8_t timeToLive;
            common::uint8_t protocol;
            common::uint16_t checksum;
            
            
            common::uint32_t srcIP;
            common::uint32_t dstIP;
            
            
        }__attribute__((packed));
        
        
        class InternetProtocolProvider;
        
        //i might want to enharet from EthernetFrameHandler
        
        class InternetProtocolHandler{
        
        protected:
            InternetProtocolProvider* backend;
            common::uint8_t ip_protocol;
            
        public:
            InternetProtocolHandler(InternetProtocolProvider* backend ,common::uint8_t ip_protocol);
            ~InternetProtocolHandler();
            //we need to pass srcIP and dstc as well because TCP will use this information for having 2 connections
            //on the same port
            virtual bool OnInternetProtocolReceived(common::uint32_t srcIP_BE, common::uint32_t dstIP_BE,
                                                        common::uint8_t * internetprotocolPayload, common::uint32_t size);
            void Send(common::uint32_t dstIP_BE,  common::uint8_t * internetprotocolPayload , common::uint32_t size);
        };
        
        
        
        class InternetProtocolProvider :  public EtherFrameHandler{
            friend class InternetProtocolHandler;
        protected:
            InternetProtocolHandler* handlers[255];
            AddressResolutionProtocol* arp;
            common::uint32_t gatewayIP;
            common::uint32_t subnetMask;
        public:
            InternetProtocolProvider(EtherFrameProvider* backend, AddressResolutionProtocol* arp,
                    common::uint32_t gatewayIP, common::uint32_t subnetMask);
            ~InternetProtocolProvider();
            
            bool OnEtherFrameReceived(common::uint8_t* internetprotocolPayload, common::uint32_t size);
            void Send(common::uint32_t dstIP_BE, common::uint8_t protocol ,common::uint8_t* data, common::uint32_t size);
            
            static common::uint16_t Checksum(common::uint16_t* data, common::uint32_t lengthInBytes); //generate a hecksum

        };
    }
}


#endif /* IPV4_H */

