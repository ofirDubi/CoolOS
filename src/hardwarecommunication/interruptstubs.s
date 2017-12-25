
.set IRQ_BASE, 0x20

.section .text

.extern _ZN6coolOS21hardwarecommunication16InterruptManager15handleInterruptEhj #the name of the object file interrupt.o

.global _ZN6coolOS21hardwarecommunication16InterruptManager22IgnoreInterruptRequestEv 

.macro HandleException num 
.global _ZN6coolOS21hardwarecommunication16InterruptManager16HandleException\num\()Ev #check this line
_ZN6coolOS21hardwarecommunication16InterruptManager16HandleException\num\()Ev:
    movb $\num, (interruptnumber)
    jmp int_bottom
.endm

.macro HandleInterruptRequest num 
.global _ZN6coolOS21hardwarecommunication16InterruptManager26HandleInterruptRequest\num\()Ev #check this line
_ZN6coolOS21hardwarecommunication16InterruptManager26HandleInterruptRequest\num\()Ev:
    movb $\num + IRQ_BASE, (interruptnumber) #what is IRQ_Base
    jmp int_bottom
.endm


HandleInterruptRequest 0x00
HandleInterruptRequest 0x01
HandleInterruptRequest 0x0C


int_bottom:
    #save all the registers
    pusha
    pushl %ds
    pushl %es
    pushl %fs
    pushl %gs
    
    
    pushl %esp
    push (interruptnumber)
    call _ZN6coolOS21hardwarecommunication16InterruptManager15handleInterruptEhj
    # addl $5, %esp
    movl %eax, %esp
    
    #restore all the registers
    popl %gs
    popl %fs
    popl %es
    popl %ds
    popa
    
.global __ZN6coolOS21hardwarecommunication16InterruptManager15InterruptIgnoreEv
_ZN6coolOS21hardwarecommunication16InterruptManager22IgnoreInterruptRequestEv:

    #return to what it was doing before
    iret
    

.data 
    interruptnumber: .byte 0

