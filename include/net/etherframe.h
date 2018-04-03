/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   etherframe.h
 * Author: ofir123dubi
 *
 * Created on April 3, 2018, 4:12 AM
 */

#ifndef __COOLOS__NET__ETHERFRAME_H
#define __COOLOS__NET__ETHERFRAME_H

#include <common/types.h>
#include <drivers/amd_am79c973.h>
#include <memorymanagment.h>

namespace coolOS{
    namespace net{
        
        struct EtherFrameHeader{
            common::uint64_t dstMAC_BE : 48;
            common::uint64_t srcMAC_BE : 48;
            common::uint16_t etherType_BE;
            
        }__attribute((packed));
        
        //checksum
        typedef common::uint32_t EtherFrameFooter;
        
        class EtherFrameProvider;
        
        class EtherFrameHandler{
        protected:
            EtherFrameProvider* backend;
            common::uint16_t etherType_BE;
            
        public:
            EtherFrameHandler(EtherFrameProvider* backend ,common::uint16_t etherType);
            ~EtherFrameHandler();
            
            virtual bool OnEtherFrameReceived(common::uint8_t * etherframePayload, common::uint32_t size);
            void Send(common::uint64_t dstMAC_BE,  common::uint8_t * etherFramePayload , common::uint32_t size);
        };
        
        
        
        
        class EtherFrameProvider : public drivers::RawDataHandler{
        
            friend class EtherFrameHandler;
        
        protected:
            //number of ports
            EtherFrameHandler* handlers[65535];
            
        public:
            
            EtherFrameProvider(drivers::amd_am79c973 * backend);
            ~EtherFrameProvider();
            
            virtual bool OnRawDataReceived(common::uint8_t* buffer , common::uint32_t size);
            void Send(common::uint64_t dstMAC_BE, common::uint16_t etherType_BE , common::uint8_t * buffer, common::uint32_t size);
        };
        
    }
}


#endif /* ETHERFRAME_H */

