#ifndef PRIVATE_ROM_HEADER_VIEW_H
#define PRIVATE_ROM_HEADER_VIEW_H

/* The DS cartridge header (NitroSDK RomHeader, 0x160 bytes) as the multiboot
 * code reads it: each static module's entry point and load region. Only
 * offsets the ROM's code reads are named; everything else stays padding.
 * Plain C types: the consumers are C and C++ files that do not all include
 * types.h. */
typedef struct RomStaticRegion {
    unsigned int ram_address; /* 0x00 */
    unsigned int size;        /* 0x04 */
} RomStaticRegion;

typedef struct RomHeaderView {
    unsigned char pad_00[0x24];
    unsigned int main_entry_address; /* 0x24 -- ARM9 static */
    RomStaticRegion main;            /* 0x28 */
    unsigned char pad_30[0x34 - 0x30];
    unsigned int sub_entry_address; /* 0x34 -- ARM7 static */
    RomStaticRegion sub;            /* 0x38 */
    unsigned char pad_40[0x160 - 0x40];
} RomHeaderView;

typedef char RomHeaderView_size_must_be_0x160[sizeof(RomHeaderView) == 0x160 ? 1 : -1];

#endif
