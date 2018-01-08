/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <drivers/amd_am79c973.h>
#include <common/coolio.h>


#define CHECK_DEVICE_CODE(code ,targetCode, targetMessage) if((code & targetCode) == targetCode) printf(targetMessage);

using namespace coolOS;
using namespace coolOS::hardwarecommunication;
using namespace coolOS::drivers;
using namespace coolOS::common;


amd_am79c973::amd_am79c973(PeripheralComponentInterconnectDeviceDescriptor *dev, InterruptManager * interruptManager)
: Driver(),                              
    InterruptHandler(interruptManager, dev->interrupt +interruptManager->getHardwareInterruptOffset()),
    MACAddress0Port(dev->portBase),
    MACAddress2Port(dev->portBase + 0x02),
    MACAddress4Port(dev->portBase + 0x04),
    registerDataPort(dev->portBase + 0x10),
    registerAddressPort(dev->portBase + 0x12),
    resetPort(dev->portBase + 0x14),
    busControlRegisterDataPort(dev->portBase + 0x16)
    
{
    currentSendBuffer = 0;
    currentRecvBuffer = 0;
    
    uint64_t MAC0 = MACAddress0Port.Read() % 256;
    uint64_t MAC1 = MACAddress0Port.Read() / 256;
    uint64_t MAC2 = MACAddress2Port.Read() % 256;
    uint64_t MAC3 = MACAddress2Port.Read() / 256;
    uint64_t MAC4 = MACAddress4Port.Read() % 256;
    uint64_t MAC5 = MACAddress4Port.Read() / 256;
    
    uint64_t MAC = MAC5 << 40
                 | MAC4 << 32
                 | MAC3 << 24
                 | MAC2 << 16
                 | MAC1 << 8
                 | MAC0;
    
    //set device to 32 bit mode
    registerAddressPort.Write(20); //set to register number 20
    busControlRegisterDataPort.Write(0x102); //write this value to register number 20
    
    
    //STOP reset - tells the device not to reset now - check
    registerAddressPort.Write(0);
    registerDataPort.Write(0x04);
    
    //initBloc
    initBlock.mode = 0x0000; //promiscuous mode - false -check
    initBlock.reserved1 = 0;
    initBlock.numSendBuffers = 3; //3 is actually 8 
    initBlock.reserved2 = 0;
    initBlock.numRecvBuffers = 3;
    initBlock.physicalAddress = MAC;
    initBlock.reserved3 = 0;
    initBlock.logicalAddress = 0;
    
                                                                                    //make 16 bit aligned
    sendBufferDescr = (BufferDescriptor*)((((uint32_t)&sendBufferDescrMemory[0])+15) & ~((uint32_t)0xf));
    initBlock.sendBufferDescrAddress = (uint32_t)sendBufferDescr;
    
    recvBufferDescr = (BufferDescriptor*)((((uint32_t)&recvBufferDescrMemory[0])+15) & ~((uint32_t)0xf));
    initBlock.recvBufferDescrAddress = (uint32_t)recvBufferDescr;
    
    for(uint8_t i=0; i< 8; i++){
        
        sendBufferDescr[i].address = (((uint32_t)&sendBuffers[i]) + 15) & ~(uint32_t)0xF;
        sendBufferDescr[i].flags = 0x7FF //set length of the descriptor - check
                                    | 0xF000;
        sendBufferDescr[i].flags2 = 0; 
        sendBufferDescr[i].avail = 0;
        
        
        recvBufferDescr[i].address = (((uint32_t)&recvBuffers[i]) + 15) & ~(uint32_t)0xF;
        recvBufferDescr[i].flags = 0xF7FF //check
                                 | 0x800000000; //tells that it is a receive buffer and not a send buffer
        recvBufferDescr[i].flags2 = 0; 
        recvBufferDescr[i].avail = 0;
    }
    //move initialization block to device
    registerAddressPort.Write(1);
    registerDataPort.Write( (uint32_t)(&initBlock) & 0xFFFF );
   
    registerAddressPort.Write(2);
    registerDataPort.Write( ((uint32_t)(&initBlock) >> 16) & 0xFFFF );
    
}

amd_am79c973::~amd_am79c973(){
    
}

