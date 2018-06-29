/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <drivers/amd_am79c970.h>
#include <common/coolio.h>

//check am79c970 message/error codes
#define CHECK_DEVICE_CODE(code ,targetCode, targetMessage) if((code & targetCode) == targetCode) printf(targetMessage);

using namespace coolOS;
using namespace coolOS::hardwarecommunication;
using namespace coolOS::drivers;
using namespace coolOS::common;




           

//RawDataHandler constructor - set this as the active data_handler
RawDataHandler::RawDataHandler(amd_am79c970* backend){
    this->backend = backend;
    backend->SetHandler(this);
}

//stop using this data handler
RawDataHandler::~RawDataHandler(){
    backend->SetHandler(0);
}

//do nothing as default
bool RawDataHandler::OnRawDataReceived(uint8_t* buffer , uint32_t size){
    
}

//send data through the backend driver
void RawDataHandler::Send(uint8_t * buffer, uint32_t size){
    backend->Send(buffer, size);
}







//amd_am79c970 constructor
amd_am79c970::amd_am79c970(ConfigurationSpaceLayout *dev, InterruptManager * interruptManager)
: Driver(),                              
    InterruptHandler(interruptManager, dev->interrupt +interruptManager->getHardwareInterruptOffset()),
    MACAddress0Port(dev->IOBARAddress),
    MACAddress2Port(dev->IOBARAddress + 0x02),
    MACAddress4Port(dev->IOBARAddress + 0x04),
    registerDataPort(dev->IOBARAddress + 0x10), //RDP
    registerAddressPort(dev->IOBARAddress + 0x12),// RAP - used for RDP and BCP as register index
    resetPort(dev->IOBARAddress + 0x14),
    busControlRegisterDataPort(dev->IOBARAddress + 0x16) //BCP
    
{
    this->data_handler = 0;
    currentSendBuffer = 0;
    currentRecvBuffer = 0;
    
    //read the device's MAC address
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
    //write this value to BCR20 - sets the device's software style to 2 - 32-bit mode.
    busControlRegisterDataPort.Write(0x102); 
    
    //write to the status register
    registerAddressPort.Write(0);
    registerDataPort.Write(0x04); //activate the STOP bit - tells the device to not send or receive data.
                                  //we will disable this bit in the Activate method
    
    //initBloc
    initBlock.mode = 0x0000; //enable receive and send, disable promiscuous mode (disable logical addresses) 
    initBlock.reserved1 = 0;
    initBlock.numSendBuffers = 3; //log2(3) is  8 
    initBlock.reserved2 = 0;
    initBlock.numRecvBuffers = 3; //log2(3) is  8 
    initBlock.physicalAddress = MAC;
    initBlock.reserved3 = 0;
    initBlock.logicalAddress = 0;//no logical address filter
    
    //point the descriptors to their memory location                                                                           
    sendBufferDescr = (BufferDescriptor*)((((uint32_t)&sendBufferDescrMemory[0])+15) & ~((uint32_t)0xf)); //make 16 bit aligned
    //insert buffer descriptor array to the init block
    initBlock.sendBufferDescrAddress = (uint32_t)sendBufferDescr;
    
    //point the descriptors to their memory location   
    recvBufferDescr = (BufferDescriptor*)((((uint32_t)&recvBufferDescrMemory[0])+15) & ~((uint32_t)0xf));//make 16 bit aligned
    //insert buffer descriptor array to the init block
    initBlock.recvBufferDescrAddress = (uint32_t)recvBufferDescr;
    
    //initiate buffer descriptors
    for(uint8_t i=0; i< 8; i++){
        
        
        sendBufferDescr[i].address = (((uint32_t)&sendBuffers[i]) + 15) & ~(uint32_t)0xf;//16-bit aligned
        sendBufferDescr[i].flags = 0x7FF //set length of the descriptor to 2048
                                    | 0xF000;
        sendBufferDescr[i].flags2 = 0; 
        sendBufferDescr[i].reserved = 0;
        
        
        recvBufferDescr[i].address = (((uint32_t)&recvBuffers[i]) + 15) & ~(uint32_t)0xf;//16-bit aligned
        recvBufferDescr[i].flags = 0xF7FF //set length of the descriptor to 2048
                                 | 0x80000000; //sets the ownership flag - the network controller owns these buffers
        recvBufferDescr[i].flags2 = 0; 
        recvBufferDescr[i].reserved = 0; 
   
    }
    //write initialization block to device
    registerAddressPort.Write(1);
    registerDataPort.Write( (uint32_t)(&initBlock) & 0xFFFF );
   
    registerAddressPort.Write(2);
    registerDataPort.Write( ((uint32_t)(&initBlock) >> 16) & 0xFFFF );
    
}

amd_am79c970::~amd_am79c970(){
    
}

void amd_am79c970::Activate(){
    
    registerAddressPort.Write(0);
    registerDataPort.Write(0x41); //set the init and interrupt flags
    
    //read 4th register and write it back, but change 1 bit to 1 -check 
    registerAddressPort.Write(4);
    uint32_t temp = registerDataPort.Read();
    registerAddressPort.Write(4);
    registerDataPort.Write(temp | 0xc00); //enable automatic padding and disable transmission polling

    registerAddressPort.Write(0);
    registerDataPort.Write(0x42);//set the start and interrupts flags
}

