/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   port.h
 * Author: ofir123dubi
 *
 * Created on December 14, 2017, 8:16 AM
 */

#ifndef __COOLOS__HARDWARECOMMUNICATION__PORT_H
#define __COOLOS__HARDWARECOMMUNICATION__PORT_H

#include <common/types.h>

//a macro for writing to a port. should be pasted inside a Write function
#define WriteToPort(command) __asm__ volatile(command: : "a"(data), "Nd"(port_number) );

//#define SlowWriteToPort(size) __asm__ volatile(size: : "a"(data), "Nd"(port_number) );
//a macro for reading from a port. should be pasted inside a Read function
#define ReadFromPort(command) __asm__ volatile(command: "=a" (result) : "Nd" (port_number));

#define ChoosePortSize(option_b, option_w, option_l) { /*a macro that checks the number of bytes in DataType */ \
    switch(sizeof(DataType)){\
            case 1: /*if size is one, then we have a uint8_t variable. choose byte option */\
                option_b;\
                break;\
            case 2:/*if size is two, then we have a uint16_t variable. choose word option */\
                option_w;\
                break;\
            case 4: /*if size is four, then we have a uint32_t variable. choose double word option */\
                option_l;\
                break;\
            default:\
                option_b;\
                break;\
    }\
}

namespace coolOS{
    namespace hardwarecommunication{



        template<class DataType>
        class Port { 
        public:
                coolOS::common::uint16_t port_number; 
                Port(coolOS::common::uint16_t port_number);
                ~Port();
                virtual void Write(DataType data); //number 
                virtual void SlowWrite(DataType data);
                virtual DataType Read();

        }; 
        template<class DataType> Port<DataType>::Port(coolOS::common::uint16_t port_number){
            this->port_number = port_number;


        }

        template<class DataType> Port<DataType>::~Port(){

        }
        // a function that writes data to the port in port_number
        template<class DataType>  void Port<DataType>::Write(DataType data){


            ChoosePortSize(WriteToPort("outb %0, %1"), WriteToPort("outw %0, %1"), WriteToPort("outl %0, %1"));

        }
        // a function that writes data and then waits a little bit. for slow ports 
        template<class DataType> void Port<DataType>::SlowWrite(DataType data){

            ChoosePortSize(WriteToPort("outb %0, %1\nnop\nnop\nnop"), WriteToPort("outw %0, %1\nnop\nnop\nnop"),
                    WriteToPort("outl %0, %1\nnop\nnop\nnop"));

        }
        // a function that reads from the port in port_number
        template<class DataType> DataType Port<DataType>::Read(){

            DataType result;
            ChoosePortSize(ReadFromPort("inb %1, %0"), ReadFromPort("inw %1, %0"), ReadFromPort("inl %1, %0"));

            return result;

        }
    }
}
#endif /* PORT_H */