void amd_am79c973::Activate(){
    
    registerAddressPort.Write(0);
    registerDataPort.Write(0x41); //enables the interrupts  -check
    
    //read 4th register and write it back, but change 1 bit to 1 -check 
    registerAddressPort.Write(4);
    uint32_t temp = registerDataPort.Read();
    registerAddressPort.Write(4);
    registerDataPort.Write(temp | 0xc00); //enables the interrupts  -check

    registerAddressPort.Write(0);
    registerDataPort.Write(0x42);
}

int amd_am79c973::Reset(){
   resetPort.Read();
   resetPort.Write(0);
   return 10; //should w8 for 10 miliseconds
}



uint32_t amd_am79c973::HandleInterrupt(uint32_t esp){
    
    printf("INTERRUPT FROM AMD am79c973\n");
    //like vanilla interrupt controller: need to read data if it exists
    
    registerAddressPort.Write(0);
    uint32_t temp  = registerDataPort.Read();
    
    //check for errors
                    //code,  targetCode, targetMessage
    CHECK_DEVICE_CODE(temp, 0x8000,"AMD am79c973 ERROR\n" );
    CHECK_DEVICE_CODE(temp, 0x2000, "AMD am79c973 COLLISION ERROR\n");
    CHECK_DEVICE_CODE(temp, 0x1000,"AMD am79c973 MISSED FRAME ERROR\n" );
    CHECK_DEVICE_CODE(temp, 0x0800,"AMD am79c973 MISSED MEMORY ERROR\n");
    if((temp & 0x0400) == 0x0400) Receive(); //if received data, go to Receive method
    CHECK_DEVICE_CODE(temp, 0x0200,"AMD am79c973 DATA SENT\n" );
    
    //acknowledge the received data
    
    registerAddressPort.Write(0);
    registerDataPort.Write(temp);
    
    CHECK_DEVICE_CODE(temp, 0x0100, "AMD am79c973 INIT DONE\n");
    
    return esp;
}


void amd_am79c973::Send(uint8_t* buffer, int size){
    //were the data was sent from
    uint8_t sendDescriptor = currentSendBuffer;
    //move to the next available buffer - so we can send data from multiple tasks at the same time
    
    currentSendBuffer = (currentSendBuffer +1) % 8;
    
    if(size > 1518){
        //not good, discards everything after 1518
        printf("AMD am79c973 WARNING: data is too big, sending the first 1518 bits.\n");
        size = 1518;
    }
    
       for(uint8_t *src = buffer + size -1,
                *dst = (uint8_t*)(sendBufferDescr[sendDescriptor].address + size -1);
                 src >= buffer; src--, dst--){
                    
           *dst = *src;
       }
    sendBufferDescr[sendDescriptor].avail = 0; //not available
    sendBufferDescr[sendDescriptor].flags2 = 0; //clear error messages
    sendBufferDescr[sendDescriptor].flags = 0x8300F000  //encode the size we need to send -check *9:54 video number 18*
                                          | ((uint16_t)((-size) & 0xFFF));
    
    //send the data to the device
    registerAddressPort.Write(0);
    registerDataPort.Write(0x48);
}

void amd_am79c973::Receive(){
    
    printf("AMD am79c973 MISSED MEMORY ERROR\n");
    
    //loop the receive buffers until you fid an empty buffer, a buffer is empty if its first bit is 0
    for(; (recvBufferDescr[currentRecvBuffer].flags & 0x80000000) == 0; currentRecvBuffer = (currentRecvBuffer +1) %8){
        
        if(!(recvBufferDescr[currentRecvBuffer].flags & 0x40000000) //check the error bit
           && (recvBufferDescr[currentRecvBuffer].flags & 0x03000000) ==0x03000000 ){ //check start of packet and en of packet bits
                                                                                        //check page 184 of documentation
            uint32_t size = recvBufferDescr[currentRecvBuffer].flags & 0xFFF;
            
            if(size > 64){ //size of ethernet 2 frame
                size -= 4; //last 4 bytes are checksum, not needed
            }
            
            uint8_t* buffer = (uint8_t*)(recvBufferDescr[currentRecvBuffer].address);
            
            //print what you received
            for(int i=0; i < size; i++){
                printfHex(buffer[i]);
                printf(" ");
            }
            
        }
        //free the receive buffer
        recvBufferDescr[currentRecvBuffer].flags2 =0;
        recvBufferDescr[currentRecvBuffer].flags =0x8000F7FF;
    }
    
}