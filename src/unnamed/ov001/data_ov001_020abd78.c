#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[2];
} _ZN3OAM12POINTER_DOWNE_Layout;
typedef char _ZN3OAM12POINTER_DOWNE_size[sizeof(_ZN3OAM12POINTER_DOWNE_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[2];
} _ZN3OAM13POINTER_RIGHTE_Layout;
typedef char _ZN3OAM13POINTER_RIGHTE_size[sizeof(_ZN3OAM13POINTER_RIGHTE_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[4];
} _ZN3OAM14BOUNCING_ARROWE_Layout;
typedef char
    _ZN3OAM14BOUNCING_ARROWE_size[sizeof(_ZN3OAM14BOUNCING_ARROWE_Layout) == 0x10 ? 1 : -1];

typedef struct {
    void *f000;
    void *f004;
    void *f008;
    void *f00c;
} data_ov001_020abd98_Layout;
typedef char data_ov001_020abd98_size[sizeof(data_ov001_020abd98_Layout) == 0x10 ? 1 : -1];

typedef struct {
    u32 w[12];
} data_ov001_020abda8_Layout;
typedef char data_ov001_020abda8_size[sizeof(data_ov001_020abda8_Layout) == 0x30 ? 1 : -1];

typedef struct {
    u32 w[82];
} data_ov001_020abdd8_Layout;
typedef char data_ov001_020abdd8_size[sizeof(data_ov001_020abdd8_Layout) == 0x148 ? 1 : -1];

typedef struct {
    u32 w[86];
} data_ov001_020abf20_Layout;
typedef char data_ov001_020abf20_size[sizeof(data_ov001_020abf20_Layout) == 0x158 ? 1 : -1];

typedef struct {
    u32 w[104];
} data_ov001_020ac078_Layout;
typedef char data_ov001_020ac078_size[sizeof(data_ov001_020ac078_Layout) == 0x1a0 ? 1 : -1];

extern _ZN3OAM12POINTER_DOWNE_Layout _ZN3OAM12POINTER_DOWNE;
extern _ZN3OAM13POINTER_RIGHTE_Layout _ZN3OAM13POINTER_RIGHTE;
extern _ZN3OAM14BOUNCING_ARROWE_Layout _ZN3OAM14BOUNCING_ARROWE;
extern data_ov001_020abd98_Layout data_ov001_020abd98;
extern data_ov001_020abda8_Layout data_ov001_020abda8;
extern data_ov001_020abdd8_Layout data_ov001_020abdd8;
extern data_ov001_020abf20_Layout data_ov001_020abf20;
extern data_ov001_020ac078_Layout data_ov001_020ac078;

_ZN3OAM13POINTER_RIGHTE_Layout _ZN3OAM13POINTER_RIGHTE = {{
    0x41f800f8,
    0xfffff05c,
}};

_ZN3OAM12POINTER_DOWNE_Layout _ZN3OAM12POINTER_DOWNE = {{
    0x41f800f8,
    0xfffff05e,
}};

_ZN3OAM14BOUNCING_ARROWE_Layout _ZN3OAM14BOUNCING_ARROWE = {{
    0x41f800f4,
    0x00001080,
    0x01f84004,
    0xffff10a2,
}};

data_ov001_020abd98_Layout data_ov001_020abd98 = {
    &data_ov001_020abf20,
    &data_ov001_020abdd8,
    &data_ov001_020ac078,
    &data_ov001_020abda8,
};

data_ov001_020abda8_Layout data_ov001_020abda8 = {{
    0xc1a000d8,
    0x00001208,
    0xc1e000d8,
    0x00001210,
    0xc02000d8,
    0x00001218,
    0xc1a00018,
    0x00001308,
    0xc1e00018,
    0x00001310,
    0xc0200018,
    0xffff1318,
}};

data_ov001_020abdd8_Layout data_ov001_020abdd8 = {{
    0x804740ed, 0x0000f10f, 0x406700ed, 0x0000f113, 0x003f00ef, 0x0000f11f, 0x80394023, 0x0000f18c,
    0x003f8014, 0x0000f11e, 0x004c80cd, 0x0000f11d, 0x803b40c0, 0x0000f180, 0x80564002, 0x0000f187,
    0x00768002, 0x0000f18b, 0x004b4007, 0x0000f11b, 0x800c4048, 0x0000f146, 0x0018801d, 0x0000f11e,
    0x4018802b, 0x0000f11f, 0x802d4042, 0x0000f14e, 0x00268035, 0x0000f1c9, 0x00330007, 0x0000f11a,
    0x002e0007, 0x0000f139, 0x002c8024, 0x0000f11f, 0x402c800d, 0x0000f11f, 0x81e640f0, 0x0000f106,
    0x01de00f5, 0x0000f135, 0x002e002d, 0x0000f1ce, 0x01ad80cd, 0x0000f11d, 0x818d40c0, 0x0000f140,
    0x81bc40c0, 0x0000f1c9, 0x01dc80c0, 0x0000f1cd, 0x01c400b8, 0x0000f1ce, 0x41ad00c0, 0x0000f144,
    0x805340c0, 0x0000f183, 0x00598023, 0x0000f190, 0x80454042, 0x0000f151, 0x004f8035, 0x0000f155,
    0x405a0035, 0x0000f156, 0x802e4035, 0x0000f192, 0x003f001f, 0x0000f11f, 0x003f80f2, 0x0000f11d,
    0x004100d8, 0x0000f1ce, 0x006280e0, 0x0000f155, 0x406d00e0, 0x0000f156, 0x804140e0, 0x0000f192,
    0x003980e0, 0xfffff1c9,
}};

data_ov001_020abf20_Layout data_ov001_020abf20 = {{
    0x01e80027, 0x0000f137, 0x003f80f2, 0x0000f11d, 0x01ad80cd, 0x0000f11d, 0x80564002, 0x0000f187,
    0x00768002, 0x0000f18b, 0x004b4007, 0x0000f11b, 0x818d40c0, 0x0000f140, 0x81bc40c0, 0x0000f1c9,
    0x01dc80c0, 0x0000f1cd, 0x803940e5, 0x0000f146, 0x81ff4048, 0x0000f1c9, 0x81de4048, 0x0000f14a,
    0x001f8048, 0x0000f1cd, 0x81e640f0, 0x0000f106, 0x01de00f5, 0x0000f135, 0x8026403a, 0x0000f10a,
    0x402c8023, 0x0000f11f, 0x00330007, 0x0000f11a, 0x002e0007, 0x0000f139, 0x402c800d, 0x0000f11f,
    0x01e8801d, 0x0000f11e, 0x41e8802c, 0x0000f11f, 0x41ad4018, 0x0000f115, 0x01b30028, 0x0000f13b,
    0x01ba0020, 0x0000f138, 0x00084027, 0x0000f13b, 0x01f84027, 0x0000f13a, 0x01eb4027, 0x0000f13a,
    0x818a4023, 0x0000f100, 0x0011801d, 0x0000f11e, 0x00070040, 0x0000f1ce, 0x01c400b8, 0x0000f1ce,
    0x41aa0023, 0x0000f104, 0x01b90028, 0x0000f13c, 0x41ad00c0, 0x0000f144, 0x004c80cd, 0x0000f11d,
    0x803b40c0, 0x0000f180, 0x805340c0, 0x0000f183, 0x0046803a, 0x0000f10e, 0x80394023, 0x0000f18c,
    0x003f8014, 0x0000f11e, 0x00598023, 0x0000f190, 0x003f001f, 0xfffff11f,
}};

data_ov001_020ac078_Layout data_ov001_020ac078 = {{
    0x003f80f2, 0x0000f11d, 0x01974007, 0x0000f139, 0x81b4403a, 0x0000f1c9, 0x8185403a, 0x0000f140,
    0x01d4803a, 0x0000f1cd, 0x41958022, 0x0000f11f, 0x4195800e, 0x0000f11f, 0x01a44007, 0x0000f119,
    0x01ba8014, 0x0000f11e, 0x01ba80f2, 0x0000f11d, 0x81b44023, 0x0000f180, 0x01bc80e5, 0x0000f191,
    0x004180e5, 0x0000f191, 0x81b440e5, 0x0000f18c, 0x80564002, 0x0000f187, 0x004b4007, 0x0000f11b,
    0x803940e5, 0x0000f18c, 0x003f8014, 0x0000f11e, 0x80394023, 0x0000f180, 0x001880d3, 0x0000f11f,
    0x601880d9, 0x0000f11f, 0x4011802b, 0x0000f11f, 0x001880f2, 0x0000f11d, 0x003f801e, 0x0000f11f,
    0x81d44002, 0x0000f187, 0x01c84007, 0x0000f11b, 0x01ba801d, 0x0000f15f, 0x01f48002, 0x0000f18b,
    0x00768002, 0x0000f18b, 0x81ff4048, 0x0000f1c9, 0x81de4048, 0x0000f14a, 0x0011801d, 0x0000f11e,
    0x00330007, 0x0000f11a, 0x002e0007, 0x0000f139, 0x8055403a, 0x0000f1c9, 0x8026403a, 0x0000f140,
    0x402c8020, 0x0000f11f, 0x402c800d, 0x0000f11f, 0x0075803a, 0x0000f1cd, 0x001f8048, 0x0000f1cd,
    0x801240c4, 0x0000f146, 0x81e640f0, 0x0000f106, 0x01de00f5, 0x0000f135, 0x005d0032, 0x0000f1ce,
    0x00070040, 0x0000f1ce, 0x01bc0032, 0x0000f1ce, 0x41a5003a, 0x0000f144, 0x4046003a, 0x0000f144,
    0x81cc4023, 0x0000f183, 0x80514023, 0x0000f183, 0x01d480e5, 0x0000f190, 0x005980e5, 0xfffff190,
}};
