ASM = nasm
CC = x86_64-elf-gcc
LD = x86_64-elf-ld

ASFLAGS = -f elf64
CFLAGS = -ffreestanding -mno-red-zone -fno-asynchronous-unwind-tables -fno-unwind-tables -Iinclude
LDFLAGS = -T linker.ld

BUILD = build

C_SOURCES := $(shell find kernel -name '*.c')

BOOT_ASM_SOURCES := $(shell find boot -name '*.asm')
KERNEL_ASM_SOURCES := $(shell find kernel -name '*.asm')

BOOT_ASM_OBJECTS := $(patsubst boot/%.asm,$(BUILD)/asm/%.o,$(BOOT_ASM_SOURCES))
KERNEL_ASM_OBJECTS := $(patsubst kernel/%.asm,$(BUILD)/asm/%.o,$(KERNEL_ASM_SOURCES))
ASM_OBJECTS := $(BOOT_ASM_OBJECTS) $(KERNEL_ASM_OBJECTS)

C_OBJECTS := $(patsubst kernel/%.c,$(BUILD)/%.o,$(C_SOURCES))

.PHONY: all clean
all: $(BUILD)/kernel.elf

$(BUILD)/%.o: kernel/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/asm/%.o: kernel/%.asm
	mkdir -p $(dir $@)
	$(ASM) $(ASFLAGS) $< -o $@

$(BUILD)/asm/%.o: boot/%.asm
	mkdir -p $(dir $@)
	$(ASM) $(ASFLAGS) $< -o $@

$(BUILD)/kernel.elf: $(ASM_OBJECTS) $(C_OBJECTS)
	$(LD) $(LDFLAGS) $^ -o $@

clean:
	rm -rf $(BUILD)