/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <hardwarecommunication/pci.h>
using namespace coolOS::common;
using namespace coolOS::drivers;
using namespace coolOS::hardwarecommunication;


PeripheralComponentInterconnectDeviceDescriptor::PeripheralComponentInterconnectDeviceDescriptor(){
    
}
PeripheralComponentInterconnectDeviceDescriptor::~PeripheralComponentInterconnectDeviceDescriptor(){
    
}




PeripheralComponentInterconnectController::PeripheralComponentInterconnectController() 
: dataPort(0xCFC), 
 commandPort(0xCF8)
{
    
}

PeripheralComponentInterconnectController::~PeripheralComponentInterconnectController() {

}

uint32_t PeripheralComponentInterconnectController::Read(uint16_t bus, uint16_t device, uint16_t function, uint32_t registeroffset) {
    //construct identifier to send to PCI controller
    
    //check why id is like that
    uint32_t id = 
            0x1 << 31 
            | ((bus & 0xff) << 16)
            | ((device & 0x1F) << 11)
            | ((function & 0x07) << 8)
            | (registeroffset & 0xFC);
    
     //will connect to the wanted bus->device->function
    commandPort.Write(id);
    
    //read the results from the data port
    uint32_t result = dataPort.Read();
    //why
    return result >> (8 * (registeroffset % 4));
    
}

void PeripheralComponentInterconnectController::Write(uint16_t bus, uint16_t device, uint16_t function, uint32_t registeroffset, uint32_t value) {
    //construct identifier to send to PCI controller
    
    //check why id is like that
    uint32_t id = 
            0x1 << 31 
            | ((bus & 0xff) << 16)
            | ((device & 0x1F) << 11)
            | ((function & 0x07) << 8)
            | (registeroffset & 0xFC);
    
    //will connect to the wanted bus->device->function
    commandPort.Write(id);
    
    //write to the device
    dataPort.Write(value);
    
    
}
//checks if a device has multiple functions or not
bool PeripheralComponentInterconnectController::DeviceHasFunctions (uint16_t bus, uint16_t device){
    //the seventh bit of the 0x0E offset tells us if there are multiple functions to the device or not
    return Read(bus, device, 0, 0x0E) & (1<<7);
}

void printfHex(uint8_t);
void printf(char * str);


void PeripheralComponentInterconnectController::SelectDrivers(coolOS::drivers::DriverManager* driveManager){
    //iterate the buses
    for(int bus = 0; bus < 8; bus ++){
        //iterate the devices
        for(int device = 0; device < 8; device ++){
            
            int numFunctions = DeviceHasFunctions(bus, device)? 8 : 1;
            //iterate the functions
            for(int function = 0; function <numFunctions; function ++){
                PeripheralComponentInterconnectDeviceDescriptor dev = GetDeviceDescriptor(bus, device, function);
                if(dev.vendor_id == 0x0000 || dev.vendor_id == 0xFFFF){
                    break;
                }
                printf("PCI BUS ");
                printfHex(bus & 0xFF);
                
                printf(", DEVICE ");
                printfHex(device & 0xFF);
                
                printf(", FUNCTION");
                printfHex(function & 0xFF);
                
                printf(" = VENDOR ");
                printfHex((dev.vendor_id & 0xff00)>>8);
                printfHex(dev.vendor_id & 0xff);
                
                printf(", DEVICE ");
                printfHex((dev.device_id & 0xff00)>>8);
                printfHex(dev.device_id & 0xff);
                
                
                
                
                printf("\n");
                
               
                
                
            }
        }
    }
}


PeripheralComponentInterconnectDeviceDescriptor PeripheralComponentInterconnectController::GetDeviceDescriptor
(uint16_t bus, uint16_t device, uint16_t function){
    PeripheralComponentInterconnectDeviceDescriptor result;
    
    
    result.bus = bus;
    result.device = device;
    result.function = function;
    
    result.vendor_id = Read(bus, device, function, 0x00);
    result.device_id = Read(bus, device, function, 0x02);
    
    result.class_id = Read(bus, device, function, 0x0B);
    result.subclass_id = Read(bus, device, function, 0x0A);
    result.interface_id = Read(bus, device, function, 0x09);
   
    result.revision = Read(bus, device, function, 0x08);
    result.interrupt = Read(bus, device, function, 0x3C);
   
    return result;
}



