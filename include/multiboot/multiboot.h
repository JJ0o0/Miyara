#ifndef MULTIBOOT_H
#define MULTIBOOT_H

#include <types/types.h>

#define MULTIBOOT_TAG_TYPE_FRAMEBUFFER 8

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
     * (see MemoryMapTag).
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

typedef struct {
    u32 type;
    u32 size;

    u64 framebuffer_addr;
    u32 framebuffer_pitch;
    u32 framebuffer_width;
    u32 framebuffer_height;

    u8 framebuffer_bpp;
    u8 framebuffer_type;
    u16 reserved;
} FramebufferTag;

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

#endif