#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[3];
} data_ov023_02111a4c_Layout;
typedef char data_ov023_02111a4c_size[sizeof(data_ov023_02111a4c_Layout) == 0xc ? 1 : -1];

typedef struct {
    u32 w[8];
} data_ov023_02111a58_Layout;
typedef char data_ov023_02111a58_size[sizeof(data_ov023_02111a58_Layout) == 0x20 ? 1 : -1];

typedef struct {
    u32 w[12];
} data_ov023_02111a78_Layout;
typedef char data_ov023_02111a78_size[sizeof(data_ov023_02111a78_Layout) == 0x30 ? 1 : -1];

extern const data_ov023_02111a4c_Layout data_ov023_02111a4c;
extern const data_ov023_02111a58_Layout data_ov023_02111a58;
extern const data_ov023_02111a78_Layout data_ov023_02111a78;

const data_ov023_02111a4c_Layout data_ov023_02111a4c = {{
    0xfffff1c5,
    0xfffff1b0,
    0xfffff19c,
}};

const data_ov023_02111a58_Layout data_ov023_02111a58 = {{
    0xfffff187,
    0xfffff172,
    0xfffff15e,
    0xfffff149,
    0xfffff135,
    0xfffff120,
    0xfffff10c,
    0xfffff0f7,
}};

const data_ov023_02111a78_Layout data_ov023_02111a78 = {{
    0xfffff0e2,
    0xfffff0ce,
    0xfffff0b9,
    0xfffff0a5,
    0xfffff090,
    0xfffff07b,
    0xfffff067,
    0xfffff052,
    0xfffff03e,
    0xfffff029,
    0xfffff015,
    0xfffff000,
}};
