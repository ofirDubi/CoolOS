/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
//this is the Global Descriptor Table

#include <gdt.h>

using namespace coolOS::common;
using namespace coolOS;

#define _16BIT 65536
//#define testGdt
//#define stableGDT
#define structGDT
// Lets us access our ASM functions from our C code.
#ifdef testGdt
extern void gdt_flush(uint32_t);
#endif
extern "C" void gdt_flush(uint32_t);

GlobalDescriptorTable::GlobalDescriptorTable() //create table
{
    
    setSegmentDescriptor(&nullSegmentSelector,0,0,0,0);
    setSegmentDescriptor(&unusedSegmentSelector, 0,0,0,0);
    setSegmentDescriptor(&codeSegmentSelector, 0,64*1024*1024, 0x9A,0); //0x9a is access flag - read wiki.osdev.org/global_descripto_table
    setSegmentDescriptor(&dataSegmentSelector, 0,64*1024*1024,0x92,0); //0x92 is access flag
    setSegmentDescriptor(&userspaceCodeSegmentSelector, 0,64*1024*1024, 0xFA,0);
    setSegmentDescriptor(&userspaceDataSegmentSelector, 0,64*1024*1024, 0xF2,0);
    
    SegmentDescriptor sd_array[6] = {nullSegmentSelector, unusedSegmentSelector, codeSegmentSelector, dataSegmentSelector, userspaceCodeSegmentSelector, userspaceDataSegmentSelector};
    //SegmentDescriptor sd_array[4] = {nullSegmentSelector, unusedSegmentSelector, codeSegmentSelector, dataSegmentSelector};
    gdt_ptr ptr;        
    ptr.limit = (sizeof(SegmentDescriptor) * 6) - 1;
    ptr.base  = (uint32_t)&sd_array;
    printf("calling flush");
    gdt_flush((uint32_t)&ptr);
    printf("flushed");

    //working version:
    
    /*tss
    uint32_t base = (uint32_t)&tss_entry;
    uint32_t limit = base + sizeof(tss_entry);
    tssSelector(base, limit, 0xE9, 1);
   */
    
   
   //
    //asm volatile("lgdt (%0)": :"p" (((uint8_t *)&ptr)+13) ); //lgdt - asm commmand that loads the descriptor table
  //  asm volatile("ltr (%0)": :"Nd" (0x2B) ); //ltr - lods task state register                                                  

#ifdef testGdt
    //test version
    //tell the processor to use table
    SegmentDescriptor gdt[5] = {
        nullSegmentSelector,
        codeSegmentSelector,
        dataSegmentSelector,
        userspaceCodeSegmentSelector,
        userspaceDataSegmentSelector
    }; 
    
    gdt_ptr ptr;
    
    ptr.limit = (sizeof(SegmentDescriptor) * 5)-1;
    ptr.base = (uint32_t)&gdt;
    gdt_flush((uint32_t)&ptr);
    asm volatile("lgdt (%0)": :"p" (((uint32_t)&ptr)+4)); //lgdt - asm commmand that loads the descriptor table

#endif
}
        

GlobalDescriptorTable::~GlobalDescriptorTable() {
    //should unload the table
}

uint16_t GlobalDescriptorTable::getDataSegmentSelector(){ //returns the data segment offset
    return (uint8_t*)&dataSegmentSelector - (uint8_t*)this;
}

uint16_t GlobalDescriptorTable::getCodeSegmentSelector(){ //returns the code segment offset
    return (uint8_t*)&codeSegmentSelector - (uint8_t*)this;
}

void GlobalDescriptorTable::setSegmentDescriptor(SegmentDescriptor * sd, uint32_t base, uint32_t limit, uint8_t flags, uint8_t granualarity_flag ){
    
    
    uint8_t* target = (uint8_t *)sd;
    
    if(granualarity_flag==1){
        target[6] = 0;
        target[6] |= 0xF0;
    }
    else if(limit <= _16BIT){ //limit is smaller then 16 bit
        target[6] = 0x40; //16 bit mode
    } 
    else{
        if((limit & 0xfff) != 0xfff){ //if the last bits are not all one's
            limit = (limit>>12)-1;
        }else{
            limit = (limit >> 12);
        }
        
        target[6] = 0xC0; //not 16 bit mode
    }
    //encode limit
    target[0] = (limit & 0xff);
    target[1] = (limit >>8 ) &0xff;
    target[6] |= (limit>>16 ) & 0xf;
    
    //encode base pointer
    
    target[2] = base & 0xff;
    target[3] = (base >> 8)& 0xff;
    target[4] = (base >> 16)& 0xff;
    target[7] = (base >> 24)& 0xff;

    //encode flags
    target[5] = flags;

}
/*
 * 
 * old version
 * 
 * 
//decode base
uint32_t GlobalDescriptorTable::SegmentDescriptor::Base(){
    uint8_t* target = (uint8_t *)this;
    uint32_t result = target[7];
    result = ( result <<8 ) + target[4];
    result = ( result <<8 ) + target[3];
    result = ( result <<8 ) + target[2];
    return result;
}

//decode limit
uint32_t GlobalDescriptorTable::SegmentDescriptor::Limit(){
    uint8_t* target = (uint8_t *)this;
    uint32_t result = target[6] & 0xf;
    result = ( result <<8 ) + target[1];
    result = ( result <<8 ) + target[0];
       
    if((target[6]&0xc0) == 0xc0){
         result = (result<<12) | 0xfff;
    }
   
}
 
 */