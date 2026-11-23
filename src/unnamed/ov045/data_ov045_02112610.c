#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[8];
} data_ov045_02112610_Layout;
typedef char data_ov045_02112610_size[sizeof(data_ov045_02112610_Layout) == 0x20 ? 1 : -1];

typedef struct {
    u32 w[11];
} data_ov045_02112630_Layout;
typedef char data_ov045_02112630_size[sizeof(data_ov045_02112630_Layout) == 0x2c ? 1 : -1];

typedef struct {
    u32 f000;
    void *f004;
    u32 f008;
    void *f00c;
    u32 f010;
    void *f014;
    u32 f018;
    void *f01c;
    u32 f020;
    void *f024;
    u32 f028;
    void *f02c;
    u32 f030;
    void *f034;
} data_ov045_0211265c_Layout;
typedef char data_ov045_0211265c_size[sizeof(data_ov045_0211265c_Layout) == 0x38 ? 1 : -1];

typedef struct {
    u32 w[15];
} data_ov045_02112694_Layout;
typedef char data_ov045_02112694_size[sizeof(data_ov045_02112694_Layout) == 0x3c ? 1 : -1];

typedef struct {
    u32 w[20];
} data_ov045_021126d0_Layout;
typedef char data_ov045_021126d0_size[sizeof(data_ov045_021126d0_Layout) == 0x50 ? 1 : -1];

typedef struct {
    u32 w[24];
} data_ov045_02112720_Layout;
typedef char data_ov045_02112720_size[sizeof(data_ov045_02112720_Layout) == 0x60 ? 1 : -1];

typedef struct {
    u32 w[112];
} data_ov045_02112780_Layout;
typedef char data_ov045_02112780_size[sizeof(data_ov045_02112780_Layout) == 0x1c0 ? 1 : -1];

extern data_ov045_02112610_Layout data_ov045_02112610;
extern data_ov045_02112630_Layout data_ov045_02112630;
extern data_ov045_0211265c_Layout data_ov045_0211265c;
extern data_ov045_02112694_Layout data_ov045_02112694;
extern data_ov045_021126d0_Layout data_ov045_021126d0;
extern data_ov045_02112720_Layout data_ov045_02112720;
extern data_ov045_02112780_Layout data_ov045_02112780;
extern u8 data_ov045_02112460[];
extern u8 data_ov045_02112478[];
extern u8 data_ov045_02112468[];
extern u8 data_ov045_0211249c[];

data_ov045_02112610_Layout data_ov045_02112610 = {{
    0x53504c43,
    0x00020008,
    0x00000fc0,
    0x000000ff,
    0x01880fcc,
    0x000000ff,
    0xffffffff,
    0xffffffff,
}};

data_ov045_02112630_Layout data_ov045_02112630 = {{
    0xe1ecff02,
    0x02bcf830,
    0x80000000,
    0xff070000,
    0xf0740000,
    0x0000e318,
    0x00000000,
    0x1518ff02,
    0x038412c0,
    0x60000000,
    0x00000000,
}};

data_ov045_0211265c_Layout data_ov045_0211265c = {
    0x00000a02, &data_ov045_02112694,    0x00000103, &data_ov045_02112460[0],
    0x00000304, &data_ov045_02112630,    0x0000040b, &data_ov045_02112478[0],
    0x00000501, &data_ov045_021126d0,    0x0000040c, &data_ov045_02112468[0],
    0x0000010a, &data_ov045_0211249c[0],
};

data_ov045_02112694_Layout data_ov045_02112694 = {{
    0xf448ea20,
    0xed400000,
    0x0000f448,
    0xf3e4edd6,
    0xf15a0000,
    0x0000f3e4,
    0xf448f1f0,
    0xf3e40000,
    0x0000f448,
    0xf31cf5d8,
    0xf7cc0000,
    0x0000f31c,
    0xf448f9c0,
    0xfda80000,
    0x0000f448,
}};

data_ov045_021126d0_Layout data_ov045_021126d0 = {{
    0xe2500000, 0x0000f894, 0x40000000, 0x02000000, 0xe2500000, 0xfe70f574, 0x40000000,
    0x00000000, 0xe2500000, 0x00c8f574, 0x40000000, 0x00000000, 0xe2500000, 0x0190f574,
    0x40000000, 0x00000000, 0x16440000, 0x006413ec, 0x40000000, 0x02100000,
}};

data_ov045_02112720_Layout data_ov045_02112720 = {{
    0x53504c43, 0x000a0008, 0x00048fc0, 0x000000ff, 0x000c8fc0, 0x000000ff, 0x0004cfc0, 0x000000ff,
    0x0004afc0, 0x000000ff, 0x011c8fc0, 0x000000ff, 0x01048fc0, 0x000000ff, 0x01000fcc, 0x000000ff,
    0x0104afcc, 0x000000ff, 0x018c8fcc, 0x000000ff, 0x0104cfc0, 0x000000ff, 0xffffffff, 0xffffffff,
}};

data_ov045_02112780_Layout data_ov045_02112780 = {{
    0xf510e225, 0xfd5df529, 0xf894e225, 0xfd5df529, 0x044ce225, 0x0064f4c5, 0x044ce225, 0xffcef4c5,
    0x044ce225, 0xff38f4c5, 0x044ce225, 0x00faf4c5, 0x044ce225, 0x0190f4c5, 0x04b0e225, 0xf830f7cc,
    0x03e8e225, 0xf894f7cc, 0x0384e225, 0xf95cf7cc, 0x03e8e225, 0xfa24f7cc, 0x04b0e225, 0xfa88f7cc,
    0x0578e225, 0xfa24f7cc, 0x05dce225, 0xf95cf7cc, 0x0578e225, 0xf894f7cc, 0x04b02226, 0xf95cf8f8,
    0x16122226, 0x0000f830, 0x109a2226, 0xff6afcae, 0xfda8e225, 0xff6a0320, 0xfda8e225, 0x00000320,
    0xfda8e225, 0x00960320, 0xfda8e225, 0x012c0320, 0xfda8e225, 0x01c20320, 0xf8c6e225, 0x00640258,
    0xf830e225, 0x00960258, 0xf7fee225, 0x012c0258, 0xf830e225, 0x01c20258, 0xf8c6e225, 0x01f40258,
    0xf95ce225, 0x01c20258, 0xf98ee225, 0x012c0258, 0xf95ce225, 0x00960258, 0xf8f82226, 0xfe3e0640,
    0xea52e225, 0xff9c02ee, 0xea52e225, 0xff9c0384, 0xea52e225, 0xff9c041a, 0xea52e225, 0xff9c04b0,
    0xea52e225, 0xff9c0546, 0xeffc2226, 0x00c80a28, 0xe69ce225, 0xfce00c3b, 0xe732e225, 0xfce00c83,
    0xe7c8e225, 0xfce00ccb, 0xe85ee225, 0xfce00d12, 0xe8f4e225, 0xfce00d57, 0xf60ae225, 0xfed40dc5,
    0xf60ae225, 0xff6a0dc5, 0xf60ae225, 0x00000dc5, 0xf60ae225, 0x00960dc5, 0xf60ae225, 0x012c0dc5,
    0xfaba2226, 0xfe700dc5, 0x0bb82226, 0x006411c6, 0x0af02226, 0x012cfaba, 0x04b0423f, 0x0064157c,
    0x1388e3ff, 0x00000064, 0xe82ce5ff, 0x000002bc, 0xe890e7ff, 0x012c09c4, 0xebb0c43f, 0x02580ed8,
}};
