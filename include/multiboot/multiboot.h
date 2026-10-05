#ifndef MULTIBOOT_H
#define MULTIBOOT_H

#include <types/types.h>

/**
 * Tag type of the framebuffer tag (see FramebufferTag).
 */
#define MULTIBOOT_TAG_TYPE_FRAMEBUFFER 8

/**
 * Value of FramebufferTag.framebuffer_type for a direct RGB
 * framebuffer, where the color of each pixel is described by
 * FramebufferRGBInfo. Multiboot2 also defines 0 (indexed color) and 2
 * (EGA text), which this kernel does not support.
 */
#define MULTIBOOT_FRAMEBUFFER_TYPE_RGB 1

/**
 * Common header shared by every Multiboot2 info tag.
 *
 * The MBI is a sequence of tags starting right after MBIHeader; each
 * tag's `size` (including this header) is used to find the next one,
 * padded up to the next 8-byte boundary (see pmm_init).
 */
typedef struct {
    /**
     * Tag type. 0 marks the end of the tag list; 6 is the memory map
     * (see MemoryMapTag); 8 is the framebuffer (see FramebufferTag).
     */
    u32 type;

    /**
     * Size of this tag in bytes, including this header.
     */
    u32 size;
} TagHeader;

/**
 * Fixed header at the very start of the Multiboot2 info structure
 * passed to the kernel in RBX.
 *
 * The first tag (a TagHeader) starts 8 bytes after this structure.
 */
typedef struct {
    /**
     * Total size of the whole MBI (this header plus all tags), in
     * bytes.
     */
    u32 total_size;

    /**
     * Reserved. Always 0.
     */
    u32 reserved;
} MBIHeader;

/**
 * Header of the memory map tag (TagHeader.type == 6).
 *
 * Followed immediately by (size - sizeof(MemoryMapTag)) bytes of
 * MemoryMapEntry structs, each entry_size bytes long (not necessarily
 * sizeof(MemoryMapEntry) — read entries by adding entry_size, not by
 * indexing the struct array directly, in case future firmware adds
 * trailing fields to each entry).
 */
typedef struct {
    /**
     * Tag type. Always 6 for a memory map tag.
     */
    u32 type;

    /**
     * Total size of this tag, including the header and every entry.
     */
    u32 size;

    /**
     * Size of a single MemoryMapEntry, in bytes.
     */
    u32 entry_size;

    /**
     * Version of the entry format. Currently always 0.
     */
    u32 entry_version;
} MemoryMapTag;

/**
 * Header of the framebuffer tag (TagHeader.type ==
 * MULTIBOOT_TAG_TYPE_FRAMEBUFFER).
 *
 * Followed immediately by color information whose layout depends on
 * framebuffer_type. For MULTIBOOT_FRAMEBUFFER_TYPE_RGB it is a
 * FramebufferRGBInfo, right after sizeof(FramebufferTag) bytes.
 */
typedef struct {
    /**
     * Tag type. Always MULTIBOOT_TAG_TYPE_FRAMEBUFFER.
     */
    u32 type;

    /**
     * Total size of this tag, including the header and the color
     * information that follows it.
     */
    u32 size;

    /**
     * Physical address of the first pixel.
     */
    u64 framebuffer_addr;

    /**
     * Size of one scanline, in bytes. May be larger than the width
     * times the bytes per pixel.
     */
    u32 framebuffer_pitch;

    /**
     * Width of the framebuffer, in pixels.
     */
    u32 framebuffer_width;

    /**
     * Height of the framebuffer, in pixels.
     */
    u32 framebuffer_height;

    /**
     * Bits per pixel.
     */
    u8 framebuffer_bpp;

    /**
     * Color model: 0 = indexed color, 1 = direct RGB
     * (MULTIBOOT_FRAMEBUFFER_TYPE_RGB), 2 = EGA text.
     */
    u8 framebuffer_type;

    /**
     * Reserved. Always 0.
     */
    u16 reserved;
} FramebufferTag;

/**
 * Color information of a direct RGB framebuffer, found right after
 * the FramebufferTag when framebuffer_type is
 * MULTIBOOT_FRAMEBUFFER_TYPE_RGB.
 *
 * For each channel, the position is the index of its lowest bit
 * inside a pixel, and the mask size is how many bits wide it is.
 */
typedef struct {
    /**
     * Bit position of the red channel inside a pixel.
     */
    u8 red_field_position;

    /**
     * Width of the red channel, in bits.
     */
    u8 red_mask_size;

    /**
     * Bit position of the green channel inside a pixel.
     */
    u8 green_field_position;

    /**
     * Width of the green channel, in bits.
     */
    u8 green_mask_size;

    /**
     * Bit position of the blue channel inside a pixel.
     */
    u8 blue_field_position;

    /**
     * Width of the blue channel, in bits.
     */
    u8 blue_mask_size;
} FramebufferRGBInfo;

/**
 * A single memory map entry, describing one physical address range.
 */
typedef struct {
    /**
     * Physical start address of the region.
     */
    u64 addr;

    /**
     * Length of the region, in bytes.
     */
    u64 len;

    /**
     * Region type. 1 = available RAM; any other value is reserved,
     * ACPI, NVS, or otherwise unusable and must not be handed out by
     * the physical memory allocator.
     */
    u32 type;

    /**
     * Reserved. Always 0.
     */
    u32 reserved;
} MemoryMapEntry;

_Static_assert(sizeof(FramebufferTag) == 32, "FramebufferTag needs 32 bytes.");
_Static_assert(sizeof(FramebufferRGBInfo) == 6, "FramebufferRGBInfo needs 6 bytes.");

#endif