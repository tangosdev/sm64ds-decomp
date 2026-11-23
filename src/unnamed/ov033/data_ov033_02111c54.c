#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[5];
} data_ov033_02111c54_Layout;
typedef char data_ov033_02111c54_size[sizeof(data_ov033_02111c54_Layout) == 0x14 ? 1 : -1];

typedef struct {
    u32 f000;
    void *f004;
    u32 f008;
    void *f00c;
    u32 f010;
    void *f014;
    u32 f018;
    void *f01c;
    u32 f020;
    void *f024;
    u32 f028;
    void *f02c;
    u32 f030;
    void *f034;
    u32 f038;
    void *f03c;
    u32 f040;
    void *f044;
    u32 f048;
    void *f04c;
} data_ov033_02111c68_Layout;
typedef char data_ov033_02111c68_size[sizeof(data_ov033_02111c68_Layout) == 0x50 ? 1 : -1];

extern data_ov033_02111c54_Layout data_ov033_02111c54;
extern data_ov033_02111c68_Layout data_ov033_02111c68;
extern u8 data_ov033_02111f9c[];
extern u8 data_ov033_02111ee4[];
extern u8 data_ov033_02111b8c[];
extern u8 data_ov033_02111b2c[];
extern u8 data_ov033_02111b4c[];
extern u8 data_ov033_02111b3c[];
extern u8 data_ov033_02111b0c[];
extern u8 data_ov033_02111b5c[];
extern u8 data_ov033_02111b6c[];
extern u8 data_ov033_02111b7c[];

#pragma define_section sName ".data.gap1"                                                          \
                             ".data.gap1"                                                          \
                             ".data.gap1" far_data
#pragma section sName begin

data_ov033_02111c54_Layout data_ov033_02111c54 = {{
    0x00ff0618,
    0xfe70076c,
    0xc000f98e,
    0x07180000,
    0x000000ff,
}};

data_ov033_02111c68_Layout data_ov033_02111c68 = {
    0x00003a00, &data_ov033_02111f9c[0], 0x00001705, &data_ov033_02111ee4[0],
    0x00000120, &data_ov033_02111b8c[0], 0x00000140, &data_ov033_02111b2c[0],
    0x00000160, &data_ov033_02111b4c[0], 0x00000180, &data_ov033_02111b3c[0],
    0x00000185, &data_ov033_02111b0c[0], 0x000001a0, &data_ov033_02111b5c[0],
    0x000001c0, &data_ov033_02111b6c[0], 0x000001e0, &data_ov033_02111b7c[0],
};
