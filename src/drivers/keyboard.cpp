/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <drivers/keyboard.h>
#include <common/coolio.h>


using namespace coolOS::common;
using namespace coolOS::drivers;
using namespace coolOS::hardwarecommunication;

// TODO: add enumeration for each key 

KeyBoardEventHandler::KeyBoardEventHandler() {

}
void KeyBoardEventHandler::OnKeyDown(char key){
    
    
}
void KeyBoardEventHandler::OnKeyUp(char key){
    
    
}

KeyboardDriver::KeyboardDriver(InterruptManager* manager, KeyBoardEventHandler * handler) 
: InterruptHandler(manager, 0x21), //keyboard interrupt number is 0x21
  dataport(0x60), commandport(0x64)
{
    this->handler = handler;
}
KeyboardDriver::~KeyboardDriver(){
    
   
}


void KeyboardDriver::Activate(){
    
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

uint32_t KeyboardDriver::HandleInterrupt(uint32_t esp){
    
    uint8_t key = dataport.Read();
    
    if(handler ==0){
        return esp;
    }
        
    static bool Shift = false;
       //TODO: add capital letters
    switch(key){
        case 0xFA: break;
        //switch y and z

        case 0x02: handler->OnKeyDown('1'); break;
        case 0x03: handler->OnKeyDown('2'); break;
        case 0x04: handler->OnKeyDown('3'); break;
        case 0x05: handler->OnKeyDown('4'); break;
        case 0x06: handler->OnKeyDown('5'); break;
        case 0x07: handler->OnKeyDown('6'); break;
        case 0x08: handler->OnKeyDown('7'); break;
        case 0x09: handler->OnKeyDown('8'); break;
        case 0x0A: handler->OnKeyDown('9'); break;
        case 0x0B: handler->OnKeyDown('0'); break;
        case 0x0C: handler->OnKeyDown('-'); break;

        case 0x10: handler->OnKeyDown('q'); break;
        case 0x11: handler->OnKeyDown('w'); break;
        case 0x12: handler->OnKeyDown('e'); break;
        case 0x13: handler->OnKeyDown('r'); break;
        case 0x14: handler->OnKeyDown('t'); break;
        case 0x15: handler->OnKeyDown('y'); break;
        case 0x16: handler->OnKeyDown('u'); break;
        case 0x17: handler->OnKeyDown('i'); break;
        case 0x18: handler->OnKeyDown('o'); break;
        case 0x19: handler->OnKeyDown('p'); break;

        case 0x1E: handler->OnKeyDown('a'); break;
        case 0x1F: handler->OnKeyDown('s'); break;
        case 0x20: handler->OnKeyDown('d'); break;
        case 0x21: handler->OnKeyDown('f'); break;
        case 0x22: handler->OnKeyDown('g'); break;
        case 0x23: handler->OnKeyDown('h'); break;
        case 0x24: handler->OnKeyDown('j'); break;
        case 0x25: handler->OnKeyDown('k'); break;
        case 0x26: handler->OnKeyDown('l'); break;


        case 0x2C: handler->OnKeyDown('z'); break;
        case 0x2D: handler->OnKeyDown('x'); break;
        case 0x2E: handler->OnKeyDown('c'); break;
        case 0x2F: handler->OnKeyDown('v'); break;
        case 0x30: handler->OnKeyDown('b'); break;
        case 0x31: handler->OnKeyDown('n'); break;
        case 0x32: handler->OnKeyDown('m'); break;
        case 0x33: handler->OnKeyDown(','); break;
        case 0x34: handler->OnKeyDown('.'); break;
        case 0x35: handler->OnKeyDown('/'); break;

        case 0x1C: handler->OnKeyDown('\n'); break;
        case 0x39: handler->OnKeyDown(' '); break;

        //ignored 
        case 0x45: case 0xc5: break;

        case 0x2A: case 0x36: Shift = true; break;
        case 0xAA: case 0xB6: Shift = false; break;


        default:
            if(key < 0x80){ //everything after 0x80 is key-release interrupt
                            //we just want the key pressed interrupt for now
                printf("KEYBOARD 0x");
                printfHex(key);
                break;
            }
    } 
    
    return esp;
}