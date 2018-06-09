/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   keyboard.h
 * Author: ofir123dubi
 *
 * Created on December 17, 2017, 1:23 PM
 */

#ifndef __COOLOS_DRIVERS__KEYBOARD_H
#define __COOLOS_DRIVERS__KEYBOARD_H


#include <common/types.h>
#include <hardwarecommunication/interrupts.h>
#include <hardwarecommunication/port.h>
#include <drivers/driver.h>

namespace coolOS{
    namespace drivers{
        //a class to handle key events 
        class KeyBoardEventHandler{
        public:
            KeyBoardEventHandler();
            ~KeyBoardEventHandler();

            virtual void OnKeyDown(common::uint8_t key);
            virtual void OnKeyUp(common::uint8_t key);
        };

        
        /*a keyboard driver. this class should communicate with the keyboard 
          and activate the KeyboardEventHandler's OnKeyUp and OnKeyDown methods */
        class KeyboardDriver : public coolOS::hardwarecommunication::InterruptHandler, public Driver{
        private:
            //ports for communication with the keyboard
            coolOS::hardwarecommunication::Port<coolOS::common::uint8_t> dataport;
            coolOS::hardwarecommunication::Port<coolOS::common::uint8_t> commandport;
            KeyBoardEventHandler * handler;
        public:
                KeyboardDriver(coolOS::hardwarecommunication::InterruptManager * manager, KeyBoardEventHandler * handler);
                ~KeyboardDriver();
                //override the InterruptHandler methods
                virtual coolOS::common::uint32_t HandleInterrupt(coolOS::common::uint32_t esp);
                virtual void Activate();
        };

    }
}

#endif /* KEYBOARD_H */

