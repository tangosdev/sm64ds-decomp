//cpp
/* dScMB_c -- the multi-boot download scene, arm9.
 *
 * .text 0x02034a78..0x02035354. Twelve class functions plus the factory
 * dScMB_c_classInit, which abuts InitResources and stores this class's
 * vtable. The next symbol, func_02035354, is a list-node compare and is
 * not part of the scene.
 *
 * _ZTS7dScMB_c 0x02094364. _ZTI7dScMB_c 0x02094370, base word _ZTI8dScene_c.
 * Nested _ZTIN7dScMB_c15graphCallback_cE 0x0209437c,
 * _ZTSN7dScMB_c15graphCallback_cE 0x020943a0. _ZTV7dScMB_c 0x020943c4.
 * The out-of-line destructor is the key function. Under
 * `#pragma defer_codegen off` it emits D1 (0x02034a78), D0 (0x02034ac0),
 * then a base-object D2 the cartridge does not carry. That pragma also
 * makes emission ROM-ascending, so this file is written in ROM order.
 *
 * Leftover: OAM::Render stays mangled (Fix12 by value). func_02034d34
 * calls the fader at +0x50 through a local vtable view -- dFdColor_c's
 * header does not declare those slots. func_02034b1c writes the global
 * graphCallback_c by offset; it is that object's initializer, not a
 * dScMB_c method, and a member form was not required to match.
 * func_02034fbc stays free (no receiver). classInit stays the literal
 * construction sequence: `return new dScMB_c` would inline a different
 * fader constructor than the cartridge's vtable chain.
 */
#include "dScMB_c.h"
#include "decl_common.h"

extern "C" {
void MultiStore16(u16 val, char *dst, int nbytes);
void DecompressLZ16(int src, void *dst);
void _ZN2GX11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
void _ZN3GXS11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
int func_0201a244(int a0, int a1, int a2, int a3, int a4);
void func_020308b4(void);
void func_02012790(int x);

void *_ZN4Heap10SetDefaultEv(void *h);
void func_0201a5f8(int t);
void *func_0201a3e4(void);
extern u32 data_020a0c60;
extern void *data_020a0c5c;
/* Linker-defined overlay IDs: the address is the overlay number. */
extern int OVERLAY_100_ID;
extern int OVERLAY_102_ID;

void _ZN3OAM5ResetEv(void);
void _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(int sub, void *attr, int x, int y, int a, int cc, int fx, int mode);
void _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(int sub, void *attr, int x, int y, int a, int cc, int fx0, int fy0, int rot, int mode);
void _ZN3OAM9RenderSubEP7OamAttrii(void *attr, int a, int b);
void _ZN3OAM5FlushEv(void);
void _ZN3OAM4LoadEv(void);
extern struct OamAttr *data_0208a0f8[];
extern s16 data_02082214[];

extern u8 data_0209d45c;
extern u8 data_0209d454;
extern void *data_0209d4a8;
extern int data_0208ee44;
extern u8 func_0201a2f8[];

void *_ZN7fBase_cC2Ev(void *);
extern int data_0208e4b8[];
extern int data_020943c4[];
extern int _ZTV8dFader_c[];
extern int _ZTV15dFdBrightness_c[];
extern int _ZTV10dFdColor_c[];
}

/* Fader at dScMB_c+0x50. Slots 5 and 2 are what this TU calls; the header's
 * dFdColor_c does not declare them. */
struct ScMbFader {
    virtual int v0();
    virtual int v1();
    virtual int m2();
    virtual int v3();
    virtual int v4();
    virtual int m5();
};

#pragma defer_codegen off

// @symbol _ZN7dScMB_cD1Ev
dScMB_c::~dScMB_c()
{
}

// @symbol func_02034b1c
void func_02034b1c(void *c, int a)
{
    *(int *)((char *)c + 4) = a;
    *(int *)((char *)c + 8) = 0;
    *(unsigned char *)((char *)c + 0xc) = 0;
    *(unsigned char *)((char *)c + 0xd) = 1;
    *(unsigned char *)((char *)c + 0xe) = 1;
    *(unsigned char *)((char *)c + 0xf) = 0;
}

