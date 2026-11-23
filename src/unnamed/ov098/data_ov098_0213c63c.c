#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[2];
} data_ov098_0213c63c_Layout;
typedef char data_ov098_0213c63c_size[sizeof(data_ov098_0213c63c_Layout) == 0x8 ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
} data_ov098_0213c644_Layout;
typedef char data_ov098_0213c644_size[sizeof(data_ov098_0213c644_Layout) == 0x8 ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
} data_ov098_0213c64c_Layout;
typedef char data_ov098_0213c64c_size[sizeof(data_ov098_0213c64c_Layout) == 0x8 ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
} data_ov098_0213c654_Layout;
typedef char data_ov098_0213c654_size[sizeof(data_ov098_0213c654_Layout) == 0x8 ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
} data_ov098_0213c65c_Layout;
typedef char data_ov098_0213c65c_size[sizeof(data_ov098_0213c65c_Layout) == 0x8 ? 1 : -1];

extern data_ov098_0213c63c_Layout data_ov098_0213c63c;
extern data_ov098_0213c644_Layout data_ov098_0213c644;
extern data_ov098_0213c64c_Layout data_ov098_0213c64c;
extern data_ov098_0213c654_Layout data_ov098_0213c654;
extern data_ov098_0213c65c_Layout data_ov098_0213c65c;
extern u8 _ZN7daCnn_c9StateLoadEv[];
extern u8 _ZN7daCnn_c8StateAimEv[];
extern u8 _ZN7daCnn_c17StateWaitForRiderEv[];
extern u8 _ZN7daCnn_c9StateFireEv[];

#pragma define_section sName ".data.blockTail"                                                     \
                             ".data.blockTail"                                                     \
                             ".data.blockTail" far_data
#pragma section sName begin

data_ov098_0213c654_Layout data_ov098_0213c654 = {
    &_ZN7daCnn_c17StateWaitForRiderEv[0],
    0x00000000,
};

data_ov098_0213c64c_Layout data_ov098_0213c64c = {
    &_ZN7daCnn_c8StateAimEv[0],
    0x00000000,
};

data_ov098_0213c644_Layout data_ov098_0213c644 = {
    &_ZN7daCnn_c9StateLoadEv[0],
    0x00000000,
};

data_ov098_0213c63c_Layout data_ov098_0213c63c = {{
    0x10000000,
    0x00000000,
}};

data_ov098_0213c65c_Layout data_ov098_0213c65c = {
    &_ZN7daCnn_c9StateFireEv[0],
    0x00000000,
};