//reset the network card
int amd_am79c970::Reset(){
   resetPort.Read();
   resetPort.Write(0);
   return 10; //should w8 for 10 miliseconds
}


//handle interrupts raised by the amd_am79c970 card
uint32_t amd_am79c970::HandleInterrupt(uint32_t esp){
    

    //read the value of the status register 
    registerAddressPort.Write(0);
    uint32_t message_code  = registerDataPort.Read();
    
    //check for errors
                    //code,  targetCode, targetMessage
    CHECK_DEVICE_CODE(message_code, 0x8000,"AMD am79c970 ERROR\n" );
    CHECK_DEVICE_CODE(message_code, 0x2000, "AMD am79c970 COLLISION ERROR\n");
    CHECK_DEVICE_CODE(message_code, 0x1000,"AMD am79c970 MISSED FRAME ERROR\n" );
    CHECK_DEVICE_CODE(message_code, 0x0800,"AMD am79c970 MISSED MEMORY ERROR\n");
    if((message_code & 0x0400) == 0x0400){ //DATA RECEIVED
        Receive();
    } 
    CHECK_DEVICE_CODE(message_code, 0x0200,"AMD am79c970 DATA SENT\n" );
    
 
    //acknowledge the received data
    registerAddressPort.Write(0);
    registerDataPort.Write(message_code);
    
    CHECK_DEVICE_CODE(message_code, 0x0100, "AMD am79c970 INIT DONE\n");
    
    return esp;
}

//send data through the amd am79c970 network card
void amd_am79c970::Send(uint8_t* buffer, int size){
    
    //were the data was sent from
    uint32_t sendDescriptor = currentSendBuffer;
    
    //move to the next available buffer - so we can send data from multiple tasks at the same time
    currentSendBuffer = (currentSendBuffer +1) % 8;
    
    if(size > 1518){
        /*we don't support splitting message into several packets in our current driver,
        so we will discard every thing after the first 1518 bytes.*/
        printf("AMD am79c970 WARNING: data is too big, sending the first 1518 bytes.\n");
        size = 1518;
    }
    
    //copy the data to the actual ring buffer
    for(uint8_t *src = buffer + size -1,
                *dst = (uint8_t*)(sendBufferDescr[sendDescriptor].address + size -1);
                 src >= buffer; src--, dst--){
                    
           *dst = *src;
       }
   
    

    printf("\n");
    printf("Sending: ");
    for(int i = 0; i < size; i++)
    {
        printfHex(buffer[i]);
        printf(" ");
    }
    printf("\n");
    sendBufferDescr[sendDescriptor].reserved = 0; //not available
    //maybey reserved should be changed to 1
    sendBufferDescr[sendDescriptor].flags2 = 0; //clear error messages
    sendBufferDescr[sendDescriptor].flags = 0x8300F000  //set the ownerhsip, STP and ENP bits
                                          | ((uint16_t)((-size) & 0xFFF));//encode the size of the message
    
   
    registerAddressPort.Write(0);
    registerDataPort.Write(0x48);/*set the transmision bit and the interrupt enable bit
    an interrupt will be raised when the transmission request has been processed*/
}

//receive data from the network card
void amd_am79c970::Receive(){
    
    printf("\nAMD am79c970 DATA RECEIVED\n");
    
    //loop the receive buffers until we find a buffer that we own - that means that the network card wrote to it and passed it's ownership to us
    for(; (recvBufferDescr[currentRecvBuffer].flags & 0x80000000) == 0; currentRecvBuffer = (currentRecvBuffer +1) %8){
        
        if(!(recvBufferDescr[currentRecvBuffer].flags & 0x40000000) //check the error bit
           && (recvBufferDescr[currentRecvBuffer].flags & 0x03000000) ==0x03000000 ){ //check start of packet and end of packet bits
                                                                                        
            uint32_t size = recvBufferDescr[currentRecvBuffer].flags & 0xFFF;
            
            if(size > 64){ //size of ethernet 2 frame
                size -= 4; //last 4 bytes are checksum, we dont need them.
            }
            
            uint8_t* buffer = (uint8_t*)(recvBufferDescr[currentRecvBuffer].address);
            
         
            
            if(data_handler != 0){
                if(data_handler->OnRawDataReceived(buffer, size)){ //returns false by default
                    //echo
                    Send(buffer, size);
                }
                
            }
            size = 64;
            //print the header of the message that we received
            for(int i=0; i < size; i++){
                printfHex(buffer[i]);
                printf(" ");
            }
            
            
            
        }
        //free the receive buffer
        recvBufferDescr[currentRecvBuffer].flags2 =0;
        recvBufferDescr[currentRecvBuffer].flags =0x8000F7FF; //resets the buffer's length and flips the ownership bit.
    }
    
}


void amd_am79c970::SetHandler(RawDataHandler* data_handler){
    this->data_handler = data_handler;
    
}

uint64_t amd_am79c970::GetMACAddress(){
    return initBlock.physicalAddress;
}

void amd_am79c970::SetIPAddress(uint32_t ip){
    initBlock.logicalAddress = ip;
}

uint32_t amd_am79c970::GetIPAddress(){
    return initBlock.logicalAddress;
}