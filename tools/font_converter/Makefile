CC = gcc

ROOT := ../..
BUILD := $(ROOT)/build
TARGET := $(BUILD)/tools/font_converter

OUTPUT_DIR := $(ROOT)/kernel/graphics/fonts/generated

.PHONY: all clean convert

all: $(TARGET)

$(TARGET): font_converter.c
	mkdir -p $(dir $@)
	$(CC) $< -o $@

convert: $(TARGET)
ifndef INPUT
	$(error INPUT is required. Example: INPUT=assets/fonts/ter-u16n.psf)
endif
ifndef NAME
	$(error NAME is required. Example: NAME=terminus_8x16)
endif
	mkdir -p $(OUTPUT_DIR)
	$(TARGET) \
		$(ROOT)/$(INPUT) \
		$(OUTPUT_DIR)/$(NAME).c \
		$(NAME)

clean:
	rm -f $(TARGET)