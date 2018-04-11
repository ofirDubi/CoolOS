/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   icmp.h
 * Author: ofir123dubi
 *
 * Created on April 11, 2018, 8:07 AM
 */

#ifndef __COOLOS__ICMP_H
#define __COOLOS__ICMP_H

#include <common/types.h>
#include <net/ipv4.h>


namespace coolOS{
    namespace net{
        
        
        struct InternetControlMessageProtocolMessage{
            common::uint8_t type;
            common::uint8_t code;
            
            common::uint16_t checksum;
            common::uint32_t data;
        }__attribute__((packed));
        
        
        class InternetControlMessageProtocol : InternetProtocolHandler{
        public:
            InternetControlMessageProtocol(InternetProtocolProvider* backend);
            ~InternetControlMessageProtocol();
            
            bool OnInternetProtocolReceived(common::uint32_t srcIP_BE, common::uint32_t dstIP_BE,
                                                        common::uint8_t * internetprotocolPayload, common::uint32_t size);



            void RequestEchoReply(common::uint32_t ip_be); //ping
            
        };
    }
}



#endif /* ICMP_H */

