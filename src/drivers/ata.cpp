/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include<drivers/ata.h>

using namespace coolOS;
using namespace coolOS::common;
using namespace coolOS::drivers;


void printf(char *);
void printfHex(uint8_t);


AdvancedTechnologyAttachment::AdvancedTechnologyAttachment(uint16_t portBase, bool master)
: dataPort(portBase),
        errorPort(portBase +1 ),
        sectorCountPort(portBase + 2),
        lbalLowPort(portBase + 3),
        lbalMidPort(portBase + 4),
        lbalHighPort(portBase + 5),
        devicePort(portBase + 6),
        commandPort(portBase + 7),
        controlPort(portBase + 0x206)
{
    bytesPerSector = 512;
    this->master = master;
}
AdvancedTechnologyAttachment::~AdvancedTechnologyAttachment(){
    
}

void AdvancedTechnologyAttachment::Identify(){
    printf("in identify:\n");
    devicePort.Write(master ? 0xA0 : 0xB0);
    controlPort.Write(0); //clears the HOB bit -check
    
    devicePort.Write(0xA0);
    uint8_t status = commandPort.Read();
    if(status == 0xFF){ //there is no device on that bus
        printf("No device on bus");
        return;
    }
   
    devicePort.Write(master ? 0xA0 : 0xB0);
    sectorCountPort.Write(0);
    lbalLowPort.Write(0);
    lbalMidPort.Write(0);
    lbalHighPort.Write(0);
    commandPort.Write(0xEC); //command for identify
    
    status = commandPort.Read();
    if(status == 0x00){ //no device on the bus
        printf("No device on buss");
        return;
    }
    
    while(((status & 0x80) == 0x80) //device is busy
            && ((status & 0x01) != 0x01)){ //there was an error
        status = commandPort.Read();
    }
    
    if(status & 0x01){
        printf("ATA IDENTIFY ERROR");
        return;
    }
    
    for(uint16_t i = 0; i < 256; i++){
        uint16_t data = dataPort.Read();
        char * msg = "  \0";
        msg[1] = (data >> 8) & 0x00FF;
        msg[0] = data & 0x00FF;
        printf(msg);
    }
    printf("\n");
}


void AdvancedTechnologyAttachment::Read28(uint32_t sector, uint8_t* data, int count){
  if(sector > 0x0FFFFFFF){
        printf("ATA ERROR: read sector does not exist.\n");
        return;
    }
    if(count > bytesPerSector){
       printf("ATA ERROR: too many bytes to write\n");
       return;
    }
    devicePort.Write((master ? 0xE0 : 0xF0) | ((sector & 0x0F000000) >> 24) );
    errorPort.Write(0);
    sectorCountPort.Write(1); //for now we read or right one sector
  
    lbalLowPort.Write((sector & 0x000000FF));
    lbalMidPort.Write((sector & 0x0000FF00) >> 8);
    lbalHighPort.Write((sector & 0x00FF000) >> 16);
    commandPort.Write(0x20); //command for identify

    
    uint8_t status = commandPort.Read();
    while(((status & 0x80) == 0x80) //device is busy
            && ((status & 0x01) != 0x01)){ //there was an error
        status = commandPort.Read();
    }
    
    if(status & 0x01){
        printf("ATA READ ERROR");
        return;
    }
    printf("Reading from ATA: ");
 
     for(int i = 0; i < count; i += 2)
    {
        uint16_t wdata = dataPort.Read();
        
        char *text = "  \0";
        text[0] = wdata & 0xFF;
        
        if(i+1 < count){
            text[1] = (wdata >> 8) & 0xFF;

        }
        else{
            text[1] = '\0';

        }
        
        printf(text);
    }    
    
    for(int i = count + (count%2); i < 512; i += 2){
        dataPort.Read();
    }
   
}
void AdvancedTechnologyAttachment::Write28(uint32_t sector, uint8_t* data, int count){
   if(sector > 0x0FFFFFFF){
        printf("ATA ERROR: write sector does not exist.\n");
        return;
    }
    if(count > bytesPerSector){
       printf("ATA ERROR: too many bytes to write\n");
       return;
    }
   
    devicePort.Write((master ? 0xE0 : 0xF0) | ((sector & 0x0F000000) >> 24));
    errorPort.Write(0);
    sectorCountPort.Write(1); //for now we read or right one sector
  
    lbalLowPort.Write((sector & 0x000000FF));
    lbalMidPort.Write((sector & 0x0000FF00) >> 8);
    lbalHighPort.Write((sector & 0x00FF000) >> 16);
    commandPort.Write(0x30); //command for write
    
    printf("Writing to ATA: ");
 
    for(uint16_t i = 0; i < 256; i++){
        uint16_t wdata = data[i];
        if( i+1 < count){
            wdata |= ((uint16_t)data[i+1]) << 8;
            i++;
        }
        char * msg = "  \0";
        msg[0] = (wdata >> 8) & 0x00FF;
        msg[1] = wdata & 0x00FF;
        printf(msg);
        
        dataPort.Write(wdata);
        
    }
    
    for(uint16_t i = count + (count%2) ; i < bytesPerSector; i+=2){
        dataPort.Write(0x0000);
    }

    
}
void AdvancedTechnologyAttachment::Flush(){ //flush the cash of the hard drive
    devicePort.Write(master ? 0xE0 : 0xF0);
    commandPort.Write(0xE7); //flush
    
    uint8_t status = commandPort.Read();

    while(((status & 0x80) == 0x80) //device is busy
            && ((status & 0x01) != 0x01)){ //there was an error
        status = commandPort.Read();
    }
    
    if(status & 0x01){
        printf("ATA FLUSH ERROR");
        return;
    }
 
    
}