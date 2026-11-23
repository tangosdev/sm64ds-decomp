#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[1];
} data_ov030_02114690_Layout;
typedef char data_ov030_02114690_size[sizeof(data_ov030_02114690_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[1];
} data_ov030_02114694_Layout;
typedef char data_ov030_02114694_size[sizeof(data_ov030_02114694_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 f000;
    void *f004;
    u32 f008;
    u32 f00c;
    u32 f010;
    u32 f014;
    u32 f018;
} data_ov030_02114698_Layout;
typedef char data_ov030_02114698_size[sizeof(data_ov030_02114698_Layout) == 0x1c ? 1 : -1];

typedef struct {
    u32 w[15];
} data_ov030_021146b4_Layout;
typedef char data_ov030_021146b4_size[sizeof(data_ov030_021146b4_Layout) == 0x3c ? 1 : -1];

typedef struct {
    u32 w[77];
} data_ov030_021146f0_Layout;
typedef char data_ov030_021146f0_size[sizeof(data_ov030_021146f0_Layout) == 0x134 ? 1 : -1];

extern const data_ov030_02114690_Layout data_ov030_02114690;
extern const data_ov030_02114694_Layout data_ov030_02114694;
extern const data_ov030_02114698_Layout data_ov030_02114698;
extern const data_ov030_021146b4_Layout data_ov030_021146b4;
extern const data_ov030_021146f0_Layout data_ov030_021146f0;
extern u8 data_ov030_02114e08[];

const data_ov030_02114690_Layout data_ov030_02114690 = {{
    0x00000000,
}};

const data_ov030_02114694_Layout data_ov030_02114694 = {{
    0x00001000,
}};

const data_ov030_02114698_Layout data_ov030_02114698 = {
    0x0000ffff, &data_ov030_02114e08[0], 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x0001005b,
};

const data_ov030_021146b4_Layout data_ov030_021146b4 = {{
    0x00000000,
    0x00000000,
    0xffffff77,
    0xfffffeef,
    0xfffffe66,
    0xfffffdde,
    0xfffffd55,
    0xfffffccd,
    0xfffffc44,
    0xfffffbbc,
    0xfffffb33,
    0xfffffaab,
    0xfffffa22,
    0xfffff99a,
    0xfffff911,
}};

const data_ov030_021146f0_Layout data_ov030_021146f0 = {{
    0xfffff889, 0xfffff800, 0xfffff777, 0xfffff6ef, 0xfffff666, 0xfffff5de, 0xfffff555, 0xfffff4cd,
    0xfffff444, 0xfffff3bc, 0xfffff333, 0xfffff2ab, 0xfffff222, 0xfffff19a, 0xfffff111, 0xfffff089,
    0xfffff000, 0xffffef77, 0xffffeeef, 0xffffee66, 0xffffedde, 0xffffed55, 0xffffeccd, 0xffffec44,
    0xffffebbc, 0xffffeb33, 0xffffeaab, 0xffffea22, 0xffffe99a, 0xffffe911, 0xffffe889, 0xffffe800,
    0xffffe777, 0xffffe6ef, 0xffffe666, 0xffffe5de, 0xffffe555, 0xffffe4cd, 0xffffe444, 0xffffe3bc,
    0xffffe333, 0xffffe2ab, 0xffffe222, 0xffffe19a, 0xffffe111, 0xffffe089, 0xffffe000, 0xffffdf77,
    0xffffdeef, 0xffffde66, 0xffffddde, 0xffffdd55, 0xffffdccd, 0xffffdc44, 0xffffdbbc, 0xffffdb33,
    0xffffdaab, 0xffffda22, 0xffffd99a, 0xffffd911, 0xffffd889, 0xffffd800, 0xffffd777, 0xffffd6ef,
    0xffffd666, 0xffffd5de, 0xffffd555, 0xffffd4cd, 0xffffd444, 0xffffd3bc, 0xffffd333, 0xffffd2ab,
    0xffffd222, 0xffffd19a, 0xffffd111, 0xffffd089, 0xffffd000,
}};
