//cpp
#include "types.h"
#pragma opt_strength_reduction off
typedef struct Pair {
    s32 x;
    s32 y;
} Pair;

struct InitObject {
    virtual void v00(); virtual void v01(); virtual void v02(); virtual void v03();
    virtual void v04(); virtual void v05(); virtual void v06(); virtual void v07();
    virtual void v08(); virtual void v09(); virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
    virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23();
    virtual void v24(); virtual void v25(); virtual void v26(); virtual void v27();
    virtual void v28(); virtual void v29(); virtual void v30(); virtual void v31();
    virtual void v32(); virtual void v33(); virtual void v34();
    virtual int query();
};

extern "C" {

extern void func_ov004_020b04d0(int);
extern void SetBg2Offset(int, int);
extern void SetBg3Offset(int, int);
extern void SetSubBg2Offset(int, int);
extern void SetSubBg3Offset(int, int);
extern void func_020731dc(void *, void *, void **);
extern void func_0203ce80(s32 *, s32 *);
extern void Quaternion_FromVector3(s32 *, s32 *, s32 *);
extern void Quaternion_Normalize(s32 *);
extern void func_020072c0(void);
extern void func_ov006_02126948(char *);
extern void func_ov006_0212a3c0(char *);
extern void func_ov006_02125994(char *);

extern u32 data_ov006_02143004;
extern void *data_ov006_02143008;
extern s32 data_ov006_02143014[3];
extern s32 data_02092768[4];
}

#define I(o) (*(s32 *)(c + (o)))
#define H(o) (*(u16 *)(c + (o)))
#define B(o) (*(u8 *)(c + (o)))
#define AT(p, o) ((void *)(int)(((long long)(int)((char *)(p) + (o)))))

extern "C" void func_ov006_021279b0(char *c)
{
    s32 tmp[3];
    int i;
    int y;
    u16 display;
    Pair *saved;

    if (((InitObject *)c)->query()) {
        I(0xba00) = 0x2dc0;
        I(0xba04) = 0x190;
        I(0xba08) = 0x2e0;
        I(0xb9dc) = 0x960;
    } else {
        I(0xba00) = 0x1740;
        I(0xba04) = 0x190;
        I(0xba08) = 0x178;
        I(0xb9dc) = 0x4b0;
    }

    display = *(volatile u16 *)0x04000304;
    *(volatile u16 *)0x04000304 = display & ~0x8000;
    B(0xb9f8) = 1;
    func_ov004_020b04d0(0x50);

    y = I(0xba00);
    I(0xab38) = 0x80000;
    I(0xab3c) = y << 12;
    I(0xab50) = I(0xab38);
    I(0xab54) = I(0xab3c);
    y = I(0xab3c);
    I(0xab68) = 0;
    I(0xab6c) = y - 0x190000;

    y = I(0xab6c) >> 12;
    SetBg2Offset(0x100, y);
    SetBg3Offset(0, I(0xab6c) >> 12);
    SetSubBg2Offset(0, (I(0xab6c) >> 12) + 0x110);
    SetSubBg3Offset(0, (I(0xab6c) >> 12) + 0x110);

    *(volatile u32 *)0x04000018 = 0x100 | (((I(0xab6c) >> 12) << 16) & 0x01ff0000);
    *(volatile u32 *)0x0400001c = ((I(0xab6c) >> 12) << 16) & 0x01ff0000;
    *(volatile u32 *)0x04001018 = (((I(0xab6c) >> 12) + 0x110) << 16) & 0x01ff0000;
    *(volatile u32 *)0x0400101c = (((I(0xab6c) >> 12) + 0x110) << 16) & 0x01ff0000;

    I(0xab40) = I(0xab38);
    I(0xab44) = I(0xab3c);
    saved = (Pair *)AT(c, 0xab40);
    I(0xab48) = saved->x;
    I(0xab4c) = saved->y;
    I(0xab60) = 0;
    I(0xab64) = 0;
    I(0xaba0) = 0x4000;
    H(0xab78) = 0;
    H(0xab7a) = 0;
    H(0xab7c) = 0;
    H(0xab7e) = 0;
    H(0xab80) = 0;
    H(0xab82) = 0;

    if (!(data_ov006_02143004 & 1)) {
        data_ov006_02143014[0] = 0;
        data_ov006_02143014[1] = 0x1000;
        data_ov006_02143014[2] = 0;
        func_020731dc(data_ov006_02143014, (void *)&func_020072c0,
                      &data_ov006_02143008);
        data_ov006_02143004 |= 1;
    }

    I(0xab84) = data_ov006_02143014[0];
    I(0xab88) = data_ov006_02143014[1];
    I(0xab8c) = data_ov006_02143014[2];
    func_0203ce80(tmp, (s32 *)(c + 0xab84));

    I(0xab90) = data_02092768[0];
    I(0xab94) = data_02092768[1];
    I(0xab98) = data_02092768[2];
    I(0xab9c) = data_02092768[3];
    Quaternion_FromVector3((s32 *)(c + 0xab90), data_ov006_02143014,
                           (s32 *)(c + 0xab84));
    Quaternion_Normalize((s32 *)(c + 0xab90));

    func_ov006_02126948(c);
    for (i = 0; i < 0x20; i++) {
        char *p = c + i + 0xa000;
        *(u8 *)(p + 0xbf8) = 0;
        *(u8 *)(p + 0xc18) = 0;
        *(u8 *)(p + 0xc38) = 0;
    }

    I(0xb9e0) = 0;
    B(0xb9e4) = 0;
    B(0xb9e5) = 0;
    I(0xb9f4) = 0;
    I(0xab58) = 0;
    I(0xb9f0) = 0;

    func_ov006_0212a3c0(c);
    func_ov006_02125994(c);

    I(0xb9fc) = 0xf1;
    I(0xba0c) = 0;
    I(0xb9d8) = 0;
    B(0xb9e6) = 0;
    I(0xba10) = 0;
}
