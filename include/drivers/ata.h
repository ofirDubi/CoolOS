/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ata.h
 * Author: ofir123dubi
 *
 * Created on January 7, 2018, 10:38 AM
 */

#ifndef __COOLOS__ATA_H
#define __COOLOS__ATA_H

#include<hardwarecommunication/port.h>
#include<common/types.h>

namespace coolOS{
    namespace drivers{
        
        class AdvancedTechnologyAttachment{
        protected:
            hardwarecommunication::Port<common::uint16_t> dataPort; //IO port
            hardwarecommunication::Port<common::uint8_t> errorPort;
            hardwarecommunication::Port<common::uint8_t> sectorCountPort; //how many sectors we want to read
            hardwarecommunication::Port<common::uint8_t> lbalLowPort; //logical block address 
            hardwarecommunication::Port<common::uint8_t> lbalMidPort;
            hardwarecommunication::Port<common::uint8_t> lbalHighPort;
            hardwarecommunication::Port<common::uint8_t> devicePort;
            hardwarecommunication::Port<common::uint8_t> commandPort; //what do we want to do - read or write
            hardwarecommunication::Port<common::uint8_t> controlPort; //status messages
            bool master; //is it a handler for master or slave - 2 hard drives on the same bus
            common::uint16_t bytesPerSector;
            
        public:
            AdvancedTechnologyAttachment(common::uint16_t portBase, bool master);
            ~AdvancedTechnologyAttachment();
            
            void Identify(); //maybe you can read how man bytes are in a sector - check
            void Read28(common::uint32_t sector, common::uint8_t* data, int count);
            void Write28(common::uint32_t sector, common::uint8_t* data, int count);
            void Flush(); //flush the cash of the hard drive
        
        };
    }
}

#endif /* ATA_H */

