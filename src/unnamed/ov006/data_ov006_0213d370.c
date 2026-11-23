#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[5];
} sMemory2CardNcgByLanguage_Layout;
typedef char
    sMemory2CardNcgByLanguage_size[sizeof(sMemory2CardNcgByLanguage_Layout) == 0x14 ? 1 : -1];

typedef struct {
    u32 w[16];
} data_ov006_0213d384_Layout;
typedef char data_ov006_0213d384_size[sizeof(data_ov006_0213d384_Layout) == 0x40 ? 1 : -1];

typedef struct {
    u32 w[18];
} data_ov006_0213d3c4_Layout;
typedef char data_ov006_0213d3c4_size[sizeof(data_ov006_0213d3c4_Layout) == 0x48 ? 1 : -1];

extern sMemory2CardNcgByLanguage_Layout sMemory2CardNcgByLanguage;
extern data_ov006_0213d384_Layout data_ov006_0213d384;
extern data_ov006_0213d3c4_Layout data_ov006_0213d3c4;

#pragma define_section sName ".data.gap58"                                                         \
                             ".data.gap58"                                                         \
                             ".data.gap58" far_data
#pragma section sName begin

sMemory2CardNcgByLanguage_Layout sMemory2CardNcgByLanguage = {{
    0x000000ba,
    0x000000bc,
    0x000000b7,
    0x000000b8,
    0x000000b9,
}};

data_ov006_0213d384_Layout data_ov006_0213d384 = {{
    0x00a80050,
    0x00a80070,
    0x00a80090,
    0x00a800b0,
    0x00780050,
    0x00780070,
    0x00780090,
    0x007800b0,
    0x00480050,
    0x00480070,
    0x00480090,
    0x004800b0,
    0x00180050,
    0x00180070,
    0x00180090,
    0x001800b0,
}};

data_ov006_0213d3c4_Layout data_ov006_0213d3c4 = {{
    0x00900030,
    0x00900050,
    0x00900070,
    0x00900090,
    0x009000b0,
    0x009000d0,
    0x00600030,
    0x00600050,
    0x00600070,
    0x00600090,
    0x006000b0,
    0x006000d0,
    0x00300030,
    0x00300050,
    0x00300070,
    0x00300090,
    0x003000b0,
    0x003000d0,
}};
