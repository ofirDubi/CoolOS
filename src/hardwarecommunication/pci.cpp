/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <hardwarecommunication/pci.h>
#include <drivers/amd_am79c973.h>
#include <common/coolio.h>


using namespace coolOS::common;
using namespace coolOS::drivers;
using namespace coolOS::hardwarecommunication;



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



void PeripheralComponentInterconnectController::SelectDrivers(coolOS::drivers::DriverManager* driverManager, coolOS::hardwarecommunication::InterruptManager * interruptManager){
    //iterate the buses
    for(int bus = 0; bus < 8; bus ++){
        //iterate the devices
        for(int device = 0; device < 8; device ++){
            
            int numFunctions = DeviceHasFunctions(bus, device)? 8 : 1;
            //iterate the functions
            for(int function = 0; function <numFunctions; function ++){
                
                PeripheralComponentInterconnectDeviceDescriptor dev = GetDeviceDescriptor(bus, device, function);
                if(dev.vendor_id == 0x0000 || dev.vendor_id == 0xFFFF){
                    continue;
                }
                
                
                for(int barNum = 0; barNum < 6; barNum++){
                    BaseAddressRegister bar = GetBaseAddressRegister(bus, device, function, barNum, interruptManager);
                    if(bar.address && (bar.type == InputOutput)){
                        dev.portBase = (uint32_t)bar.address;
                    }
                   
                }
                Driver * driver = GetDriver(dev, interruptManager);
                if(driver != 0){
                    driverManager->AddDriver(driver);
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


BaseAddressRegister PeripheralComponentInterconnectController::GetBaseAddressRegister                          //change to int
                                                            (uint16_t bus, uint16_t device, uint16_t function, int16_t barNum, InterruptManager * interrupManager){
    BaseAddressRegister result;
    //header type is - 00(32 bit), 01(20 bit), 10 (64 bit)
    uint32_t headertype = Read(bus, device, function, 0x0E) & 0x7F;
    
    int maxBARs = 6 - (4*headertype);
    
    if(barNum >= maxBARs){
        return result;
    }
    
    uint32_t bar_value = Read(bus, device, function, 0x10 + 4*barNum);
    result.type = (bar_value & 0x1) ? InputOutput : MemoryMapping;
    
    
    uint32_t temp;
    if(result.type == MemoryMapping){
       /* switch((bar_value >>1) & 0x3 ){
            //complete later
            case 0: //32 bit mode
                
            case 1: //20 bit mode 
            case 2: //64 bit mode
                
        }
        result.prefetchable = ((bar_value >>3 )& 0x1) ==0x1; //it is prefetchable
        */ 
    }   
    else{ //InputOoutput
        result.address = (uint8_t*)(bar_value & ~0x3);
        result.prefetchable = false;
    }
    return result;
    
}


Driver* PeripheralComponentInterconnectController::GetDriver(PeripheralComponentInterconnectDeviceDescriptor dev, InterruptManager * interruptManager){
    Driver *driver = 0;
    switch(dev.vendor_id){
        case  0x1022: //AMD
            switch(dev.device_id){
                case 0x2000: //am79c973 
                    printf("AMD am79c973");
                    driver = (Driver*)MemoryManager::activeMemoryManager->malloc(sizeof(amd_am79c973));
                    if(driver != 0){
                        new (driver) amd_am79c973(&dev, interruptManager);
                    }
                    return driver;
                    break;
            }
            break;
            
        case 0x8086: //Intel
            break;
    }
    
    switch(dev.class_id){
        case 0x03: //graphics
            switch(dev.subclass_id){
                case 0x00: //VGA
                    printf("VGA");
                    break;
            }
            break;
    }
    return 0;
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