// @symbol _ZN7dScMB_c15graphCallback_c14GraphCallback3Ev
int dScMB_c::graphCallback_c::GraphCallback3()
{
    if (mOwner != 0) {
        ((dScMB_c *)mOwner)->func_02034d34();
    }
    _ZN3OAM5ResetEv();

    if (mEnabled != 0) {
        int id = func_0200f0bc();
        void *attr = data_0208a0f8[id];

        if (mStillTimer == 0) {
            int sb = mSpinAngle;
            int x = 0x80;
            int one = 1;
            int arr[3];
            arr[0] = 0;
            arr[1] = 0;
            arr[2] = 0;

            do {
                int y;
                if (sb >= 0 && sb < 0x10000) {
                    u16 t = (u16)sb;
                    s16 ang = (s16)t;
                    u16 t2 = (u16)ang;
                    y = data_02082214[(t2 >> 4) * 2];
                    y = (y >> 10) + 0x80;
                    arr[0] = one;
                } else {
                    y = x;
                }

                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(arr[1], attr, x, y, -1, -1, 0x1000, arr[1]);
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(one, attr, x, y, -1, -1, 0x1000, arr[2]);

                if (*(u16 *)((char *)attr + 6) == 0xffff)
                    break;
                attr = (char *)attr + 8;
                sb -= 0x2000;
            } while (1);

            if (mAdvancing != 0) {
                mSpinAngle += (data_0208ee44 << 10);
                if (arr[0] == 0)
                    mStillTimer = 0x78;
            }
        } else {
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, attr, 0x80, 0x80, -1, -1, 0x1000, 0x1000, 0, -1);
            _ZN3OAM9RenderSubEP7OamAttrii(attr, 0x80, 0x80);
            mStillTimer -= data_0208ee44;
            mSpinAngle = 0;
        }
    }

    func_0203083c();
    _ZN3OAM5FlushEv();
    _ZN3OAM4LoadEv();
    return 1;
}

// @symbol _ZN7dScMB_c15graphCallback_c14GraphCallback2Ev
int dScMB_c::graphCallback_c::GraphCallback2()
{
    return 0;
}

// @symbol _ZN7dScMB_c15graphCallback_c14GraphCallback0Ev
int dScMB_c::graphCallback_c::GraphCallback0()
{
    return 0;
}

// @symbol _ZN7dScMB_c13func_02034d34Ev
int dScMB_c::func_02034d34()
{
    int r = ((ScMbFader *)((char *)this + 0x50))->m5();
    if (r) return r;
    return ((ScMbFader *)((char *)this + 0x50))->m2();
}

// @symbol _ZN7dScMB_c16CleanupResourcesEv
s32 dScMB_c::CleanupResources()
{
    data_0209d4a8 = 0;
    dScene_c::SetAndStopColorFader();
    return 1;
}

// @symbol _ZN7dScMB_c6RenderEv
s32 dScMB_c::Render()
{
    return 1;
}

// @symbol _ZN7dScMB_c8BehaviorEv
s32 dScMB_c::Behavior()
{
    switch (unk_060) {
    case 0:
        if (func_0203d8fc() == 0) break;
        DecompressLZ16((int)data_0208a0e4[func_0200f0bc()], (void *)0x6400000);
        DecompressLZ16((int)data_0208a0e4[func_0200f0bc()], (void *)0x6600000);
        data_0209d45c = 0x10;
        data_0209d454 = 0x10;
        *(volatile u32 *)0x4000000 = (*(volatile u32 *)0x4000000 & ~0x1f00) | 0x1000;
        *(volatile u32 *)0x4001000 = (*(volatile u32 *)0x4001000 & ~0x1f00) | 0x1000;
        unk_060++;
        break;

    case 1:
        {
            int r = func_0201a1bc();
            *(int *)&data_020a0c64 = r;
            if (r == 0) break;
        }
        unk_060++;
        /* fallthrough */
    case 2:
        unk_060++;
        /* fallthrough */
    case 3:
        unk_060++;
        /* fallthrough */
    case 4:
        func_0201a244((int)&func_02034fbc, 0, 0xf, 0, 0x1000);
        unk_060++;
        /* fallthrough */
    case 5:
        if (func_0201a1bc() == 0) break;
        unk_060++;
        /* fallthrough */
    case 6:
        if (func_0203d7b8() != 0) break;
        func_0200f220();
        func_0200f13c();
        func_0203d930();
        func_020308b4();
        unk_060++;
        /* fallthrough */
    case 7:
        if (func_020308a8() == 0) break;
        dScene_c::StartSceneFade(6, 0, 0x7fff);
        func_02012790(0x11f);
        unk_060++;
        break;
    }

    if (unk_060 < 8) {
        unk_064++;
        if (unk_064 >= 0x1518) {
            data_0209fc54 = 1;
        }
    }

    return 1;
}

