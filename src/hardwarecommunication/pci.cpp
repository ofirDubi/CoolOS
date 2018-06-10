/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <hardwarecommunication/pci.h>
#include <drivers/amd_am79c970.h>
#include <common/coolio.h>


using namespace coolOS::common;
using namespace coolOS::drivers;
using namespace coolOS::hardwarecommunication;


//constructor, set the command and data ports.
PeripheralComponentInterconnectController::PeripheralComponentInterconnectController() 
: commandPort(0xCF8),
  dataPort(0xCFC) 
 {
    
}

PeripheralComponentInterconnectController::~PeripheralComponentInterconnectController() {

}

uint32_t PeripheralComponentInterconnectController::Read(uint16_t bus, uint16_t device, uint16_t function, uint32_t registeroffset) {
    //construct identifier to send to PCI controller
    
    /*configuration space address structure:
     * 0-1 bits = always 0 
    *  2-7 bits = register offset
     * 8-10 function number
     * 11-15 device number
     * 16-23 bus number
     * 24 -30 reserved
     * 31 - always one for active registers
    */
    uint32_t CONFIG_ADDRESS = 
            0x1 << 31 
            | ((bus & 0xff) << 16)
            | ((device & 0x1F) << 11)
            | ((function & 0x07) << 8)
            | (registeroffset & 0xFC);
    
     //will connect to the wanted bus->device->function
    commandPort.Write(CONFIG_ADDRESS);
    
    //read the results from the data port
    uint32_t result = dataPort.Read();
    
    /* 
     * the registers are 32 bit long - 4 bytes. if we want to read an entire register,
     * our register offset will be dividable by 4. in order to read a specific part of a register, 
     * we shift the result by a byte for each reminder of the register offset divided by 4
     * if we don't do that, the result will contain a part of the next register. 
     */
    return result >> (8 * (registeroffset % 4));
    
}

//write to a configuration space register 
void PeripheralComponentInterconnectController::Write(uint16_t bus, uint16_t device, uint16_t function, uint32_t registeroffset, uint32_t value) {
    //construct identifier to send to PCI controller
    
    //configuration space address structure
    uint32_t CONFIG_ADDRESS = 
            0x1 << 31 
            | ((bus & 0xff) << 16)
            | ((device & 0x1F) << 11)
            | ((function & 0x07) << 8)
            | (registeroffset & 0xFC);
    
    //will connect to the wanted bus->device->function
    commandPort.Write(CONFIG_ADDRESS);
    
    //write to the device
    dataPort.Write(value);
    
    
}
//checks if a device has multiple functions or not
bool PeripheralComponentInterconnectController::DeviceHasFunctions (uint16_t bus, uint16_t device){
    //the seventh bit of the 0x0E register tells us if there are multiple functions to the device or not
    return Read(bus, device, 0, 0x0E) & (1<<7);
}


//scan the PCI and search for devices
void PeripheralComponentInterconnectController::SelectDrivers(coolOS::drivers::DriverManager* driverManager, coolOS::hardwarecommunication::InterruptManager * interruptManager){
    //iterate the buses
    for(int bus = 0; bus < 8; bus ++){
        //iterate the devices
        for(int device = 0; device < 32; device ++){
            
            int numFunctions = DeviceHasFunctions(bus, device)? 8 : 1;
            //iterate the functions
            for(int function = 0; function <numFunctions; function ++){
                
                //get the device's function configuration space layout
                ConfigurationSpaceLayout dev = GetDeviceDescriptor(bus, device, function);
                
                //check if this function exists
                if(dev.vendor_id == 0x0000 || dev.vendor_id == 0xFFFF){
                    continue;
                }
                
                //get the device's BARs
                for(int barNum = 0; barNum < 6; barNum++){
                    BaseAddressRegister bar = GetBaseAddressRegister(bus, device, function, barNum);
                    //add an IO BAR to the IOBARAddress variable
                    if(bar.address && (bar.type == InputOutput)){
                        dev.IOBARAddress = (uint32_t)bar.address;
                    }
                   
                }
                Driver * driver = GetDriver(dev, interruptManager);
                if(driver != 0){
                    driverManager->AddDriver(driver);
                }
                //print device data
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

//receive a base address register
BaseAddressRegister PeripheralComponentInterconnectController::GetBaseAddressRegister                       
                                                            (uint16_t bus, uint16_t device, uint16_t function, int16_t barNum ){
    BaseAddressRegister result;
    //header type is - 00(standard, 32-bit), 01(PCI-to-PCI, 32 bit), 10 (PCI-to-PC Card, 64 bit)
    uint32_t headertype = Read(bus, device, function, 0x0E) & 0x7F;
    
   /*the standard header has 6 bars, the rest have 2 bars, 
    * but we cant read the PCI-to-PC Card BAR with the same method*/
    int maxBARs = 6-4*headertype;
    
    if(barNum >= maxBARs){
        return result;
    }
    
    //read the BAR from the configuration space
    uint32_t bar_value = Read(bus, device, function, 0x10 + 4*barNum);
    
    //determine the bar's type
    result.type = (bar_value & 0x1) ? InputOutput : MemoryMapping;
    
    //memory mapping - not implemented
    if(result.type == MemoryMapping){
        switch((bar_value >>1) & 0x3 ){
            //complete later
            case 0: //32 bit mode
                result.address = (uint8_t*)(bar_value & ~0xF); //16-bit aligned
                break;
            
            case 1: break;//20 bit mode - deprecated 
            case 2: break;/*64 bit mode - need to read the next bar for the full address
            not implemented in our 32-bit system*/
                
        }
        result.prefetchable = ((bar_value >>3 )& 0x1) ==0x1; //it is prefetchable
        
    }   
    else{ //InputOoutput
        //make a 4-bit alignment
        result.address = (uint8_t*)(bar_value & ~0x3);
        result.prefetchable = false;
    }
    return result;
    
}

//attach driver to compatible devices
Driver* PeripheralComponentInterconnectController::GetDriver(ConfigurationSpaceLayout dev, InterruptManager * interruptManager){
    Driver *driver = 0;
    switch(dev.vendor_id){
        case  0x1022: //AMD
            switch(dev.device_id){
                case 0x2000: //am79c970 
                    printf("AMD am79c970");
                    driver = (Driver*)MemoryManager::activeMemoryManager->malloc(sizeof(amd_am79c970));
                    if(driver != 0){
                        new (driver) amd_am79c970(&dev, interruptManager);
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

//insert device data into ConfigurationSpaceLayout structure
ConfigurationSpaceLayout PeripheralComponentInterconnectController::GetDeviceDescriptor
(uint16_t bus, uint16_t device, uint16_t function){
    ConfigurationSpaceLayout result;
    
    
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



