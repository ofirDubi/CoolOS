/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   amd_am79c973.h
 * Author: ofir123dubi
 *
 * Created on January 6, 2018, 6:37 AM
 */

#ifndef __COOLOS__AMD_AM79C973_H
#define __COOLOS__AMD_AM79C973_H

#include<common/types.h>
#include <drivers/driver.h>
#include <hardwarecommunication/pci.h>
#include <hardwarecommunication/interrupts.h>
#include <hardwarecommunication/port.h>

namespace coolOS{
    
    namespace drivers{
        
        class amd_am79c973;
        
        class RawDataHandler{
        protected:
            //should point to ethernet driver
            amd_am79c973 *  backend;  
        public:
            RawDataHandler(amd_am79c973* backend);
            ~RawDataHandler();
            virtual bool OnRawDataReceived(common::uint8_t* buffer , common::uint32_t size);
            void Send(common::uint8_t * buffer, common::uint32_t size);
            
        };

        //can do: derive from ethernet driver 
        class amd_am79c973 : public Driver, public hardwarecommunication::InterruptHandler{
            
            struct InitializationBlock{
                common::uint16_t mode;
                unsigned reserved1 : 4;
                unsigned numSendBuffers : 4;
                unsigned reserved2 : 4;
                unsigned numRecvBuffers : 4;
                common::uint64_t physicalAddress : 48; //MAC address - check 
                common::uint16_t reserved3;
                common::uint64_t logicalAddress; //might be IP - check
                common::uint32_t recvBufferDescrAddress;
                common::uint32_t sendBufferDescrAddress;
            }__attribute__((packed));
            
            struct BufferDescriptor{ //check
                
                common::uint32_t address;
                common::uint32_t flags;
                common::uint32_t flags2;
                common::uint32_t avail;
                
            }__attribute__((packed));
            
            hardwarecommunication::Port<common::uint16_t> MACAddress0Port;
            hardwarecommunication::Port<common::uint16_t> MACAddress2Port;
            hardwarecommunication::Port<common::uint16_t> MACAddress4Port;
            hardwarecommunication::Port<common::uint16_t> registerDataPort;
            hardwarecommunication::Port<common::uint16_t> registerAddressPort;
            hardwarecommunication::Port<common::uint16_t> resetPort;
            hardwarecommunication::Port<common::uint16_t> busControlRegisterDataPort;
            
            InitializationBlock initBlock;
                            
            BufferDescriptor* sendBufferDescr;
            common::uint8_t sendBufferDescrMemory[2*1024+15];
            //8 buffers, each of size 2KB plus 15 bytes that are used by the amd_am79c973 chip
            common::uint8_t sendBuffers[2*1024+15][8];
            //which buffers are now activated
            common::uint8_t currentSendBuffer;
            
            BufferDescriptor* recvBufferDescr;
            common::uint8_t recvBufferDescrMemory[2*1024+15];
            common::uint8_t recvBuffers[2*1024+15][8];
            common::uint8_t currentRecvBuffer;
            
            RawDataHandler* data_handler;
        public:
            amd_am79c973(hardwarecommunication::PeripheralComponentInterconnectDeviceDescriptor *dev, 
                    hardwarecommunication::InterruptManager * interruptManager); 
            ~amd_am79c973();
            
            void Activate();
            int Reset();
            common::uint32_t HandleInterrupt(common::uint32_t esp);
            
            void Send(common::uint8_t* bufer, int count);
            void Receive();
            
            void SetHandler(RawDataHandler*  data_handler);
            
            common::uint64_t GetMACAddress();
        };
    }
}


#endif /* AMD_AM79C973_H */

