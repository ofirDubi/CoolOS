/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   new_gdt.cpp
 * Author: ofir123dubi
 * 
 * Created on June 4, 2018, 6:49 AM
 */

#include <gdt_new.h>

using namespace coolOS;
using namespace coolOS::common;
gdt_entry kgdt[0xff];
tss default_tss;
void * coolOS::memcpy(char *dst, char *src, int n)
{
	char *p = dst;
	while (n--)
		*dst++ = *src++;
	return p;
}

uint16_t  coolOS::GDTGetCodeSegment(){
    return   (uint16_t)(&kgdt[1]) - (uint16_t)kgdt;
}

void coolOS::init_gdt(void)
{
 gdtr_struct kgdtr;

    default_tss.debug_flag = 0x00;
    default_tss.io_map = 0x00;
    default_tss.esp0 = 0x1FFF0;
    default_tss.ss0 = 0x18;
    
    /* initialize gdt segments */
    init_gdt_desc(0x0, 0x0, 0x0, 0x0, &kgdt[0]);
    init_gdt_desc(0x0, 0xFFFFF, 0x9B, 0x0D, &kgdt[1]);    /* kernel code segment */
    init_gdt_desc(0x0, 0xFFFFF, 0x93, 0x0D, &kgdt[2]);    /* kernel data segment*/
    init_gdt_desc(0x0, 0x0, 0x97, 0x0D, &kgdt[3]);        /* kernel stack segment*/

    init_gdt_desc(0x0, 0xFFFFF, 0xFF, 0x0D, &kgdt[4]);    /* user code segment */
    init_gdt_desc(0x0, 0xFFFFF, 0xF3, 0x0D, &kgdt[5]);    /* user data segment*/
    init_gdt_desc(0x0, 0x0, 0xF7, 0x0D, &kgdt[6]);        /* user stack segment*/

    init_gdt_desc((uint32_t) & default_tss, 0x67, 0xE9, 0x00, &kgdt[7]);    /* descripteur de tss */

    /* initialize the gdtr structure */
    kgdtr.limite = 0xff * 8;
    kgdtr.base = 0x00000800;

    /* copy the gdtr to its memory area */
    memcpy((char *) kgdtr.base, (char *) kgdt, kgdtr.limite);

    /* load the gdtr registry */
    asm("lgdt (%0)": :"p" ((uint8_t *)&kgdtr) );

    /* initiliaz the segments */
    asm("   movw $0x10, %ax    \n \
            movw %ax, %ds    \n \
            movw %ax, %es    \n \
            movw %ax, %fs    \n \
            movw %ax, %gs    \n \
            ljmp $0x08, $next    \n \
            next:        \n");
}

void coolOS::init_gdt_desc(uint32_t base, uint32_t limite, uint8_t acces, uint8_t other,  gdt_entry *desc)
{
    desc->lim0_15 = (limite & 0xffff);
    desc->base0_15 = (base & 0xffff);
    desc->base16_23 = (base & 0xff0000) >> 16;
    desc->acces = acces;
    desc->lim16_19 = (limite & 0xf0000) >> 16;
    desc->other = (other & 0xf);
    desc->base24_31 = (base & 0xff000000) >> 24;
    return;
}
