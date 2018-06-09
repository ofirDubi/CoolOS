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

KeyBoardEventHandler::~KeyBoardEventHandler(){
    
}

void KeyBoardEventHandler::OnKeyDown(uint8_t key){
    
    
}
void KeyBoardEventHandler::OnKeyUp(uint8_t key){
    
    
}

KeyboardDriver::KeyboardDriver(InterruptManager* manager, KeyBoardEventHandler * handler) 
: InterruptHandler(manager, 0x21), //keyboard interrupt number is 0x21
  dataport(0x60), commandport(0x64)
{
    this->handler = handler;
}
KeyboardDriver::~KeyboardDriver(){
    
   
}

//activate the keyboard through the PS/2 controller
void KeyboardDriver::Activate(){
    //wait until the keyboard hardware has powered up
    while(commandport.Read() & 0x1){
        dataport.Read();
    }
    commandport.Write(0xAE); //Enables first PS/2 port 
    commandport.Write(0x20); //get the PS/2 control configuration byte 
    
	/*the dataport.Read() will return the control configuration mask. 
	it determents which PS/2 port is active and some other PS/2 controller settings.
	we enable the keyboard PS/2 port by changing the register*/
    uint8_t status = (dataport.Read() | 1) & ~0x10; 
    
	
    commandport.Write(0x60); //command to set the new configuration byte
    dataport.Write(status); //pass the status as the new configuration byte
    
    
    dataport.Write(0xF4); //activates the keyboard - the keyboard will start sending scan codes
}

//handle keyboard interrupts - call the driver's OnKeyUp or OnKeyDown
uint32_t KeyboardDriver::HandleInterrupt(uint32_t esp){
    
    //receive the scan code
    uint8_t key = dataport.Read();   

    
    if(handler ==0){
        return esp;
    }
    
    //create a function pointer that will point on OnKeyUp or OnKeyDown
    typedef void (coolOS::drivers::KeyBoardEventHandler::*KeyFuncType)(uint8_t);
    KeyFuncType KeyFunc; 
    
     //the seventh bit is a flag that is 1 for key up and for key down, so everything after 0x80 is a key-release
    if(key > 0x80){
        KeyFunc = &coolOS::drivers::KeyBoardEventHandler::OnKeyUp;
        key -= 0x80;
    }else{
        KeyFunc = &coolOS::drivers::KeyBoardEventHandler::OnKeyDown;
    }
    
    static bool Shift = false;
       //TODO: add capital letters
    //turn scan codes into ASCII codes.
    switch(key){
        case 0xFA: break;

        case 0x02: (handler->*KeyFunc)('1'); break;
        case 0x03: (handler->*KeyFunc)('2'); break;
        case 0x04: (handler->*KeyFunc)('3'); break;
        case 0x05: (handler->*KeyFunc)('4'); break;
        case 0x06: (handler->*KeyFunc)('5'); break;
        case 0x07: (handler->*KeyFunc)('6'); break;
        case 0x08: (handler->*KeyFunc)('7'); break;
        case 0x09: (handler->*KeyFunc)('8'); break;
        case 0x0A: (handler->*KeyFunc)('9'); break;
        case 0x0B: (handler->*KeyFunc)('0'); break;
        case 0x0C: (handler->*KeyFunc)('-'); break;

        case 0x10: (handler->*KeyFunc)('q'); break;
        case 0x11: (handler->*KeyFunc)('w'); break;
        case 0x12: (handler->*KeyFunc)('e'); break;
        case 0x13: (handler->*KeyFunc)('r'); break;
        case 0x14: (handler->*KeyFunc)('t'); break;
        case 0x15: (handler->*KeyFunc)('y'); break;
        case 0x16: (handler->*KeyFunc)('u'); break;
        case 0x17: (handler->*KeyFunc)('i'); break;
        case 0x18: (handler->*KeyFunc)('o'); break;
        case 0x19: (handler->*KeyFunc)('p'); break;

        case 0x1E: (handler->*KeyFunc)('a'); break;
        case 0x1F: (handler->*KeyFunc)('s'); break;
        case 0x20: (handler->*KeyFunc)('d'); break;
        case 0x21: (handler->*KeyFunc)('f'); break;
        case 0x22: (handler->*KeyFunc)('g'); break;
        case 0x23: (handler->*KeyFunc)('h'); break;
        case 0x24: (handler->*KeyFunc)('j'); break;
        case 0x25: (handler->*KeyFunc)('k'); break;
        case 0x26: (handler->*KeyFunc)('l'); break;


        case 0x2C: (handler->*KeyFunc)('z'); break;
        case 0x2D: (handler->*KeyFunc)('x'); break;
        case 0x2E: (handler->*KeyFunc)('c'); break;
        case 0x2F: (handler->*KeyFunc)('v'); break;
        case 0x30: (handler->*KeyFunc)('b'); break;
        case 0x31: (handler->*KeyFunc)('n'); break;
        case 0x32: (handler->*KeyFunc)('m'); break;
        case 0x33: (handler->*KeyFunc)(','); break;
        case 0x34: (handler->*KeyFunc)('.'); break;
        case 0x35: (handler->*KeyFunc)('/'); break;

        case 0x1C: (handler->*KeyFunc)('\n'); break;
        case 0x39: (handler->*KeyFunc)(' '); break;

        //ignored 
        case 0x45: case 0xc5: case 0x7A : break;

        case 0x2A: case 0x36: Shift = true; break;
        case 0xAA: case 0xB6: Shift = false; break;


        default:   
            
            printf("KEYBOARD 0x");
            printfHex(key);
            break;
           
    } 
    
    return esp;
}