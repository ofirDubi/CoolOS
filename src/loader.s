/*bootloader header variables*/
.set MAGIC, 0x1badb002 /* a magic number that lets the bootloader detect the header*/
.set FLAGS, (1<<0 | 1<<1) /*multiboot flags - tells the bootloader to provide a memory map and to load all the modules on this page */ 
.set CHECKSUM, -(MAGIC + FLAGS) /* multiboot checksum*/

/*declare multiboot header in a different section*/
.section .multiboot
    .long MAGIC
    .long FLAGS
    .long CHECKSUM


.section .text
.extern kernelMain
.extern callConstructors
.global loader

loader:
/*move stack pointer to our uninitialized stack*/
    mov $kernel_stack, %esp

    call callConstructors /*call linker constructors*/

    /*push multiboot structure */
    push %eax

    /*push magic number*/
    push %ebx

    /*call the kernel function*/
    call kernelMain

/* if the kernel has nothing else to do, go into an infinite loop*/

    cli /*clear interrupts*/
_stop:
    hlt /*wait for an interrupt - should never happen*/
    jmp _stop /*if it does happen, jump to it again*/



.section .bss
/*initialize stack */
.space 4*1024 * 1024 #4 MB
kernel_stack:
