/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   amd_am79c970.h
 * Author: ofir123dubi
 *
 * Created on January 6, 2018, 6:37 AM
 */

#ifndef __COOLOS__AMD_AM79C970_H
#define __COOLOS__AMD_AM79C970_H

#include<common/types.h>
#include<common/coolmemory.h>
#include <drivers/driver.h>
#include <hardwarecommunication/pci.h>
#include <hardwarecommunication/interrupts.h>
#include <hardwarecommunication/port.h>

namespace coolOS{
    
    namespace drivers{
        
        class amd_am79c970;
        
        //a base class for network stacks
        class RawDataHandler{
        protected:
            //The driver that will be used to receive and send data
            amd_am79c970 *  backend;  
        public:
            RawDataHandler(amd_am79c970* backend);
            ~RawDataHandler();
            //decide what to do when data is received
            virtual bool OnRawDataReceived(common::uint8_t* buffer , common::uint32_t size);
            //send data through the driver
            void Send(common::uint8_t * buffer, common::uint32_t size);
            
        };

        //a driver for the amd79c970 network controller
        class amd_am79c970 : public Driver, public hardwarecommunication::InterruptHandler{
            
            //this structure is passed to the network controller in initialization phase
            struct InitializationBlock{
                common::uint16_t mode;
                unsigned reserved1 : 4;
                unsigned numSendBuffers : 4;
                unsigned reserved2 : 4;
                unsigned numRecvBuffers : 4;
                common::uint64_t physicalAddress : 48; //MAC address  
                common::uint16_t reserved3;
                common::uint64_t logicalAddress; //logical address filter
                common::uint32_t recvBufferDescrAddress;
                common::uint32_t sendBufferDescrAddress;
            }__attribute__((packed));
            
            //used as receive and transmission buffer descriptor
            struct BufferDescriptor{
                
                common::uint32_t address; //address of the buffer in memory
                common::uint32_t flags; //we will use the ownership and size bits from this flag
                common::uint32_t flags2;
                common::uint32_t reserved;
                
            }__attribute__((packed));
            //these ports are used to read the MAC address
            hardwarecommunication::Port<common::uint16_t> MACAddress0Port;
            hardwarecommunication::Port<common::uint16_t> MACAddress2Port;
            hardwarecommunication::Port<common::uint16_t> MACAddress4Port;
            
            //write or read data from a specific control and status register (CSR)
            hardwarecommunication::Port<common::uint16_t> registerDataPort;
            //write or read data from a specific bus control register (BCR)
            hardwarecommunication::Port<common::uint16_t> busControlRegisterDataPort;
            //specify register number for registerDataPort and for busControlRegisterDataPort 
            hardwarecommunication::Port<common::uint16_t> registerAddressPort;
            //this port is used to invoke a device reset
            hardwarecommunication::Port<common::uint16_t> resetPort;

            //declare an initialization block that will be passed to the controller
            InitializationBlock initBlock;
                            
           
            //8 transmission buffers, each of size 2KB plus 15 bytes (to create 16-bit alignment)
            common::uint8_t recvBuffers[2*1024+15][8];
            
            //8 receive buffers, each of size 2KB plus 15 bytes (to create 16-bit alignment)
            common::uint8_t sendBuffers[2*1024+15][8];
           
            //create an array send and receive descriptors
            BufferDescriptor* sendBufferDescr;
            BufferDescriptor* recvBufferDescr;
            
            //allocate space for said array
            common::uint8_t sendBufferDescrMemory[2*1024+15];
            common::uint8_t recvBufferDescrMemory[2*1024+15];
            
            //holds the number of the current available buffer
            common::uint8_t currentRecvBuffer;
            common::uint8_t currentSendBuffer;
            
            //pointer to the current data handler 
            RawDataHandler* data_handler;
        public:
            //constructor
            amd_am79c970(hardwarecommunication::ConfigurationSpaceLayout *dev, 
                    hardwarecommunication::InterruptManager * interruptManager); 
            ~amd_am79c970();
            
            //start receiving network interrupts
            void Activate();
            //reset the network card
            int Reset();
            //handle network interrupts
            common::uint32_t HandleInterrupt(common::uint32_t esp);
            
            //send a message through the network card
            void Send(common::uint8_t* bufer, int count);
            //receive a message from the network card
            void Receive();
            //set a RawDataHandler
            void SetHandler(RawDataHandler*  data_handler);
            common::uint64_t GetMACAddress();
            //manually set you IP address, we will later change this to DHCP protocol
            void SetIPAddress (common::uint32_t);
            //get the IP address
            common::uint32_t GetIPAddress();

            
        };
    }
}


#endif /* AMD_AM79C970_H */

