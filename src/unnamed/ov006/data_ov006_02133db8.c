#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[2];
} data_ov006_02133db8_Layout;
typedef char data_ov006_02133db8_size[sizeof(data_ov006_02133db8_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov006_02133dc0_Layout;
typedef char data_ov006_02133dc0_size[sizeof(data_ov006_02133dc0_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov006_02133dc8_Layout;
typedef char data_ov006_02133dc8_size[sizeof(data_ov006_02133dc8_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov006_02133dd0_Layout;
typedef char data_ov006_02133dd0_size[sizeof(data_ov006_02133dd0_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov006_02133dd8_Layout;
typedef char data_ov006_02133dd8_size[sizeof(data_ov006_02133dd8_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov006_02133de0_Layout;
typedef char data_ov006_02133de0_size[sizeof(data_ov006_02133de0_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov006_02133de8_Layout;
typedef char data_ov006_02133de8_size[sizeof(data_ov006_02133de8_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov006_02133df0_Layout;
typedef char data_ov006_02133df0_size[sizeof(data_ov006_02133df0_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov006_02133df8_Layout;
typedef char data_ov006_02133df8_size[sizeof(data_ov006_02133df8_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov006_02133e00_Layout;
typedef char data_ov006_02133e00_size[sizeof(data_ov006_02133e00_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov006_02133e08_Layout;
typedef char data_ov006_02133e08_size[sizeof(data_ov006_02133e08_Layout) == 0x8 ? 1 : -1];

typedef struct {
    void *f000;
    void *f004;
    void *f008;
} sPachinkoFxSpritesA_Layout;
typedef char sPachinkoFxSpritesA_size[sizeof(sPachinkoFxSpritesA_Layout) == 0xc ? 1 : -1];

typedef struct {
    u32 w[8];
} data_ov006_02133e1c_Layout;
typedef char data_ov006_02133e1c_size[sizeof(data_ov006_02133e1c_Layout) == 0x20 ? 1 : -1];

typedef struct {
    u32 w[12];
} data_ov006_02133e3c_Layout;
typedef char data_ov006_02133e3c_size[sizeof(data_ov006_02133e3c_Layout) == 0x30 ? 1 : -1];

extern data_ov006_02133db8_Layout data_ov006_02133db8;
extern data_ov006_02133dc0_Layout data_ov006_02133dc0;
extern data_ov006_02133dc8_Layout data_ov006_02133dc8;
extern data_ov006_02133dd0_Layout data_ov006_02133dd0;
extern data_ov006_02133dd8_Layout data_ov006_02133dd8;
extern data_ov006_02133de0_Layout data_ov006_02133de0;
extern data_ov006_02133de8_Layout data_ov006_02133de8;
extern data_ov006_02133df0_Layout data_ov006_02133df0;
extern data_ov006_02133df8_Layout data_ov006_02133df8;
extern data_ov006_02133e00_Layout data_ov006_02133e00;
extern data_ov006_02133e08_Layout data_ov006_02133e08;
extern sPachinkoFxSpritesA_Layout sPachinkoFxSpritesA;
extern data_ov006_02133e1c_Layout data_ov006_02133e1c;
extern data_ov006_02133e3c_Layout data_ov006_02133e3c;

#pragma define_section sName ".data.gap4"                                                          \
                             ".data.gap4"                                                          \
                             ".data.gap4" far_data
#pragma section sName begin

data_ov006_02133db8_Layout data_ov006_02133db8 = {{
    0x81f880ec,
    0xffff0000,
}};

data_ov006_02133dc0_Layout data_ov006_02133dc0 = {{
    0x81f880ec,
    0xffff0002,
}};

data_ov006_02133e08_Layout data_ov006_02133e08 = {{
    0x41f800f8,
    0xffff51c4,
}};

data_ov006_02133e00_Layout data_ov006_02133e00 = {{
    0x81f880ec,
    0xffff0084,
}};

data_ov006_02133df8_Layout data_ov006_02133df8 = {{
    0x81f880ec,
    0xffff0086,
}};

data_ov006_02133df0_Layout data_ov006_02133df0 = {{
    0x81f880ec,
    0xffff0004,
}};

data_ov006_02133de8_Layout data_ov006_02133de8 = {{
    0x81f880ec,
    0xffff0006,
}};

data_ov006_02133de0_Layout data_ov006_02133de0 = {{
    0x81f880ec,
    0xffff3084,
}};

data_ov006_02133dd8_Layout data_ov006_02133dd8 = {{
    0x81f880ec,
    0xffff3086,
}};

data_ov006_02133dd0_Layout data_ov006_02133dd0 = {{
    0x81f880ec,
    0xffff0082,
}};

data_ov006_02133dc8_Layout data_ov006_02133dc8 = {{
    0x81f880ec,
    0xffff0080,
}};

sPachinkoFxSpritesA_Layout sPachinkoFxSpritesA = {
    &data_ov006_02133e08,
    &data_ov006_02133e3c,
    &data_ov006_02133e1c,
};

data_ov006_02133e1c_Layout data_ov006_02133e1c = {{
    0x01f600f5,
    0x00005185,
    0x000300f5,
    0x00005186,
    0x00030002,
    0x000051a7,
    0x31f60002,
    0xffff5186,
}};

data_ov006_02133e3c_Layout data_ov006_02133e3c = {{
    0x01f780f6,
    0x00005144,
    0x11f800f6,
    0x00005145,
    0x01fd40f7,
    0x00005146,
    0x000300fd,
    0x00005165,
    0x01fa4001,
    0x000051a5,
    0x01f680fa,
    0xffff5184,
}};
