/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   pci.h
 * Author: ofir123dubi
 *
 * Created on December 25, 2017, 10:48 AM
 */

#ifndef __COOLOS__HARDWARECOMMUNICATION__PCI_H
#define __COOLOS__HARDWARECOMMUNICATION__PCI_H

#include <hardwarecommunication/port.h>
#include <drivers/driver.h>
#include <common/types.h>
#include <hardwarecommunication/interrupts.h>
#include <memorymanagment.h>

namespace coolOS{
    namespace hardwarecommunication{
    
        //an enum that represent the type of a BAR (I/O or memory map)
        enum BaseAddressRegisterType{
            MemoryMapping = 0,
            InputOutput = 1
        };
        
        //a structure for Base Address Registers
        typedef struct BaseAddressRegister{
            //is the device prefetchable
            bool prefetchable;
            //the address stored in the BAR
            coolOS::common::uint8_t * address;
            //the size of the memory that is used by the BAR
            coolOS::common::uint32_t size;
            //IO or memory map
            BaseAddressRegisterType type;
            
        };
        

        
        //a structure for storing a device configuration space 
        typedef struct ConfigurationSpaceLayout {
            
            //an address from a base address register
            coolOS::common::uint32_t IOBARAddress;
            
            //interrupt number corresponding to this device
            coolOS::common::uint32_t interrupt;
            
            
            coolOS::common::uint16_t bus;
            coolOS::common::uint16_t device;
            coolOS::common::uint16_t function;
            coolOS::common::uint16_t vendor_id;
            coolOS::common::uint16_t device_id;
            
            
            coolOS::common::uint8_t class_id;
            coolOS::common::uint8_t subclass_id;
            //PROG IF register
            coolOS::common::uint8_t interface_id;
            
            
            coolOS::common::uint8_t revision ;

        };
        
        //a class that handles PCI communication
        class PeripheralComponentInterconnectController{
            
            //commandPort - tell the PCI which configuration space would you like to access
            Port<coolOS::common::uint32_t> commandPort;
            
            //dataPort - provides the data that is inside the configuration access specified in the commandPort
            Port<coolOS::common::uint32_t> dataPort;
            
        
                   
            //read a register from the specified device's function configuration space
            common::uint32_t  Read(common::uint16_t bus, common::uint16_t device, common::uint16_t function, common::uint32_t registeroffset);
            
            //write something to a register
            void Write (common::uint16_t bus, common::uint16_t device, common::uint16_t function, common::uint32_t registeroffset, common::uint32_t value);
             
            //try to get a driver for a specific device, called by SelevtDrivers 
            coolOS::drivers::Driver* GetDriver(ConfigurationSpaceLayout dev, coolOS::hardwarecommunication::InterruptManager * interrupManager); 
             
            //get the configuration space layout for a device
            ConfigurationSpaceLayout GetDeviceDescriptor(common::uint16_t bus, common::uint16_t device, common::uint16_t function);
            
            //check if a certain device has any functions
            bool DeviceHasFunctions (common::uint16_t bus, common::uint16_t device);
        
                //get a devices base address register
            BaseAddressRegister GetBaseAddressRegister(common::uint16_t bus, common::uint16_t device, common::uint16_t function, coolOS::common::int16_t barNum); 
            
        public:
            PeripheralComponentInterconnectController();
            ~PeripheralComponentInterconnectController();

            //probe the PCI to discover devices and add them to the driverManager if we have a compatible driver for them
            void SelectDrivers(coolOS::drivers::DriverManager* driverManager, coolOS::hardwarecommunication::InterruptManager* interrupts);
           
            
        
           
        };

    }
}

#endif /* PCI_H */

