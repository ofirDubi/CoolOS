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

        class KeyBoardEventHandler{
        public:
            KeyBoardEventHandler();

            virtual void OnKeyDown(char key);
            virtual void OnKeyUp(char key);
        };

        class KeyboardDriver : public coolOS::hardwarecommunication::InterruptHandler, public Driver{

            coolOS::hardwarecommunication::Port<coolOS::common::uint8_t> dataport;
            coolOS::hardwarecommunication::Port<coolOS::common::uint8_t> commandport;

            KeyBoardEventHandler * handler;
        public:
                KeyboardDriver(coolOS::hardwarecommunication::InterruptManager * manager, KeyBoardEventHandler * handler);
                ~KeyboardDriver();
                virtual coolOS::common::uint32_t HandleInterrupt(coolOS::common::uint32_t esp);
                virtual void Activate();
        };

    }
}

#endif /* KEYBOARD_H */

