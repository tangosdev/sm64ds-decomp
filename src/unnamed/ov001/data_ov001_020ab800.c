#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[2];
} _ZN3OAM13MM_MARIO_ICONE_Layout;
typedef char _ZN3OAM13MM_MARIO_ICONE_size[sizeof(_ZN3OAM13MM_MARIO_ICONE_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[6];
} _ZN3OAM19MM_MARIO_W_CAP_ICONE_Layout;
typedef char _ZN3OAM19MM_MARIO_W_CAP_ICONE_size[sizeof(_ZN3OAM19MM_MARIO_W_CAP_ICONE_Layout) == 0x18
                                                    ? 1
                                                    : -1];

typedef struct {
    u32 w[6];
} _ZN3OAM19MM_MARIO_L_CAP_ICONE_Layout;
typedef char _ZN3OAM19MM_MARIO_L_CAP_ICONE_size[sizeof(_ZN3OAM19MM_MARIO_L_CAP_ICONE_Layout) == 0x18
                                                    ? 1
                                                    : -1];

typedef struct {
    u32 w[6];
} _ZN3OAM20MM_MARIO_NO_CAP_ICONE_Layout;
typedef char
    _ZN3OAM20MM_MARIO_NO_CAP_ICONE_size[sizeof(_ZN3OAM20MM_MARIO_NO_CAP_ICONE_Layout) == 0x18 ? 1
                                                                                              : -1];

extern _ZN3OAM13MM_MARIO_ICONE_Layout _ZN3OAM13MM_MARIO_ICONE;
extern _ZN3OAM19MM_MARIO_W_CAP_ICONE_Layout _ZN3OAM19MM_MARIO_W_CAP_ICONE;
extern _ZN3OAM19MM_MARIO_L_CAP_ICONE_Layout _ZN3OAM19MM_MARIO_L_CAP_ICONE;
extern _ZN3OAM20MM_MARIO_NO_CAP_ICONE_Layout _ZN3OAM20MM_MARIO_NO_CAP_ICONE;

_ZN3OAM13MM_MARIO_ICONE_Layout _ZN3OAM13MM_MARIO_ICONE = {{
    0x81f000f0,
    0xffffa180,
}};

_ZN3OAM19MM_MARIO_W_CAP_ICONE_Layout _ZN3OAM19MM_MARIO_W_CAP_ICONE = {{
    0x41f400f0,
    0x0000c149,
    0x000480f0,
    0x0000c14b,
    0x81f000f0,
    0xffffa180,
}};

_ZN3OAM19MM_MARIO_L_CAP_ICONE_Layout _ZN3OAM19MM_MARIO_L_CAP_ICONE = {{
    0x41f400f0,
    0x0000b109,
    0x000480f0,
    0x0000b10b,
    0x81f000f0,
    0xffffa180,
}};

_ZN3OAM20MM_MARIO_NO_CAP_ICONE_Layout _ZN3OAM20MM_MARIO_NO_CAP_ICONE = {{
    0x41f400f0,
    0x0000a192,
    0x000480f0,
    0x0000a194,
    0x81f04000,
    0xffffa1c0,
}};
