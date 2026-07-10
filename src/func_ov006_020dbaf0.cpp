//cpp
/* func_ov006_020dbaf0 @ 0x020dbaf0 (ov006, size 0x264)
 * Minigame graphics init: loads/decompresses the OBJ tiles+palettes for both
 * engines, sets blending, patches the OAM attr template list, spawns the two
 * rows of 5 slot sprites, and resets the shared counters.
 */
typedef unsigned short u16;
typedef unsigned char u8;
typedef unsigned int u32;

typedef struct Slot6 {
    char b[0x30];
} Slot6;

typedef struct Obj6 {
    char _p0[0x51a8];
    Slot6 rowA[5]; /* 0x51a8 */
    Slot6 rowB[5]; /* 0x5298 */
} Obj6;

struct VtObj {
    virtual void d0();
    virtual void d1();
    virtual void d2();
    virtual void d3();
    virtual void d4();
    virtual void d5();
    virtual void d6();
    virtual void d7();
    virtual void d8();
    virtual void d9();
    virtual void d10();
    virtual void d11();
    virtual void d12();
    virtual void d13();
    virtual void d14();
    virtual void d15();
    virtual void d16();
    virtual void d17();
    virtual void m18(int);
};

extern "C" {
extern u8 data_0209d45c;
extern u8 data_0209d454;
extern int data_ov006_0213bcb0[];
extern u32 *data_ov006_02134028;
extern char *data_ov004_020beb68;

extern void func_ov004_020b04d0(int);
extern void func_ov006_0210a534(char *);
extern void *LoadFile(int);
extern int func_ov004_020ad674(void);
extern void DecompressLZ16(void *, u32);
extern void _ZN2GX11LoadOBJPlttEPKvjj(void *, u32, u32);
extern void _ZN3GXS11LoadOBJPlttEPKvjj(void *, u32, u32);
extern void Deallocate(void *);
extern void _ZN3G2x13SetBlendAlphaEPVttttt(volatile u16 *, int, int, int, int);
extern void func_ov006_020c0aa8(char *);
extern int func_ov006_020c1a88(char *);
extern void func_ov006_020da0ac(char *, int *);
extern int func_ov004_020ad8b8(void);
extern int func_ov004_020ad878(void);
extern void func_ov004_020b682c(void);

int func_ov006_020dbaf0(char *c)
{
    void *f7, *f6, *f5, *f4;
    int v[2];

    func_ov004_020b04d0(0x20);
    func_ov006_0210a534(c);
    data_0209d45c = 0x11;
    data_0209d454 = 0x18;
    f7 = LoadFile(0xbd);
    f6 = LoadFile(0xbe);
    f5 = LoadFile(data_ov006_0213bcb0[func_ov004_020ad674()]);
    f4 = LoadFile(0xbb);
    DecompressLZ16(f7, 0x6400000);
    _ZN2GX11LoadOBJPlttEPKvjj(f6, 0, 0x20);
    DecompressLZ16(f5, 0x6600000);
    _ZN3GXS11LoadOBJPlttEPKvjj(f4, 0, 0x100);
    Deallocate(f7);
    Deallocate(f6);
    Deallocate(f5);
    Deallocate(f4);
    _ZN3G2x13SetBlendAlphaEPVttttt((volatile u16 *)0x04000050, 0, 1, 0x10, 8);
    *(volatile u16 *)0x04001050 = 0;

    {
        u32 *e = data_ov006_02134028;
        u16 last;
        do {
            e[0] = (e[0] & ~0xc00) | 0x400;
            *((u16 *)e + 2) &= ~0xf000;
            last = *((u16 *)e + 3);
            e += 2;
        } while (last != 0xffff);
    }

    *(volatile u16 *)0x04000008 = (*(volatile u16 *)0x04000008 & ~3) | 1;

    func_ov006_020c0aa8(c + 0x4660);
    if (func_ov006_020c1a88(c + 0x4f38) == 0)
        return 0;

    {
        int y = 0x2c;
        int i;
        for (i = 0; i < 5; i++) {
            v[0] = y << 12;
            v[1] = 0x80000;
            func_ov006_020da0ac(((Obj6 *)c)->rowA[i].b, v);
            v[1] = 0x10000;
            func_ov006_020da0ac(((Obj6 *)c)->rowB[i].b, v);
            y += 0x28;
        }
    }

    *(u16 *)(c + 0x5388) = 0;
    *(u16 *)(c + 0x538a) = 0;
    *(int *)(c + 0xa8) = func_ov004_020ad8b8();
    *(int *)(c + 0xac) = *(int *)(c + 0xa8);
    {
        int r = func_ov004_020ad878();
        if (data_ov004_020beb68 != 0)
            *(int *)(data_ov004_020beb68 + 0xb4) = r;
    }
    *(u16 *)(c + 0x5398) = 0;
    func_ov004_020b682c();
    ((VtObj *)c)->m18(-1);
    return 1;
}
}
