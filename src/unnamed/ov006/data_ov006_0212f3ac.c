#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[1];
} data_ov006_0212f3ac_Layout;
typedef char data_ov006_0212f3ac_size[sizeof(data_ov006_0212f3ac_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[1];
} data_ov006_0212f3b0_Layout;
typedef char data_ov006_0212f3b0_size[sizeof(data_ov006_0212f3b0_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov006_0212f3b4_Layout;
typedef char data_ov006_0212f3b4_size[sizeof(data_ov006_0212f3b4_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[66];
} sSnowballCourseTiles_Layout;
typedef char sSnowballCourseTiles_size[sizeof(sSnowballCourseTiles_Layout) == 0x108 ? 1 : -1];

extern data_ov006_0212f3ac_Layout data_ov006_0212f3ac;
extern data_ov006_0212f3b0_Layout data_ov006_0212f3b0;
extern data_ov006_0212f3b4_Layout data_ov006_0212f3b4;
extern sSnowballCourseTiles_Layout sSnowballCourseTiles;

#pragma define_section sName ".rodata.after_sound"                                                 \
                             ".rodata.after_sound"                                                 \
                             ".rodata.after_sound" far_data
#pragma section sName begin

data_ov006_0212f3ac_Layout data_ov006_0212f3ac = {{
    0x00000201,
}};

data_ov006_0212f3b0_Layout data_ov006_0212f3b0 = {{
    0x00000201,
}};

data_ov006_0212f3b4_Layout data_ov006_0212f3b4 = {{
    0x00000001,
    0x00000002,
}};

sSnowballCourseTiles_Layout sSnowballCourseTiles = {{
    0xf200f200, 0xf200f200, 0xf3c1f3c0, 0xf3e1f3e0, 0xf3c4f3c3, 0xf3e4f3e3, 0xf364f363, 0xf384f383,
    0xf367f366, 0xf387f386, 0xf304f303, 0xf324f323, 0xf307f306, 0xf327f326, 0xf3cdf3cc, 0xf3edf3ec,
    0xf36af369, 0xf38af389, 0xf36df36c, 0xf38df38c, 0xf30af309, 0xf32af329, 0xf30df30c, 0xf32df32c,
    0xf3c7f3c6, 0xf3e7f3e6, 0xf38ff38e, 0xf3aff3ae, 0xf310f30f, 0xf330f32f, 0xf313f312, 0xf333f332,
    0xf3d3f3d2, 0xf3f3f3f2, 0xf3d6f3d5, 0xf3f6f3f5, 0xf376f375, 0xf396f395, 0xf379f378, 0xf399f398,
    0xf316f315, 0xf336f335, 0xf319f318, 0xf339f338, 0xf3dff3de, 0xf3fff3fe, 0xf37cf37b, 0xf39cf39b,
    0xf37ff37e, 0xf39ff39e, 0xf31cf31b, 0xf33cf33b, 0xf31ff31e, 0xf33ff33e, 0xf38ff3d5, 0xf3aff3f5,
    0xf3dff38e, 0xf3fff3ae, 0xf241f240, 0xf261f260, 0xf244f243, 0xf264f263, 0xf2a1f2a0, 0xf2c1f2c0,
    0xf2a4f2a3, 0xf2c4f2c3,
}};

#pragma section sName end
