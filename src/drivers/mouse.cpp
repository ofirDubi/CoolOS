/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <drivers/mouse.h>
#include <common/coolio.h>


using namespace coolOS::common;
using namespace coolOS::drivers;
using namespace coolOS::hardwarecommunication;


#define INIT_X 40
#define INIT_Y 12

MouseEventHandler::MouseEventHandler() {

}

void MouseEventHandler::OnMouseDown(uint8_t button){
    
}
void MouseEventHandler::OnMouseUp(uint8_t button){
    
}
void MouseEventHandler::OnMouseMove(int x, int y){
    printf("mouse was moved");
    
}

MouseDriver::MouseDriver(InterruptManager* manager, MouseEventHandler * handler) 
: InterruptHandler(manager, 0x2C), //mouse interrupt number is 0x21
  dataport(0x60), commandport(0x64)
{
    this->handler = handler;
}
MouseDriver::~MouseDriver(){
    
   
}

//Activates the mouse
void MouseDriver::Activate(){
    
    //initialize offset and buttons to 0
    offset = 0;
    buttons = 0;

    
    commandport.Write(0xAB); //Enables second PS/2 port 
    commandport.Write(0x20); //get the PS/2 control configuration byte 
    
    /*the dataport.Read() will return the control configuration mask. 
	it determents which PS/2 port is active and some other PS/2 controller settings.
	we enable the mouse PS/2 port by activating the second bit of the register*/
    uint8_t status = (dataport.Read() | 2); 
    
    commandport.Write(0x60); //command to set the new configuration byte
    dataport.Write(status); //pass the status as the new configuration byte
    
    commandport.Write(0xD4); //write the next incoming byte to the second PS/2 port (the mouse port)
    
    dataport.Write(0xF4); //activates the keyboard - the mouse will start sending data packets
    dataport.Read(); //read an OK response from the mouse after it is activated
}

//handle mouse interrupts
uint32_t MouseDriver::HandleInterrupt(uint32_t esp){
    
    uint8_t status = commandport.Read();
            
    if(!(status & 0x20)){ //only if the sixth bit of status is 1 there is data to read
        return esp;
    }
    
    //insert the data into the buffer at the current location
    buffer[offset] = dataport.Read();
    
    if(handler ==0){ 
        return esp;
    }
    //increase the offset with every received interrupts
    offset = (offset + 1) %3;

    
    //after receiving three interrupts, pass the mouse packet to the mouse event handler
    if(offset ==0){
        if(buffer[1] !=0 || buffer[2] != 0 ){
            handler->OnMouseMove((int8_t)buffer[1], - ((int8_t)buffer[2]));
            
        } 
       
    
        //check all 3 buttons 
        for(uint8_t i =0; i<3; i++){
            if((buffer[0] & (0x01 <<i)) != (buttons & (0x01<<i))){ //if a button was pressed
             
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