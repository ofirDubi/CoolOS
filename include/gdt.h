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


#ifndef __COMMON__HARDWARECOMMUNICATION__GDT_H
#define __COMMON__HARDWARECOMMUNICATION__GDT_H

#include <common/types.h>
//#include <tss.h>

namespace coolOS{

            class GlobalDescriptorTable
            {
            public:
                class SegmentDescriptor{
                private:
                    coolOS::common::uint16_t limit_lo;
                    coolOS::common::uint16_t base_lo;
                    coolOS::common::uint8_t base_hi;
                    coolOS::common::uint8_t type;
                    coolOS::common::uint8_t flags_limit_hi;
                    coolOS::common::uint8_t base_vhi;
                public:
                    SegmentDescriptor(coolOS::common::uint32_t base, coolOS::common::uint32_t limit, coolOS::common::uint8_t type);
                    coolOS::common::uint32_t Base();
                    coolOS::common::uint32_t Limit();


                }__attribute__((packed));
            
                
                
            SegmentDescriptor nullSegmentSelector;
            SegmentDescriptor unusedSegmentSelector; //check why there is a need for this
            SegmentDescriptor codeSegmentSelector;
            SegmentDescriptor dataSegmentSelector;
            
            SegmentDescriptor userspaceCodeSegmentSelector;
            SegmentDescriptor userspaceDataSegmentSelector;
            
            //SegmentDescriptor tss;
            struct gdt_ptr_struct
            {
               common::uint16_t limit;               // The upper 16 bits of all selector limits.
               common::uint32_t base;                // The address of the first gdt_entry_t struct.
            }__attribute__((packed));
            
            typedef struct gdt_ptr_struct gdt_ptr;      
           

        public:
            GlobalDescriptorTable();
            ~GlobalDescriptorTable();

            coolOS::common::uint16_t getCodeSegmentSelector();
            coolOS::common::uint16_t getDataSegmentSelector();

    
    };

}

#endif /* GDT_H */

