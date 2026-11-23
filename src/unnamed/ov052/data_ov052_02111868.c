#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[1];
} data_ov052_02111868_Layout;
typedef char data_ov052_02111868_size[sizeof(data_ov052_02111868_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[1];
} data_ov052_0211186c_Layout;
typedef char data_ov052_0211186c_size[sizeof(data_ov052_0211186c_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 f000;
    void *f004;
    u32 f008;
    u32 f00c;
    u32 f010;
    u32 f014;
    u32 f018;
} data_ov052_02111870_Layout;
typedef char data_ov052_02111870_size[sizeof(data_ov052_02111870_Layout) == 0x1c ? 1 : -1];

typedef struct {
    u32 w[32];
} data_ov052_0211188c_Layout;
typedef char data_ov052_0211188c_size[sizeof(data_ov052_0211188c_Layout) == 0x80 ? 1 : -1];

extern const data_ov052_02111868_Layout data_ov052_02111868;
extern const data_ov052_0211186c_Layout data_ov052_0211186c;
extern const data_ov052_02111870_Layout data_ov052_02111870;
extern const data_ov052_0211188c_Layout data_ov052_0211188c;
extern u8 data_ov052_02111c7c[];

const data_ov052_02111868_Layout data_ov052_02111868 = {{
    0x00000000,
}};

const data_ov052_0211186c_Layout data_ov052_0211186c = {{
    0x00001000,
}};

const data_ov052_02111870_Layout data_ov052_02111870 = {
    0x0000ffff, &data_ov052_02111c7c[0], 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x000100b5,
};

const data_ov052_0211188c_Layout data_ov052_0211188c = {{
    0x00000000, 0x00000000, 0x00000017, 0x0000002e, 0x00000044, 0x0000005b, 0x00000072, 0x00000089,
    0x0000009f, 0x000000b6, 0x000000cd, 0x000000e4, 0x000000fa, 0x00000111, 0x00000128, 0x0000013f,
    0x00000155, 0x0000016c, 0x00000183, 0x0000019a, 0x000001b0, 0x000001c7, 0x000001de, 0x000001f5,
    0x0000020b, 0x00000222, 0x00000239, 0x00000250, 0x00000266, 0x0000027d, 0x00000294, 0x000002ab,
}};
