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

#ifndef __COOLlOS__HARDWARECOMMUNICATION__PCI_H
#define __coolOS__HARDWARECOMMUNICATION__PCI_H

#include <hardwarecommunication/port.h>
#include <drivers/driver.h>
#include <common/types.h>
#include <hardwarecommunication/interrupts.h>
namespace coolOS{
    namespace hardwarecommunication{
    
        
        class PeripheralComponentInterconnectDeviceDescriptor {
        public:
            coolOS::common::uint32_t portBase;
            coolOS::common::uint32_t interrupt;
            
            
            coolOS::common::uint16_t bus;
            coolOS::common::uint16_t device;
            coolOS::common::uint16_t function;
            coolOS::common::uint16_t vendor_id;
            coolOS::common::uint16_t device_id;
            
            
            coolOS::common::uint8_t class_id;
            coolOS::common::uint8_t subclass_id;
            coolOS::common::uint8_t interface_id;
            
            
            coolOS::common::uint8_t revision ;
            
            PeripheralComponentInterconnectDeviceDescriptor();
            ~PeripheralComponentInterconnectDeviceDescriptor();
            
        };
        
        class PeripheralComponentInterconnectController{
            //commandPort - which command the pci needs to perform 
            //dataPort - the arguments for the command in commandPort
            Port<coolOS::common::uint32_t> dataPort;
            Port<coolOS::common::uint32_t> commandPort;
            
        public:
            PeripheralComponentInterconnectController();
            ~PeripheralComponentInterconnectController();
        
            common::uint32_t  Read(common::uint16_t bus, common::uint16_t device, common::uint16_t function, common::uint32_t registeroffset);
            void Write (common::uint16_t bus, common::uint16_t device, common::uint16_t function, common::uint32_t registeroffset, common::uint32_t value);
            bool DeviceHasFunctions (common::uint16_t bus, common::uint16_t device);
            
            void SelectDrivers(coolOS::drivers::DriverManager* driveManager);
            
            
            PeripheralComponentInterconnectDeviceDescriptor GetDeviceDescriptor(common::uint16_t bus, common::uint16_t device, common::uint16_t function);
            
        };

    }
}

#endif /* PCI_H */

