/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   gdt.h
 * Author: ofir123dubi
 *
 * Created on December 14, 2017, 4:46 AM
 */


#ifndef __GDT_H
#define __GDT_H

#include "types.h"
class GlobalDescriptorTable
{
    public:
        class SegmentDescriptor{
        private:
            uint16_t limit_lo;
            uint16_t base_lo;
            uint8_t base_hi;
            uint8_t type;
            uint8_t flags_limit_hi;
            uint8_t base_vhi;
        public:
            SegmentDescriptor(uint32_t base, uint32_t limit, uint8_t type);
            uint32_t Base();
            uint32_t Limit();
            
            
        }__attribute__((packed));
        
    SegmentDescriptor nullSegmentSelector;
    SegmentDescriptor unusedSegmentSelector; //check why there is a need for this
    SegmentDescriptor codeSegmentSelector;
    SegmentDescriptor dataSegmentSelector;

public:
    GlobalDescriptorTable();
    ~GlobalDescriptorTable();
    
    uint16_t getCodeSegmentSelector();
    uint16_t getDataSegmentSelector();

};

#endif /* GDT_H */

