#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[3];
} data_ov052_02111a4c_Layout;
typedef char data_ov052_02111a4c_size[sizeof(data_ov052_02111a4c_Layout) == 0xc ? 1 : -1];

typedef struct {
    u32 w[8];
} data_ov052_02111a58_Layout;
typedef char data_ov052_02111a58_size[sizeof(data_ov052_02111a58_Layout) == 0x20 ? 1 : -1];

typedef struct {
    u32 w[17];
} data_ov052_02111a78_Layout;
typedef char data_ov052_02111a78_size[sizeof(data_ov052_02111a78_Layout) == 0x44 ? 1 : -1];

extern const data_ov052_02111a4c_Layout data_ov052_02111a4c;
extern const data_ov052_02111a58_Layout data_ov052_02111a58;
extern const data_ov052_02111a78_Layout data_ov052_02111a78;

const data_ov052_02111a4c_Layout data_ov052_02111a4c = {{
    0x000009de,
    0x000009f5,
    0x00000a0b,
}};

const data_ov052_02111a58_Layout data_ov052_02111a58 = {{
    0x00000a22,
    0x00000a39,
    0x00000a50,
    0x00000a66,
    0x00000a7d,
    0x00000a94,
    0x00000aab,
    0x00000ac1,
}};

const data_ov052_02111a78_Layout data_ov052_02111a78 = {{
    0x00000ad8,
    0x00000aef,
    0x00000b06,
    0x00000b1c,
    0x00000b33,
    0x00000b4a,
    0x00000b61,
    0x00000b77,
    0x00000b8e,
    0x00000ba5,
    0x00000bbc,
    0x00000bd2,
    0x00000be9,
    0x00000c00,
    0x00000c17,
    0x00000c2e,
    0x00000c44,
}};
