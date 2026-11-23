#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[2];
} _ZN3OAM13MM_WARIO_ICONE_Layout;
typedef char _ZN3OAM13MM_WARIO_ICONE_size[sizeof(_ZN3OAM13MM_WARIO_ICONE_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[4];
} _ZN3OAM20MM_WARIO_NO_CAP_ICONE_Layout;
typedef char
    _ZN3OAM20MM_WARIO_NO_CAP_ICONE_size[sizeof(_ZN3OAM20MM_WARIO_NO_CAP_ICONE_Layout) == 0x10 ? 1
                                                                                              : -1];

typedef struct {
    u32 w[6];
} _ZN3OAM19MM_WARIO_L_CAP_ICONE_Layout;
typedef char _ZN3OAM19MM_WARIO_L_CAP_ICONE_size[sizeof(_ZN3OAM19MM_WARIO_L_CAP_ICONE_Layout) == 0x18
                                                    ? 1
                                                    : -1];

typedef struct {
    u32 w[6];
} _ZN3OAM19MM_WARIO_M_CAP_ICONE_Layout;
typedef char _ZN3OAM19MM_WARIO_M_CAP_ICONE_size[sizeof(_ZN3OAM19MM_WARIO_M_CAP_ICONE_Layout) == 0x18
                                                    ? 1
                                                    : -1];

extern _ZN3OAM13MM_WARIO_ICONE_Layout _ZN3OAM13MM_WARIO_ICONE;
extern _ZN3OAM20MM_WARIO_NO_CAP_ICONE_Layout _ZN3OAM20MM_WARIO_NO_CAP_ICONE;
extern _ZN3OAM19MM_WARIO_L_CAP_ICONE_Layout _ZN3OAM19MM_WARIO_L_CAP_ICONE;
extern _ZN3OAM19MM_WARIO_M_CAP_ICONE_Layout _ZN3OAM19MM_WARIO_M_CAP_ICONE;

_ZN3OAM13MM_WARIO_ICONE_Layout _ZN3OAM13MM_WARIO_ICONE = {{
    0x81f000f0,
    0xffffc188,
}};

_ZN3OAM20MM_WARIO_NO_CAP_ICONE_Layout _ZN3OAM20MM_WARIO_NO_CAP_ICONE = {{
    0x81f04000,
    0x0000c1c8,
    0x81f040f0,
    0xffffc198,
}};

_ZN3OAM19MM_WARIO_L_CAP_ICONE_Layout _ZN3OAM19MM_WARIO_L_CAP_ICONE = {{
    0x41f400f0,
    0x0000b14f,
    0x000480f0,
    0x0000b151,
    0x81f000f0,
    0xffffc188,
}};

_ZN3OAM19MM_WARIO_M_CAP_ICONE_Layout _ZN3OAM19MM_WARIO_M_CAP_ICONE = {{
    0x41f400f0,
    0x0000a10f,
    0x000480f0,
    0x0000a111,
    0x81f000f0,
    0xffffc188,
}};
