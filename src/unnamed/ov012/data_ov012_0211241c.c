#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 f000;
    void *f004;
    u32 f008;
} data_ov012_0211241c_Layout;
typedef char data_ov012_0211241c_size[sizeof(data_ov012_0211241c_Layout) == 0xc ? 1 : -1];

typedef struct {
    u32 f000;
    void *f004;
    u32 f008;
} data_ov012_02112428_Layout;
typedef char data_ov012_02112428_size[sizeof(data_ov012_02112428_Layout) == 0xc ? 1 : -1];

extern data_ov012_0211241c_Layout data_ov012_0211241c;
extern data_ov012_02112428_Layout data_ov012_02112428;
extern u8 _ZN14daObjC0Water_c8BehaviorEv[];
extern u8 _ZN14daObjC0Water_c6RenderEv[];

#pragma define_section sName ".data.c0Rest"                                                        \
                             ".data.c0Rest"                                                        \
                             ".data.c0Rest" far_data
#pragma section sName begin

data_ov012_0211241c_Layout data_ov012_0211241c = {
    0x02011214,
    &_ZN14daObjC0Water_c8BehaviorEv[0],
    0x02010fd4,
};

data_ov012_02112428_Layout data_ov012_02112428 = {
    0x02010fc8,
    &_ZN14daObjC0Water_c6RenderEv[0],
    0x02010f78,
};
