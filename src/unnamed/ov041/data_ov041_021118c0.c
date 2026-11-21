#include "types.h"

extern u32 data_ov041_02111a58[];
extern u32 data_ov041_02111b88[];

u32 data_ov041_021118c0[4] = {
    0xfe3e0000,
    0xc0001068,
    0x09010000,
    0x00006cff,
};

u32 data_ov041_021118d0[4] = {
    0x00001300,
    (u32)data_ov041_02111a58,
    0x00003405,
    (u32)data_ov041_02111b88,
};
