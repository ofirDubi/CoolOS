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
#include <drivers/driver.h>

namespace coolOS{
    namespace drivers{
        //a class for handling mouse events. should be inherited from by other mouse event handlers.
        class MouseEventHandler{

        public:
            MouseEventHandler();
            //mouse pressed
            virtual void OnMouseDown(coolOS::common::uint8_t button);
            //mouse released
            virtual void OnMouseUp(coolOS::common::uint8_t button);
            //mouse moved
            virtual void OnMouseMove(int x, int  y);
        };

        // a mouse driver
        class MouseDriver : public coolOS::hardwarecommunication::InterruptHandler, public Driver{
            //ports for communicating withe the PS/2 controller
            coolOS::hardwarecommunication::Port<coolOS::common::uint8_t> dataport;
            coolOS::hardwarecommunication::Port<coolOS::common::uint8_t> commandport;
            
            /*
             * buffer[0] is a set of flags - including three button click flags.
               buffer[1] - movement on x axis
             * buffer[2] - movement on y axis
             */
            coolOS::common::uint8_t buffer[3];
            
            //which part of the buffer is being received in a specific interrupt
            coolOS::common::uint8_t offset;
          
            //will hold the clicked button number
            coolOS::common::uint8_t buttons;

            //a pointer to the current mouse handler
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

