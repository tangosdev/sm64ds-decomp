#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[1];
} data_ov031_02111548_Layout;
typedef char data_ov031_02111548_size[sizeof(data_ov031_02111548_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 f000;
    void *f004;
    u32 f008;
    void *f00c;
} data_ov031_0211154c_Layout;
typedef char data_ov031_0211154c_size[sizeof(data_ov031_0211154c_Layout) == 0x10 ? 1 : -1];

extern data_ov031_02111548_Layout data_ov031_02111548;
extern data_ov031_0211154c_Layout data_ov031_0211154c;
extern u8 data_ov031_02111668[];
extern u8 data_ov031_02111758[];

data_ov031_02111548_Layout data_ov031_02111548 = {{
    0x000011ff,
}};

data_ov031_0211154c_Layout data_ov031_0211154c = {
    0x00000f00,
    &data_ov031_02111668[0],
    0x00003205,
    &data_ov031_02111758[0],
};
