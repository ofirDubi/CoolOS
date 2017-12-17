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

#ifndef __PORT_H
#define __PORT_H
#include "types.h"

#define WriteToPort(size) __asm__ volatile(size: : "a"(data), "Nd"(port_number) );

#define SlowWriteToPort(size) __asm__ volatile(size: : "a"(data), "Nd"(port_number) );

#define ReadFromPort(size) __asm__ volatile(size: "=a" (result) : "Nd" (port_number));

#define ChoosePortSize(option_b, option_w, option_l) {\
    switch(sizeof(DataType)){\
            case 1:\
                option_b;\
                break;\
            case 2:\
                option_w;\
                break;\
            case 4:\
                option_l;\
                break;\
            default:\
                option_b;\
                break;\
    }\
}

    /*TODO:
        add a SlowPort */
    template<class DataType>
    class Port { 
    public:
            uint16_t port_number;
            Port(uint16_t port_number);
            ~Port();
            virtual void Write(DataType data);
            virtual void SlowWrite(DataType data);
            virtual DataType Read();
            
    }; 
    template<class DataType> Port<DataType>::Port(uint16_t port_number){
        this->port_number = port_number;
        
        
    }
    
    template<class DataType> Port<DataType>::~Port(){
        
    }
    
    template<class DataType>  void Port<DataType>::Write(DataType data){
       
        
        ChoosePortSize(WriteToPort("outb %0, %1"), WriteToPort("outw %0, %1"), WriteToPort("outl %0, %1"));
    
    }
    
    template<class DataType> void Port<DataType>::SlowWrite(DataType data){

        ChoosePortSize(WriteToPort("outb %0, %1\njmp lf\nlf:"), WriteToPort("outw %0, %1\njmp lf\nlf:"),
                WriteToPort("outl %0, %1\njmp lf\nlf:"));

    }
    
    template<class DataType> DataType Port<DataType>::Read(){
        
        DataType result;
        
        ChoosePortSize(ReadFromPort("inb %1, %0"), ReadFromPort("inw %1, %0"), ReadFromPort("inl %1, %0"));

        return result;
    
    }
    
#endif /* PORT_H */

