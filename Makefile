# Makefile for tas demo
#
# basic seria/uart driver for the STM32F411.

TOOLS = arm-none-eabi

# Assembling with gcc makes it want crt0 at link time.
#AS = $(TOOLS)-gcc
AS = $(TOOLS)-as

# Use the -g flag if you intend to use gdb
#CC = $(TOOLS)-gcc -mcpu=cortex-m4 -mthumb
#CC = $(TOOLS)-gcc -mcpu=cortex-m4 -mthumb -g
#CC = $(TOOLS)-gcc -mcpu=cortex-m4 -mthumb -Os


CC = $(TOOLS)-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 \
 -Wno-implicit-function-declaration -fno-builtin \
-Irtt/Config -Irtt/SEGGER

#LD = $(TOOLS)-gcc
LD = $(TOOLS)-ld.bfd
OBJCOPY = $(TOOLS)-objcopy
DUMP = $(TOOLS)-objdump -d
GDB = $(TOOLS)-gdb

RTT = rtt/SEGGER_SYSVIEW_Config_NoOS.o rtt/SEGGER/SEGGER_RTT.o rtt/SEGGER/SEGGER_SYSVIEW.o rtt/SEGGER/SEGGER_RTT_ASM_ARMv7M.o
OBJS = locore.o init.o main.o rcc.o gpio.o serial.o string.o nvic.o printf.o tim.o $(RTT)

all: tas.elf tas.dump tas.bin

tas.dump: tas.elf
	$(DUMP) tas.elf > tas.dump

tas.elf: tas.inc $(OBJS) f411.lds
	$(LD) -T f411.lds -o tas.elf $(OBJS) --print-memory-usage

tas.bin: tas.elf
	$(OBJCOPY) tas.elf tas.bin -O binary

rtt/SEGGER/SEGGER_RTT_ASM_ARMv7M.o: rtt/SEGGER/SEGGER_RTT_ASM_ARMv7M.S
	$(CC) -c rtt/SEGGER/SEGGER_RTT_ASM_ARMv7M.S -o rtt/SEGGER/SEGGER_RTT_ASM_ARMv7M.o

locore.o:	locore.s
	$(AS) locore.s -o locore.o
.c.o:
	$(CC) -o $@ -c $<

OCDCFG = -f openocd.cfg

flash:  tas.elf
	openocd $(OCDCFG) -c "program tas.elf verify reset exit"

ocd:
	openocd $(OCDCFG)

gdb:
	$(GDB) --eval-command="target remote localhost:3333" tas.elf

gdbtui:
	$(GDB) -tui --eval-command="target remote localhost:3333" tas.elf

clean:
	rm -f $(OBJS) tas.elf tas.dump tas.bin fm2ToC tas.inc

fm2ToC: fm2ToC.c
	gcc fm2ToC.c -o fm2ToC

tas.inc: fm2ToC
	./fm2ToC
