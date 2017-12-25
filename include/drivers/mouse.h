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

#ifndef __COOLOS_DRIVERS__MOUSE_H
#define __COOLOS_DRIVERS__MOUSE_H


#include <common/types.h>
#include <hardwarecommunication/interrupts.h>
#include <hardwarecommunication/port.h>
#include "driver.h"

namespace coolOS{
    namespace drivers{
        
        class MouseEventHandler{




        public:
            MouseEventHandler();
            virtual void OnActivate();
            virtual void OnMouseDown(coolOS::common::uint8_t button);
            virtual void OnMouseUp(coolOS::common::uint8_t button);
            virtual void OnMouseMove(coolOS::common::int8_t x, coolOS::common::int8_t y);
        };


        class MouseDriver : public coolOS::hardwarecommunication::InterruptHandler, public Driver{

            coolOS::hardwarecommunication::Port<coolOS::common::uint8_t> dataport;
            coolOS::hardwarecommunication::Port<coolOS::common::uint8_t> commandport;

            coolOS::common::uint8_t buffer[3];
            coolOS::common::uint8_t offset;
            coolOS::common::uint8_t buttons;

            MouseEventHandler * handler;
            public:
                MouseDriver(coolOS::hardwarecommunication::InterruptManager * manager, MouseEventHandler * handler);
                ~MouseDriver();
                virtual coolOS::common::uint32_t HandleInterrupt(coolOS::common::uint32_t esp);
                virtual void Activate();

        };

    }
}

#endif /* KEYBOARD_H */

