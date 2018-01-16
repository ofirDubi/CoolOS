/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include<tss.h>

using namespace coolOS;
using namespace coolOS::common;

//to be called when switching tasks
void setKernelStack(uint32_t stack, tss_entry_t tss_entry){
    tss_entry.esp0 = stack;
    
}