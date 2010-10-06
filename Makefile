##########################################################################
# User configuration and firmware specific object files	
##########################################################################

# The target, flash and ram of the LPC1xxx microprocessor.
# Use for the target the value: LPC11xx, LPC13xx or LPC17xx
TARGET = LPC13xx
FLASH = 32K
SRAM = 8K

# For USB support the LPC1xxx reserves 384 bytes from the sram,
# if you don't want to use the USB features, just use 0 here.
SRAM_USB = 384

VPATH = 
OBJS = main.o
OBJS_DIR = build

##########################################################################
# brunql: Flash Magic config
# make fm
FM_CONSOLE = wine "/home/brunql/.wine/drive_c/Program Files/Flash Magic/FM.exe"
FM_COM_NUM = 2
FM_COM_BAUD_RATE = 115200
FM_DEVICE = LPC1343
FM_FREQ_Mhz = 12.000

##########################################################################
# Library files 
##########################################################################
VPATH += core core/cpu core/gpio core/uart core/adc core/usbhid-rom
OBJS += cpu.o gpio.o uart.o uart_buf.o adc.o usbhid.o usbconfig.o

##########################################################################
# GNU GCC compiler prefix and location
##########################################################################
PATH_TO_GCC = /home/brunql/Arm-tools-build-dir/arm-cs-tools/bin
CROSS_COMPILE = $(PATH_TO_GCC)/arm-none-eabi-
AS = $(CROSS_COMPILE)gcc
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)gcc
SIZE = $(CROSS_COMPILE)size
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump
OUTFILE = firmware

##########################################################################
# Startup files
##########################################################################

LD_PATH = lpc1xxx
LD_SCRIPT = $(LD_PATH)/linkscript.ld
LD_TEMP = $(LD_PATH)/memory.ld

ifeq (LPC11xx,$(TARGET))
  CORTEX_TYPE=m0
else
  CORTEX_TYPE=m3
endif

CPU_TYPE = cortex-$(CORTEX_TYPE)
VPATH += lpc1xxx
OBJS += $(TARGET)_handlers.o LPC1xxx_startup.o

OUT_OBJS := $(addprefix $(OBJS_DIR)/,$(OBJS))

##########################################################################
# Compiler settings, parameters and flags
##########################################################################
CFLAGS  = -std=c99 -c -Os -I. -Wall -mthumb -ffunction-sections -fdata-sections -fmessage-length=0 -mcpu=$(CPU_TYPE) -DTARGET=$(TARGET) -fno-builtin
ASFLAGS = -c -Os -I. -Wall -mthumb -ffunction-sections -fdata-sections -fmessage-length=0 -mcpu=$(CPU_TYPE) -D__ASSEMBLY__ -x assembler-with-cpp
LDFLAGS = -nostartfiles -mthumb -Wl,--gc-sections
OCFLAGS = --strip-unneeded

all: firmware program_flash

%.o : %.c
	$(CC) $(CFLAGS) -o $(OBJS_DIR)/$@ $<

%.o : %.s
	$(AS) $(ASFLAGS) -o $(OBJS_DIR)/$@ $<

firmware: $(OBJS) $(SYS_OBJS)
	-@echo "MEMORY" > $(LD_TEMP)
	-@echo "{" >> $(LD_TEMP)
	-@echo "  flash(rx): ORIGIN = 0x00000000, LENGTH = $(FLASH)" >> $(LD_TEMP)
	-@echo "  sram(rwx): ORIGIN = 0x10000000+$(SRAM_USB), LENGTH = $(SRAM)-$(SRAM_USB)" >> $(LD_TEMP)
	-@echo "}" >> $(LD_TEMP)
	-@echo "INCLUDE $(LD_SCRIPT)" >> $(LD_TEMP)
	$(LD) $(LDFLAGS) -T $(LD_TEMP) -o $(OBJS_DIR)/$(OUTFILE).elf $(OUT_OBJS)
	-@echo ""
	$(SIZE) $(OBJS_DIR)/$(OUTFILE).elf
	-@echo ""
	$(OBJCOPY) $(OCFLAGS) -O binary $(OBJS_DIR)/$(OUTFILE).elf $(OBJS_DIR)/$(OUTFILE).bin
	$(OBJCOPY) $(OCFLAGS) -O ihex $(OBJS_DIR)/$(OUTFILE).elf $(OUTFILE).hex

clean:
	rm -f $(OBJS_DIR)/*.o $(LD_TEMP) $(OBJS_DIR)/$(OUTFILE).elf $(OBJS_DIR)/$(OUTFILE).bin $(OUTFILE).hex

program_flash: fm

fm: firmware
	$(FM_CONSOLE) "COM($(FM_COM_NUM), $(FM_COM_BAUD_RATE)) DEVICE($(FM_DEVICE), $(FM_FREQ_Mhz)) ERASE(DEVICE, PROTECTISP) HEXFILE($(OUTFILE).hex, CHECKSUMS, NOFILL, PROTECTISP)"

#usb_flash: firmware
#	rm "/media/CRP DISABLD/firmware.bin"
#	cp firmware.bin "/media/CRP DISABLD/"

# check connection of LPC1343 as usb_storage
d: 
	dmesg | tail 


# to be continued...
