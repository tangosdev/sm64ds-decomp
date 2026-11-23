#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[3];
} sMascotWalkerOffset_Layout;
typedef char sMascotWalkerOffset_size[sizeof(sMascotWalkerOffset_Layout) == 0xc ? 1 : -1];

typedef struct {
    u32 w[4];
} sDealerLampTimes_Layout;
typedef char sDealerLampTimes_size[sizeof(sDealerLampTimes_Layout) == 0x10 ? 1 : -1];

extern sMascotWalkerOffset_Layout sMascotWalkerOffset;
extern sDealerLampTimes_Layout sDealerLampTimes;

#pragma define_section sName ".rodata.before_coin_tables"                                          \
                             ".rodata.before_coin_tables"                                          \
                             ".rodata.before_coin_tables" far_data
#pragma section sName begin

sMascotWalkerOffset_Layout sMascotWalkerOffset = {{
    0x00000000,
    0x00008000,
    0x00100000,
}};

sDealerLampTimes_Layout sDealerLampTimes = {{
    0x001e0078,
    0x0078001e,
    0x001e001e,
    0x007800b4,
}};
