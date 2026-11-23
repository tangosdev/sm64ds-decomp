#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    void *f000;
    void *f004;
    void *f008;
} sMemory2CardPositionsPerDifficulty_Layout;
typedef char sMemory2CardPositionsPerDifficulty_size
    [sizeof(sMemory2CardPositionsPerDifficulty_Layout) == 0xc ? 1 : -1];

typedef struct {
    u32 w[3];
} sMemory2FlipFrameTimes_Layout;
typedef char sMemory2FlipFrameTimes_size[sizeof(sMemory2FlipFrameTimes_Layout) == 0xc ? 1 : -1];

extern sMemory2CardPositionsPerDifficulty_Layout sMemory2CardPositionsPerDifficulty;
extern sMemory2FlipFrameTimes_Layout sMemory2FlipFrameTimes;
extern u8 data_ov006_0213d384[];
extern u8 data_ov006_0213d3c4[];
extern u8 data_ov006_0213d40c[];

#pragma define_section sName ".data.gap58"                                                         \
                             ".data.gap58"                                                         \
                             ".data.gap58" far_data
#pragma section sName begin

sMemory2CardPositionsPerDifficulty_Layout sMemory2CardPositionsPerDifficulty = {
    &data_ov006_0213d384[0],
    &data_ov006_0213d3c4[0],
    &data_ov006_0213d40c[0],
};

sMemory2FlipFrameTimes_Layout sMemory2FlipFrameTimes = {{
    0x00060008,
    0x00060004,
    0x00180018,
}};
