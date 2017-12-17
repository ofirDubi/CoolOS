/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "keyboard.h"

KeyboardDriver::KeyboardDriver(InterruptManager* manager) 
: InterruptHandler(0x21,manager), //keyboard interrupt number is 0x21
  dataport(0x60), commandport(0x64)
{
    while(commandport.Read() & 0x1){
        dataport.Read();
    }
    commandport.Write(0xAE); //tells the PiC to start using interrupts - check that in OSdev
    commandport.Write(0x20); // get current state
    
    uint8_t status = (dataport.Read() | 1) & ~0x10;
    
    commandport.Write(0x60); //going to set state
    dataport.Write(status); //set the state
    
    
    dataport.Write(0xF4); //activates the keyboard - check
}
KeyboardDriver::~KeyboardDriver(){
    
   
}

void printf(char *);

uint32_t KeyboardDriver::HandleInterrupt(uint32_t esp){
    
    uint8_t key = dataport.Read();
    
    char * msg = "KEYBOARD 0x00";
    char * hex = "0123456789ABCDEF";
    msg[11] = hex[(key >> 4) & 0x0F];
    msg[12] = hex[key & 0x0f];
    printf(msg);
    return esp;
}