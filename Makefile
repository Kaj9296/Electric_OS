OUTPUTNAME = Kernel
OSNAME = Electric_OS

GNUEFI = gnu-efi
OVMFDIR = OVMFbin
LDS = src/linker.ld
CC = gcc
ASMC = nasm
LD = ld

CFLAGS = -Wall -fno-rtti -ffreestanding -fno-threadsafe-statics -fno-stack-protector -fno-exceptions -Isrc/ -std=c++2a -Os
ASMFLAGS =
LDFLAGS = -T $(LDS) -Bsymbolic -nostdlib

SRCDIR := src
OBJDIR := build
BINDIR = bin
FONTSDIR = fonts
BOOTEFI := $(GNUEFI)/x86_64/bootloader

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

SRC = $(call rwildcard,$(SRCDIR),*.cpp)
ASMSRC = $(call rwildcard,$(SRCDIR),*.asm)
OBJS = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRC))
OBJS += $(patsubst $(SRCDIR)/%.asm, $(OBJDIR)/%_ASM.o, $(ASMSRC))
DIRS = $(wildcard $(SRCDIR)/*)

kernel: $(OBJS) link

$(OBJDIR)/Kernel/Interrupts/Handlers.o: $(SRCDIR)/Kernel/Interrupts/Handlers.cpp
	@echo !==== COMPILING $^
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -mno-red-zone -mgeneral-regs-only -c $^ -o $@ 

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@echo !==== COMPILING $^
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $^ -o $@

$(OBJDIR)/%_ASM.o: $(SRCDIR)/%.asm
	@echo !==== COMPILING $^
	@mkdir -p $(@D)
	$(ASMC) $(ASMFLAGS) $^ -f elf64 -o $@

link:
	@echo !==== LINKING
	$(LD) $(LDFLAGS) -o $(BINDIR)/$(OUTPUTNAME).elf $(OBJS)

setup:
	@mkdir -p $(BINDIR)
	@mkdir -p $(SRCDIR)
	@mkdir -p $(OBJDIR)

buildimg:
	dd if=/dev/zero of=$(BINDIR)/$(OSNAME).img bs=512 count=93750
	sudo mformat -i $(BINDIR)/$(OSNAME).img -f 1440 ::
	mmd -i $(BINDIR)/$(OSNAME).img ::/EFI
	mmd -i $(BINDIR)/$(OSNAME).img ::/EFI/BOOT
	mmd -i $(BINDIR)/$(OSNAME).img ::/KERNEL
	mmd -i $(BINDIR)/$(OSNAME).img ::/FONTS
	cp $(BOOTEFI)/main.efi  $(BOOTEFI)/bootx64.efi
	mcopy -i $(BINDIR)/$(OSNAME).img $(BOOTEFI)/bootx64.efi ::/EFI/BOOT
	mcopy -i $(BINDIR)/$(OSNAME).img $(SRCDIR)/startup.nsh ::
	mcopy -i $(BINDIR)/$(OSNAME).img $(BINDIR)/$(OUTPUTNAME).elf ::/KERNEL
	mcopy -i $(BINDIR)/$(OSNAME).img $(FONTSDIR)/zap-vga16.psf ::/FONTS
	mcopy -i $(BINDIR)/$(OSNAME).img $(FONTSDIR)/zap-light16.psf ::/FONTS

all:
	@cd gnu-efi && make bootloader
	make setup
	make kernel
	make buildimg

run:
	qemu-system-x86_64 -drive file=$(BINDIR)/$(OSNAME).img -m 4G -cpu qemu64 -drive if=pflash,format=raw,unit=0,file="$(OVMFDIR)/OVMF_CODE-pure-efi.fd",readonly=on -drive if=pflash,format=raw,unit=1,file="$(OVMFDIR)/OVMF_VARS-pure-efi.fd" -net none
