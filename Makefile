ASM = nasm
CC = x86_64-elf-gcc
LD = x86_64-elf-ld

ASFLAGS = -f elf64
CFLAGS = -ffreestanding -mno-red-zone -fno-asynchronous-unwind-tables -fno-unwind-tables
LDFLAGS = -T linker.ld

BUILD = build

C_SOURCES := $(wildcard kernel/*.c)
ASM_SOURCES := $(wildcard boot/*.asm)

C_OBJECTS := $(patsubst kernel/%.c,$(BUILD)/%.o,$(C_SOURCES))
ASM_OBJECTS := $(patsubst boot/%.asm,$(BUILD)/%.o,$(ASM_SOURCES))

.PHONY: all clean
all: $(BUILD)/kernel.elf

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/%.o: kernel/%.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/%.o: boot/%.asm | $(BUILD)
	$(ASM) $(ASFLAGS) $< -o $@

$(BUILD)/kernel.elf: $(ASM_OBJECTS) $(C_OBJECTS)
	$(LD) $(LDFLAGS) $^ -o $@

clean:
	rm -rf $(BUILD)