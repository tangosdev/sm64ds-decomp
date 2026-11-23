#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[1];
} data_ov023_02111760_Layout;
typedef char data_ov023_02111760_size[sizeof(data_ov023_02111760_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[1];
} data_ov023_02111764_Layout;
typedef char data_ov023_02111764_size[sizeof(data_ov023_02111764_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 f000;
    void *f004;
    u32 f008;
    u32 f00c;
    u32 f010;
    u32 f014;
    u32 f018;
} data_ov023_02111768_Layout;
typedef char data_ov023_02111768_size[sizeof(data_ov023_02111768_Layout) == 0x1c ? 1 : -1];

typedef struct {
    u32 w[44];
} data_ov023_02111784_Layout;
typedef char data_ov023_02111784_size[sizeof(data_ov023_02111784_Layout) == 0xb0 ? 1 : -1];

extern const data_ov023_02111760_Layout data_ov023_02111760;
extern const data_ov023_02111764_Layout data_ov023_02111764;
extern const data_ov023_02111768_Layout data_ov023_02111768;
extern const data_ov023_02111784_Layout data_ov023_02111784;
extern u8 data_ov023_02111b30[];

const data_ov023_02111760_Layout data_ov023_02111760 = {{
    0x00000000,
}};

const data_ov023_02111764_Layout data_ov023_02111764 = {{
    0x00001000,
}};

const data_ov023_02111768_Layout data_ov023_02111768 = {
    0x0000ffff, &data_ov023_02111b30[0], 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x000100c8,
};

const data_ov023_02111784_Layout data_ov023_02111784 = {{
    0x00000000, 0x00000000, 0xffffffeb, 0xffffffd7, 0xffffffc2, 0xffffffae, 0xffffff99, 0xffffff85,
    0xffffff70, 0xffffff5b, 0xffffff47, 0xffffff32, 0xffffff1e, 0xffffff09, 0xfffffef4, 0xfffffee0,
    0xfffffecb, 0xfffffeb7, 0xfffffea2, 0xfffffe8e, 0xfffffe79, 0xfffffe64, 0xfffffe50, 0xfffffe3b,
    0xfffffe27, 0xfffffe12, 0xfffffdfd, 0xfffffde9, 0xfffffdd4, 0xfffffdc0, 0xfffffdab, 0xfffffd97,
    0xfffffd82, 0xfffffd6d, 0xfffffd59, 0xfffffd44, 0xfffffd30, 0xfffffd1b, 0xfffffd06, 0xfffffcf2,
    0xfffffcdd, 0xfffffcc9, 0xfffffcb4, 0xfffffca0,
}};
