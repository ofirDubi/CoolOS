GPPPARAMS = -m32 -Iinclude -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore -fpermissive
ASPARAMS = --32
LDPARAMS = -melf_i386

objects = obj/common/coolio.o \
	  obj/loader.o \
	  obj/gdt.o \
	  obj/tss.o \
	  obj/gdtflush.o \
	  obj/memorymanagment.o \
	  obj/drivers/driver.o \
	  obj/hardwarecommunication/port.o \
	  obj/hardwarecommunication/interrupts.o \
	  obj/hardwarecommunication/interruptstubs.o \
	  obj/syscalls.o \
	  obj/multitasking.o \
	  obj/drivers/amd_am79c973.o \
	  obj/hardwarecommunication/pci.o \
	  obj/drivers/keyboard.o \
	  obj/drivers/mouse.o \
	  obj/drivers/vga.o \
	  obj/drivers/ata.o \
	  obj/gui/widget.o \
	  obj/gui/window.o \
	  obj/gui/desktop.o \
	  obj/kernel.o

obj/%.o: src/%.cpp
	mkdir -p $(@D)
	g++ $(GPPPARAMS) -o $@ -c $<
obj/%.o: src/%.s
	mkdir -p $(@D)
	as $(ASPARAMS) -o $@ $<

mykernel.bin: linker.ld $(objects)
	ld $(LDPARAMS) -T $< -o $@ $(objects)
	


mykernel.iso: mykernel.bin
	mkdir iso
	mkdir iso/boot
	mkdir iso/boot/grub
	cp  mykernel.bin iso/boot/mykernel.bin
	echo 'set timeout=0' >> iso/boot/grub/grub.cfg
	echo 'set default=0' >> iso/boot/grub/grub.cfg
	echo '' >> iso/boot/grub/grub.cfg
	echo 'menuentry "CoolOs" {' >> iso/boot/grub/grub.cfg
	echo '	multiboot /boot/mykernel.bin' >> iso/boot/grub/grub.cfg
	echo '	boot' >> iso/boot/grub/grub.cfg
	echo '}' >> iso/boot/grub/grub.cfg
	grub-mkrescue --output=mykernel.iso iso
	rm -rf iso

run: mykernel.iso
	(killall VirtualBox && sleep1) || true
	VirtualBox --startvm "CoolOS" &
	
install: mykernel.bin
	sudo cp $< /boot/mykernel.bin
	
.PHONY: clean
clean:
	rm -rf $ obj mykernel.bin mykernel.iso
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
