#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[2];
} _ZN3OAM13MM_YOSHI_ICONE_Layout;
typedef char _ZN3OAM13MM_YOSHI_ICONE_size[sizeof(_ZN3OAM13MM_YOSHI_ICONE_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[6];
} _ZN3OAM19MM_YOSHI_L_CAP_ICONE_Layout;
typedef char _ZN3OAM19MM_YOSHI_L_CAP_ICONE_size[sizeof(_ZN3OAM19MM_YOSHI_L_CAP_ICONE_Layout) == 0x18
                                                    ? 1
                                                    : -1];

typedef struct {
    u32 w[6];
} _ZN3OAM19MM_YOSHI_M_CAP_ICONE_Layout;
typedef char _ZN3OAM19MM_YOSHI_M_CAP_ICONE_size[sizeof(_ZN3OAM19MM_YOSHI_M_CAP_ICONE_Layout) == 0x18
                                                    ? 1
                                                    : -1];

typedef struct {
    u32 w[6];
} _ZN3OAM19MM_YOSHI_W_CAP_ICONE_Layout;
typedef char _ZN3OAM19MM_YOSHI_W_CAP_ICONE_size[sizeof(_ZN3OAM19MM_YOSHI_W_CAP_ICONE_Layout) == 0x18
                                                    ? 1
                                                    : -1];

extern _ZN3OAM13MM_YOSHI_ICONE_Layout _ZN3OAM13MM_YOSHI_ICONE;
extern _ZN3OAM19MM_YOSHI_L_CAP_ICONE_Layout _ZN3OAM19MM_YOSHI_L_CAP_ICONE;
extern _ZN3OAM19MM_YOSHI_M_CAP_ICONE_Layout _ZN3OAM19MM_YOSHI_M_CAP_ICONE;
extern _ZN3OAM19MM_YOSHI_W_CAP_ICONE_Layout _ZN3OAM19MM_YOSHI_W_CAP_ICONE;

_ZN3OAM13MM_YOSHI_ICONE_Layout _ZN3OAM13MM_YOSHI_ICONE = {{
    0x81f000f0,
    0xffffd18c,
}};

_ZN3OAM19MM_YOSHI_L_CAP_ICONE_Layout _ZN3OAM19MM_YOSHI_L_CAP_ICONE = {{
    0x41f400eb,
    0x0000b103,
    0x000480eb,
    0x0000b105,
    0x81f000f0,
    0xffffd18c,
}};

_ZN3OAM19MM_YOSHI_M_CAP_ICONE_Layout _ZN3OAM19MM_YOSHI_M_CAP_ICONE = {{
    0x41f400ea,
    0x0000a100,
    0x000480ea,
    0x0000a102,
    0x81f000f0,
    0xffffd18c,
}};

_ZN3OAM19MM_YOSHI_W_CAP_ICONE_Layout _ZN3OAM19MM_YOSHI_W_CAP_ICONE = {{
    0x41f400ea,
    0x0000c106,
    0x000480ea,
    0x0000c108,
    0x81f000f0,
    0xffffd18c,
}};
