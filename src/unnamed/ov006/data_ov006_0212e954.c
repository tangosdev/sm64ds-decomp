#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[10];
} sMCarlo2StartFaceCounts_Layout;
typedef char sMCarlo2StartFaceCounts_size[sizeof(sMCarlo2StartFaceCounts_Layout) == 0x28 ? 1 : -1];

typedef struct {
    u32 w[100];
} sMCarlo2BoardRows_Layout;
typedef char sMCarlo2BoardRows_size[sizeof(sMCarlo2BoardRows_Layout) == 0x190 ? 1 : -1];

extern sMCarlo2StartFaceCounts_Layout sMCarlo2StartFaceCounts;
extern sMCarlo2BoardRows_Layout sMCarlo2BoardRows;

#pragma define_section sName ".rodata.gap3"                                                        \
                             ".rodata.gap3"                                                        \
                             ".rodata.gap3" far_data
#pragma section sName begin

sMCarlo2StartFaceCounts_Layout sMCarlo2StartFaceCounts = {{
    0x00000008,
    0x00000008,
    0x00000008,
    0x00000008,
    0x00000008,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
}};

sMCarlo2BoardRows_Layout sMCarlo2BoardRows = {{
    0x00000004, 0x00000004, 0x00000004, 0x00000004, 0x00000004, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000002, 0x00000002, 0x00000004, 0x00000004, 0x00000004, 0x00000004,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000002, 0x00000002, 0x00000002, 0x00000002,
    0x00000004, 0x00000004, 0x00000004, 0x00000000, 0x00000000, 0x00000000, 0x00000002, 0x00000002,
    0x00000002, 0x00000002, 0x00000002, 0x00000002, 0x00000004, 0x00000004, 0x00000000, 0x00000000,
    0x00000000, 0x00000004, 0x00000002, 0x00000002, 0x00000002, 0x00000002, 0x00000002, 0x00000002,
    0x00000004, 0x00000000, 0x00000002, 0x00000000, 0x00000004, 0x00000002, 0x00000002, 0x00000002,
    0x00000002, 0x00000002, 0x00000002, 0x00000002, 0x00000002, 0x00000002, 0x00000000, 0x00000004,
    0x00000002, 0x00000002, 0x00000002, 0x00000002, 0x00000002, 0x00000002, 0x00000002, 0x00000002,
    0x00000002, 0x00000000, 0x00000004, 0x00000002, 0x00000002, 0x00000002, 0x00000002, 0x00000002,
    0x00000002, 0x00000002, 0x00000002, 0x00000002, 0x00000000, 0x00000004, 0x00000002, 0x00000002,
    0x00000002, 0x00000002, 0x00000002, 0x00000002, 0x00000002, 0x00000002, 0x00000002, 0x00000002,
    0x00000002, 0x00000002, 0x00000002, 0x00000002,
}};

#pragma section sName end