// @symbol func_02034fbc
void func_02034fbc(void)
{
    void *h;

    LoadArchive(0);
    h = (void *)func_0201a458();
    if (!(data_020a0c60 & 1)) {
        data_020a0c5c = _ZN4Heap10SetDefaultEv(h);
        data_020a0c60 |= 1;
    }
    LoadTextNarcs();
    LoadArchive(1);
    _ZN4Heap10SetDefaultEv(data_020a0c5c);
    func_0201a5f8(6);
    LoadOverlay((int)&OVERLAY_100_ID);
    LoadOverlay((int)&OVERLAY_102_ID);
    data_020a0c5c = _ZN4Heap10SetDefaultEv(func_0201a3e4());
    LoadArchive(7);
    _ZN4Heap10SetDefaultEv(data_020a0c5c);
}

// @symbol _ZN7dScMB_c13InitResourcesEv
s32 dScMB_c::InitResources()
{
    volatile u16 sp4;
    volatile u16 sp6;

    func_02053b98();
    *(volatile u32 *)0x4001000 &= ~0x10000;
    func_02030aa4(2);

    *(volatile u16 *)0x4000304 = (*(volatile u16 *)0x4000304 & ~0x20e) | 0x20e;
    func_0200f2cc();

    *(volatile u16 *)0x400000a = (*(volatile u16 *)0x400000a & 0x43) | 0x704;
    *(volatile u16 *)0x400000a &= ~0x40;
    *(volatile u16 *)0x400100a = (*(volatile u16 *)0x400100a & 0x43) | 0x408;
    *(volatile u16 *)0x400100a &= ~0x40;

    sp4 = 0xffff;
    MultiStore16(sp4, (char *)0x5000000, 2);
    sp6 = 0xffff;
    MultiStore16(sp6, (char *)0x5000400, 2);

    {
        char *vram = (char *)0x6400000;
        vram += 0x1280;
        DecompressLZ16((int)&data_0209446c, vram);
    }
    _ZN2GX11LoadOBJPlttEPKvjj((const void *)&data_0209444c, 0, 0x20);
    _ZN2GX11LoadOBJPlttEPKvjj((const void *)&data_020945d0, 0xa0, 0x20);
    _ZN3GXS11LoadOBJPlttEPKvjj((const void *)&data_0209444c, 0, 0x20);

    *(volatile u16 *)0x400000a &= ~3;
    *(volatile u16 *)0x400100a &= ~3;
    SetBg1Offset(0, 0);
    SetSubBg1Offset(0, 0);

    *(u8 *)&data_0209d45c = 0;
    *(u8 *)&data_0209d454 = 0;
    *(volatile u32 *)0x4000000 &= ~0x1f00;
    *(volatile u32 *)0x4001000 &= ~0x1f00;
    _ZN2GX6DispOnEv();

    *(volatile u32 *)0x4001000 |= 0x10000;
    func_02034b1c((void *)&data_020a0c68, (int)this);

    *(void **)&data_0209d4a8 = (void *)&data_020a0c68;
    unk_060 = 0;
    unk_064 = 0;
    dScene_c::SetFaders(&fader);

    fader.color = 0x7fff;
    *(int *)&data_0208ee44 = 1;
    *(int *)&data_020a0c64 = func_0201a244((int)&func_0201a2f8, 0, 0xf, 0, 0x1000);

    return 1;
}

// @symbol dScMB_c_classInit
extern "C" void *dScMB_c_classInit(void)
{
    char *p = (char *)_ZN7fBase_cnwEj(0x68);
    if (p) {
        _ZN7fBase_cC2Ev(p);
        *(int **)p = data_0208e4b8;
        *(int **)p = _ZTV8dScene_c;
        {
            u8 *bp = (u8 *)((int)p + 0x13);
            *bp |= 1;
            *bp |= 4;
        }
        *(int **)p = data_020943c4;
        {
            int *fp = (int *)((int)p + 0x50);
            fp[0] = (int)_ZTV8dFader_c;
            fp[0] = (int)_ZTV15dFdBrightness_c;
            fp[1] = 0x1000;
            fp[2] = 0;
            fp[0] = (int)_ZTV10dFdColor_c;
            *(short *)(fp + 3) = 0;
        }
    }
    return p;
}
