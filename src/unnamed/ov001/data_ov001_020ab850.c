#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[2];
} _ZN3OAM13MM_LUIGI_ICONE_Layout;
typedef char _ZN3OAM13MM_LUIGI_ICONE_size[sizeof(_ZN3OAM13MM_LUIGI_ICONE_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[6];
} _ZN3OAM19MM_LUIGI_W_CAP_ICONE_Layout;
typedef char _ZN3OAM19MM_LUIGI_W_CAP_ICONE_size[sizeof(_ZN3OAM19MM_LUIGI_W_CAP_ICONE_Layout) == 0x18
                                                    ? 1
                                                    : -1];

typedef struct {
    u32 w[6];
} _ZN3OAM19MM_LUIGI_M_CAP_ICONE_Layout;
typedef char _ZN3OAM19MM_LUIGI_M_CAP_ICONE_size[sizeof(_ZN3OAM19MM_LUIGI_M_CAP_ICONE_Layout) == 0x18
                                                    ? 1
                                                    : -1];

typedef struct {
    u32 w[6];
} _ZN3OAM20MM_LUIGI_NO_CAP_ICONE_Layout;
typedef char
    _ZN3OAM20MM_LUIGI_NO_CAP_ICONE_size[sizeof(_ZN3OAM20MM_LUIGI_NO_CAP_ICONE_Layout) == 0x18 ? 1
                                                                                              : -1];

extern _ZN3OAM13MM_LUIGI_ICONE_Layout _ZN3OAM13MM_LUIGI_ICONE;
extern _ZN3OAM19MM_LUIGI_W_CAP_ICONE_Layout _ZN3OAM19MM_LUIGI_W_CAP_ICONE;
extern _ZN3OAM19MM_LUIGI_M_CAP_ICONE_Layout _ZN3OAM19MM_LUIGI_M_CAP_ICONE;
extern _ZN3OAM20MM_LUIGI_NO_CAP_ICONE_Layout _ZN3OAM20MM_LUIGI_NO_CAP_ICONE;

_ZN3OAM13MM_LUIGI_ICONE_Layout _ZN3OAM13MM_LUIGI_ICONE = {{
    0x81f000f0,
    0xffffb184,
}};

_ZN3OAM19MM_LUIGI_W_CAP_ICONE_Layout _ZN3OAM19MM_LUIGI_W_CAP_ICONE = {{
    0x41f400f0,
    0x0000c14c,
    0x000480f0,
    0x0000c14e,
    0x81f000f0,
    0xffffb184,
}};

_ZN3OAM19MM_LUIGI_M_CAP_ICONE_Layout _ZN3OAM19MM_LUIGI_M_CAP_ICONE = {{
    0x41f400f0,
    0x0000a10c,
    0x000480f0,
    0x0000a10e,
    0x81f000f0,
    0xffffb184,
}};

_ZN3OAM20MM_LUIGI_NO_CAP_ICONE_Layout _ZN3OAM20MM_LUIGI_NO_CAP_ICONE = {{
    0x81f04000,
    0x0000b1c4,
    0x41f400f0,
    0x0000b195,
    0x000480f0,
    0xffffb197,
}};
