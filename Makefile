# Makefile of Orange'S

# kernel entry point, corresponding with boot/include/load.inc
ENTRYPOINT	= 0x1000

FD		= fd.img
HD		= 80m.img

ASM		= nasm
DASM		= objdump
CC		= gcc
LD		= ld
AR		= ar
ASMBFLAGS	= -I boot/include/
ASMKFLAGS	= -I include/ -I include/sys/ -f elf
#CFLAGS		= -I include/ -I include/sys/ -c -fno-builtin -fno-stack-protector -fpack-struct -Wall
CFLAGS		= -I include/ -I include/sys/ -c -fno-builtin -fno-stack-protector -Wall -g
LDFLAGS		= -Ttext $(ENTRYPOINT) -Map krnl.map
DASMFLAGS	= -D
ARFLAGS		= rcs

# source
BSRC		:= $(wildcard boot/*.asm)
KSRC		:= $(wildcard kernel/*.c) $(wildcard fs/*.c) $(wildcard mm/*.c)
KASMSRC		:= $(wildcard kernel/*.asm)
LSRC		:= $(wildcard lib/*.c)
LASMSRC		:= $(wildcard lib/*.asm)

# c objs
KOBJS		:= $(subst .c,.o,$(KSRC))
LOBJS		:= $(subst .c,.o,$(LSRC))

# asm objs
KASMOBJS	:= $(subst .asm,.o,$(KASMSRC))
LASMOBJS	:= $(subst .asm,.o,$(LASMSRC))

# dependencies
DEPDIR		:= .mkdep
BDEP		:= $(patsubst %.asm,$(DEPDIR)/%.d,$(BSRC))
KDEP		:= $(patsubst %.c,$(DEPDIR)/%.d,$(KSRC))
KASMDEP		:= $(patsubst %.asm,$(DEPDIR)/%.d,$(KASMSRC))
LDEP		:= $(patsubst %.c,$(DEPDIR)/%.d,$(LSRC))
LASMDEP		:= $(patsubst %.asm,$(DEPDIR)/%.d,$(LASMSRC))

# lib
LIB		:= lib/orangescrt.a

# targets
BOOT_BIN	:= $(subst .asm,.bin,$(BSRC))
KRNL_BIN	:= kernel.bin

DASM_OUTPUT	:= kernel.bin.asm

# All Phony Targets
.PHONY : everything final image clean realclean disasm all buildimg test

everything : $(BOOT_BIN) $(KRNL_BIN)

all : realclean everything prnt_krnl_info

final : clean everything buildimg clean

image : everything buildimg

clean :
	rm -f $(KOBJS) $(LOBJS) $(KASMOBJS) $(LASMOBJS) $(BOOT_BIN) $(KRNL_BIN) $(LIB)

realclean :
	find . -regex '.*\.\([aod]\|bin\)' -exec rm -f {} +

disasm :
	$(DASM) $(DASMFLAGS) $(KRNL_BIN) > $(DASM_OUTPUT)

prnt_krnl_info :
	@./scripts/kinfo

# Write "boot.bin" & "loader.bin" into floppy image
buildimg :
	@echo -e '\033[31m'
	dd if=boot/boot.bin of=$(FD) bs=512 count=1 conv=notrunc
	dd if=boot/hdboot.bin of=$(HD) seek=`echo "obase=10;ibase=16;\`egrep -e '^ROOT_BASE' boot/include/load.inc | sed -e 's/.*0x//g'\`*200" | bc` bs=1 count=446 conv=notrunc
	dd if=boot/hdboot.bin of=$(HD) seek=`echo "obase=10;ibase=16;\`egrep -e '^ROOT_BASE' boot/include/load.inc | sed -e 's/.*0x//g'\`*200+1FE" | bc` skip=510 bs=1 count=2 conv=notrunc
	sudo mount $(FD) /mnt/floppy -o loop
	sudo cp -f boot/loader.bin /mnt/floppy
	strip kernel.bin -o kernel.bin.stripped
	sudo cp -fv kernel.bin.stripped /mnt/floppy/kernel.bin
	sudo umount /mnt/floppy
	@echo -e '\033[0m'


-include $(BDEP)
-include $(KDEP)
-include $(KASMDEP)
-include $(LDEP)
-include $(LASMDEP)

$(BOOT_BIN) : %.bin : %.asm
	@echo -n -e '\033[1;32m'
	$(ASM) $(ASMBFLAGS) -o $@ $<
	@echo -n -e '\033[0m'

$(KRNL_BIN) : $(KOBJS) $(KASMOBJS) $(LIB)
	@echo -n -e '\033[36m'
	$(LD) $(LDFLAGS) -o $@ kernel/kernel.o $(filter-out kernel/kernel.o,$^)
	@echo -n -e '\033[0m'

$(LIB) : $(LOBJS) $(LASMOBJS)
	@echo -n -e '\033[34m'
	$(AR) $(ARFLAGS) $@ $^
	@echo -n -e '\033[0m'

$(KOBJS) : %.o : %.c
	@echo -n -e '\033[32m'
	$(CC) $(CFLAGS) -o $@ $<
	@echo -n -e '\033[0m'

$(LOBJS) : %.o : %.c
	@echo -n -e '\033[32m'
	$(CC) $(CFLAGS) -o $@ $<
	@echo -n -e '\033[0m'

$(KASMOBJS) : %.o : %.asm
	@echo -n -e '\033[1;32m'
	$(ASM) $(ASMKFLAGS) -o $@ $<
	@echo -n -e '\033[0m'

$(LASMOBJS) : %.o : %.asm
	@echo -n -e '\033[1;32m'
	$(ASM) $(ASMKFLAGS) -o $@ $<
	@echo -n -e '\033[0m'

$(KDEP) : $(DEPDIR)/%.d : %.c
	@echo -n -e '\033[37m'
	$(CC) $(CFLAGS) -MM $< -MT $(subst .c,.o,$<) -MF $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
	@echo -n -e '\033[0m'

$(LDEP) : $(DEPDIR)/%.d : %.c
	@echo -n -e '\033[37m'
	$(CC) $(CFLAGS) -MM $< -MT $(subst .c,.o,$<) -MF $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
	@echo -n -e '\033[0m'

$(BDEP) : $(DEPDIR)/%.d : %.asm
	@echo -n -e '\033[37m'
	$(ASM) $(ASMBFLAGS) -M $< -o $(subst .asm,.bin,$<) > $@.$$$$; \
	sed 's,\($*\)\.bin[ :]*,\1.bin $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
	@echo -n -e '\033[0m'

$(KASMDEP) : $(DEPDIR)/%.d : %.asm
	@echo -n -e '\033[37m'
	$(ASM) $(ASMKFLAGS) -M $< -o $(subst .asm,.bin,$<) > $@.$$$$; \
	sed 's,\($*\)\.bin[ :]*,\1.bin $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
	@echo -n -e '\033[0m'

$(LASMDEP) : $(DEPDIR)/%.d : %.asm
	@echo -n -e '\033[37m'
	$(ASM) $(ASMKFLAGS) -M $< -o $(subst .asm,.bin,$<) > $@.$$$$; \
	sed 's,\($*\)\.bin[ :]*,\1.bin $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
	@echo -n -e '\033[0m'

