#include "types.h"

/* Same-sized objects are reversed except the last, which is the order mwcc emits. */

extern u32 data_ov008_02111380[];
extern u32 data_ov008_02111450[];

u32 data_ov008_021111d0[2] = {
    0x00000008,
    (u32)data_ov008_02111380,
};

u32 data_ov008_021111c8[2] = {
    0x092ee225,
    0x05780000,
};

u32 data_ov008_021111d8[2] = {
    0x0000000b,
    (u32)data_ov008_02111450,
};
