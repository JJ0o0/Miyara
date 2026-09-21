ASM = nasm
CC = x86_64-elf-gcc
LD = x86_64-elf-ld

ASFLAGS = -f elf64
CFLAGS = -ffreestanding -mno-red-zone -fno-asynchronous-unwind-tables -fno-unwind-tables -Iinclude
LDFLAGS = -T linker.ld

BUILD = build

C_SOURCES := $(shell find kernel -name '*.c')
ASM_SOURCES := $(shell find boot -name '*.asm')

C_OBJECTS := $(patsubst kernel/%.c,$(BUILD)/%.o,$(C_SOURCES))
ASM_OBJECTS := $(patsubst boot/%.asm,$(BUILD)/%.o,$(ASM_SOURCES))

.PHONY: all clean
all: $(BUILD)/kernel.elf

$(BUILD)/%.o: kernel/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/%.o: boot/%.asm
	mkdir -p $(dir $@)
	$(ASM) $(ASFLAGS) $< -o $@

$(BUILD)/kernel.elf: $(ASM_OBJECTS) $(C_OBJECTS)
	$(LD) $(LDFLAGS) $^ -o $@

clean:
	rm -rf $(BUILD)