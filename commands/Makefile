# commands/Makefile

#ENTRYPOINT	= 0x1000
HD		= ../80m.img

ASM		= nasm
DASM		= objdump
CC		= gcc
LD		= ld
ASMFLAGS	= -I ../include/ -f elf
#CFLAGS		= -I ../include/ -c -fno-builtin -fno-stack-protector -fpack-struct -Wall
#CFLAGS		= -I ../include/ -c -fno-builtin -Wall
CFLAGS		= -I ../include/ -c -fno-builtin -fno-stack-protector -Wall -g
LDFLAGS		= -Ttext 0x1000
DASMFLAGS	= -D
LIB		= ../lib/orangescrt.a
BIN		= ls touch echo pwd cat test01

# All Phony Targets
.PHONY : everything final clean realclean disasm all install

# Default starting position
everything : $(BIN)

install : all clean
	@echo -e '\033[31m'
	cp ../boot/hd*.bin ./ -v
	cp ../kernel.bin ./ -v
	tar vcf inst.tar kernel.bin $(BIN) hd*.bin proc.txt
	@echo "                                position of inst.tar = device_base + INSTALL_START_SECT * SECTOR_SIZE"
	@echo "                                ~~~~~~|~~~~~~~~~~~~~    (0x9DFE00) + (0x8000)           * (200)"
	@echo "                                      V"
	dd if=inst.tar of=$(HD) seek=`echo "obase=10;ibase=16;(\`egrep -e '^ROOT_BASE' ../boot/include/load.inc | sed -e 's/.*0x//g'\`+\`egrep -e '#define[[:space:]]*INSTALL_START_SECT' ../include/sys/config.h | sed -e 's/.*0x//g'\`)*200" | bc` bs=1 count=`ls -l inst.tar | awk -F " " '{print $$5}'` conv=notrunc
	@echo -e '\033[0m'

all : realclean everything

final : all clean

clean :
	rm -f *.o

realclean :
	rm -f $(BIN) *.o

kernel.bin :
	cp ../kernel.bin ./

start.o : start.asm
	@echo -n -e '\033[1;32m'
	$(ASM) $(ASMFLAGS) -o $@ $<
	@echo -n -e '\033[0m'

ls.o: ls.c ../include/type.h ../include/stdio.h ../include/string.h
	@echo -n -e '\033[32m'
	$(CC) $(CFLAGS) -o $@ $<
	@echo -n -e '\033[0m'

ls : ls.o start.o $(LIB)
	@echo -n -e '\033[36m'
	$(LD) $(LDFLAGS) -o $@ $?
	@echo -n -e '\033[0m'

touch.o: touch.c ../include/type.h ../include/stdio.h
	@echo -n -e '\033[32m'
	$(CC) $(CFLAGS) -o $@ $<
	@echo -n -e '\033[0m'

touch : touch.o start.o $(LIB)
	@echo -n -e '\033[36m'
	$(LD) $(LDFLAGS) -o $@ $?
	@echo -n -e '\033[0m'

echo.o: echo.c ../include/type.h ../include/stdio.h
	@echo -n -e '\033[32m'
	$(CC) $(CFLAGS) -o $@ $<
	@echo -n -e '\033[0m'

echo : echo.o start.o $(LIB)
	@echo -n -e '\033[36m'
	$(LD) $(LDFLAGS) -o $@ $?
	@echo -n -e '\033[0m'

pwd.o: pwd.c ../include/type.h ../include/stdio.h
	@echo -n -e '\033[32m'
	$(CC) $(CFLAGS) -o $@ $<
	@echo -n -e '\033[0m'

pwd : pwd.o start.o $(LIB)
	@echo -n -e '\033[36m'
	$(LD) $(LDFLAGS) -o $@ $?
	@echo -n -e '\033[0m'

cat.o: cat.c ../include/stdio.h
	@echo -n -e '\033[32m'
	$(CC) $(CFLAGS) -o $@ $<
	@echo -n -e '\033[0m'

cat : cat.o start.o $(LIB)
	@echo -n -e '\033[36m'
	$(LD) $(LDFLAGS) -o $@ $?
	@echo -n -e '\033[0m'

test01.o: test01.c ../include/type.h ../include/stdio.h
	@echo -n -e '\033[32m'
	$(CC) $(CFLAGS) -o $@ $<
	@echo -n -e '\033[0m'

test01 : test01.o start.o $(LIB)
	@echo -n -e '\033[36m'
	$(LD) $(LDFLAGS) -o $@ $?
	@echo -n -e '\033[0m'

