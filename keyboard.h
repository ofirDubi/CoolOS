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

#ifndef __KEYBOARD_H
#define __KEYBOARD_H


#include "types.h"
#include "interrupts.h"
#include "port.h"


class KeyboardDriver : public InterruptHandler{
    
    Port<uint8_t> dataport;
    Port<uint8_t> commandport;
    public:
        KeyboardDriver(InterruptManager * manager);
        ~KeyboardDriver();
        virtual uint32_t HandleInterrupt(uint32_t esp);
};

#endif /* KEYBOARD_H */

