/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <drivers/mouse.h>


using namespace coolOS::common;
using namespace coolOS::drivers;
using namespace coolOS::hardwarecommunication;


#define INIT_X 40
#define INIT_Y 12

MouseEventHandler::MouseEventHandler() {

}

void MouseEventHandler::OnActivate(){
    
}

void MouseEventHandler::OnMouseDown(uint8_t button){
    
}
void MouseEventHandler::OnMouseUp(uint8_t button){
    
}
void MouseEventHandler::OnMouseMove(int8_t x, int8_t y){
    
}

MouseDriver::MouseDriver(InterruptManager* manager, MouseEventHandler * handler) 
: InterruptHandler(manager, 0x2C), //mouse interrupt number is 0x21
  dataport(0x60), commandport(0x64)
{
    this->handler = handler;
}
MouseDriver::~MouseDriver(){
    
   
}

void MouseDriver::Activate(){
    
    
    offset = 0;
    buttons = 0;
    
    commandport.Write(0xAB); //tells the PiC to start using mouse interrupts - check that in OSdev
    commandport.Write(0x20); // going to set current state
    
    uint8_t status = (dataport.Read() | 2); //
    
    commandport.Write(0x60); //going to set state
    dataport.Write(status); //set the state
    
    commandport.Write(0xD4); //check what this is
    
    dataport.Write(0xF4); //activates the mouse - check
    dataport.Read();   
}

void printf(char *);

uint32_t MouseDriver::HandleInterrupt(uint32_t esp){
    
    uint8_t status = commandport.Read();
    if(!(status & 0x20)){ //only if the sixth bit of status is 1 there is data to read
        return esp;
    }
            
    buffer[offset] = dataport.Read();
    
    if(handler ==0){ //only if the sixth bit of status is 1 there is data to read
        return esp;
    }
    offset = (offset + 1) %3;
    /*
     buffer[1] - movement on x axis
     * buffer[2] - movement on y axis
     * 
     */
    
    static int8_t x =INIT_X,y=INIT_Y;
    static uint16_t * VideoMemory = (uint16_t *)0xb8000; 
    
    if(offset ==0){
        if(buffer[1] !=0 || buffer[2] != 0 ){
            
            handler->OnMouseMove(buffer[1], -buffer[2]);
            
        } 
       
    
    
        for(uint8_t i =0; i<3; i++){
            if((buffer[0] & (0x01 <<i)) != (buttons & (0x01<<i))){ //if a button was pressed
                // ... do what ever you want when buton is pressed
                if(buttons  & (0x1 << i)){
                    handler->OnMouseUp(i+1);
                }else{
                    handler->OnMouseDown(i+1);
                }
                
            }
        }

        buttons = buffer[0];
    }        
    return esp;
}