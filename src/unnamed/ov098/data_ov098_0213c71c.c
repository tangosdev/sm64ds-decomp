#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    void *f000;
    u32 f004;
} data_ov098_0213c71c_Layout;
typedef char data_ov098_0213c71c_size[sizeof(data_ov098_0213c71c_Layout) == 0x8 ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
} data_ov098_0213c724_Layout;
typedef char data_ov098_0213c724_size[sizeof(data_ov098_0213c724_Layout) == 0x8 ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
} data_ov098_0213c72c_Layout;
typedef char data_ov098_0213c72c_size[sizeof(data_ov098_0213c72c_Layout) == 0x8 ? 1 : -1];

extern data_ov098_0213c71c_Layout data_ov098_0213c71c;
extern data_ov098_0213c724_Layout data_ov098_0213c724;
extern data_ov098_0213c72c_Layout data_ov098_0213c72c;
extern u8 _ZN7daWbm_c11StateBounceEv[];
extern u8 _ZN7daWbm_c9StateDropEv[];
extern u8 _ZN7daWbm_c9StateFallEv[];

#pragma define_section sName ".data.blockTail"                                                     \
                             ".data.blockTail"                                                     \
                             ".data.blockTail" far_data
#pragma section sName begin

data_ov098_0213c724_Layout data_ov098_0213c724 = {
    &_ZN7daWbm_c9StateDropEv[0],
    0x00000000,
};

data_ov098_0213c71c_Layout data_ov098_0213c71c = {
    &_ZN7daWbm_c11StateBounceEv[0],
    0x00000000,
};

data_ov098_0213c72c_Layout data_ov098_0213c72c = {
    &_ZN7daWbm_c9StateFallEv[0],
    0x00000000,
};
