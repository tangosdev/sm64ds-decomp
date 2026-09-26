//cpp
/* Tamaire (scene 0x171, profile MG_TAMAIRE): dScMgPachinko2_c, the whole
 * class in one translation unit. 74 functions, .text 0x020ff420..0x0210428c:
 * the destructor pair, 67 free helpers, OnYoshiTryEat, Render, Behavior,
 * InitResources and the factory.
 *
 * The destructor is declared first in the class header, so this TU owns the
 * key function and emits dScMgPachinko2_c's vtable and RTTI; the ROM's own
 * copies live in ov006 .data, and these are discarded at link time.
 *
 * Functions run in ROM order here, lowest address first, under `#pragma
 * defer_codegen off`. Do not reorder. Each optimisation pragma sits in its
 * own push/pop bracket around the one member that needs it.
 *
 * Still raw: the helpers are unnamed in symbols.txt and reach the scene
 * through byte offsets; the offsets above 0x5260 are padding in
 * dScMgPachinko2_c.h. Where two helpers once named different views of the
 * scene alike, the later view carries its helper's address as a suffix.
 */

#pragma defer_codegen off

#include "types.h"
#include "dScMgPachinko2_c.h"

namespace Sound {
    void PlayBank2_2D(unsigned int id);
}

/* The receiver the pointer-to-member tables dispatch on. The member type is
   formed while C is still incomplete: the compiler picks the
   pointer-to-member layout from that, and completing the class first changes
   the code. */
struct C;
typedef void (C::*PMF)(int);
struct Entry { PMF pmf; };
struct C { char pad[1]; };

/* func_ov006_020ff47c's two score popups at 0x5620. */
struct E {
    int x;                  /* 0x00 */
    int y;                  /* 0x04 */
    char pad0[0x0f];        /* 0x08 */
    unsigned char idx;      /* 0x17 */
    char pad1;              /* 0x18 */
    unsigned char flag;     /* 0x19 */
    char pad2[6];           /* 0x1a */
};

struct Obj {
    char pad[0x5620];
    struct E arr[2];
};

/* func_ov006_020ff4ec's two slot entries at 0x563b. */
struct Slot {
    u8 active;   /* +0x00 (this+0x563b) */
    u8 count;    /* +0x01 */
    u8 id;       /* +0x02 */
    u8 timer;    /* +0x03 */
    char _pad[0x1c];
};

struct Obj4ec {
    char _pad[0x563b];
    struct Slot slots[2];  /* 0x563b */
};

/* func_ov006_02100380's view of the 16 entries at 0x5330. */
typedef struct {
    char _pad0[0x5330];
    u16 timer;   /* +0x5330 */
    char _pad1[2];
    u8 f4;       /* +0x5334 */
    u8 stage;    /* +0x5335 */
    u8 active;   /* +0x5336 */
} View;

/* func_ov006_021004c0's view: the state word at 0x5660. */
struct Obj4c0 { char pad[0x5660]; int f; /* 0x5660 */ };

/* func_ov006_0210068c's three 0x40-byte records at 0x5260. */
struct E68c {
    int x;                          /* +0x00 (abs 0x5260) */
    int y;                          /* +0x04 (abs 0x5264) */
    unsigned char pad0[0x35 - 0x08];
    unsigned char flag;             /* +0x35 (abs 0x5295) */
    unsigned char pad1[0x40 - 0x36];
};

struct Obj68c {
    unsigned char pad[0x5260];
    struct E68c arr[3];
};

/* func_ov006_02100e3c's view of the same records. */
struct Sub {
    /* 0x00 */ s32 unk00;
    /* 0x04 */ s32 unk04;
    /* 0x08 */ s32 unk08;
    /* 0x0c */ s32 unk0c;
    /* 0x10 */ s32 unk10;
    /* 0x14 */ s32 unk14;
    /* 0x18 */ s32 unk18;
    /* 0x1c */ s32 unk1c;
    /* 0x20 */ s32 unk20;
    /* 0x24 */ s32 unk24;
    /* 0x28 */ s32 unk28;
    /* 0x2c */ s32 unk2c;
    /* 0x30 */ u16 unk30;
    /* 0x32 */ u16 unk32;
    /* 0x34 */ u16 unk34;
    /* 0x36 */ u8 unk36;
    /* 0x37 */ u8 unk37;
    /* 0x38 */ u8 unk38;
    /* 0x39 */ u8 unk39;
    /* 0x3a */ u8 unk3a;
    /* 0x3b */ u8 unk3b;
    /* 0x3c */ s32 unk3c;
};

struct Obje3c {
    /* 0x0000 */ u8 pad0[0x5260];
    /* 0x5260 */ struct Sub sub[2];
};

/* func_ov006_0210246c's stride-only view of the records at 0x5260. */
struct Row { u8 d[0x40]; };

/* func_ov006_02102d6c's view of the 0x40-byte balls at 0x4660. */
struct Entryd6c {
    u32 handle;    /* +0x00 (this+0x4660) */
    int a;         /* +0x04 = 0xa8000 */
    int b;         /* +0x08 = 0 */
    int vel;       /* +0x0c = -old/4 */
    char _pad10[0x26];
    u16 half;      /* +0x36 (this+0x4696) = 0 */
    char _pad38;
    u8 state;      /* +0x39 (this+0x4699) = 4 */
    char _pad3a[3];
    u8 flag;       /* +0x3d (this+0x469d) = 0 */
    char _pad3e[2];
};

struct Objd6c {
    char _pad[0x4660];
    struct Entryd6c entries[1];  /* 0x4660 */
};

extern "C" {

/* --- shared ov004, main and library helpers --- */
extern void func_ov004_020afdd0(void *a0, int a1, int a2, int a3, int a4);
extern void func_ov004_020aff38(int a, int b, int c, int d, int e, int f, int g);
extern void func_ov004_020b2444(int a, int b, int c, int d, int e, int f, int g);
extern int  func_ov004_020adbc0(void);
extern int  func_ov004_020adbe0(void);
extern int  func_ov004_020adc1c(void);
extern void func_ov004_020b19f0(void);
extern void func_ov004_020b1a5c(int, int);
extern void func_ov004_020b1e44(int a0);
extern void func_ov004_020adb1c(int self);
extern void func_ov004_020af2f8(char *, char, int, int);
extern void func_ov004_020b04d0(int);
extern void func_ov004_020b0a54(int);
extern void FreeGfxSlotsById(int n);
extern int  RandomIntInternal(int *seed);
extern int  Sound_PlayIfNotActive(int a, int b, int c, int d);
extern int  func_020126e8(int a);
extern int  func_02012468(int a, int b, int c, int d, int e, int f, int g, short h);
extern void func_02012718(int, int);
extern int  _ZN4cstd4sqrtEy(u64 x);
extern s16  _ZN4cstd5atan2E5Fix12IiES1_(s32 y, s32 x);
extern int  LoadFile(int handle);
extern void Deallocate(void*);
extern void DecompressLZ16(int src, void *dst);
extern void MultiStore16(unsigned short val, char *dst, int nbytes);
extern int  func_02054d88(void);
extern void func_02056314(void*, u32, u32);
extern void func_020562b4(const void*, u32, u32);
extern void func_020564f4(const void*, int, int);
extern void _ZN2GX10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN2GX11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN3GXS10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN3GXS11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern unsigned _ZN3G2S13GetBG2CharPtrEv(void);
extern unsigned _ZN3G2S12GetBG2ScrPtrEv(void);
extern unsigned _ZN3G2S13GetBG3CharPtrEv(void);
extern void *_ZN11dScMgBase_cC2Ev(void*);

/* --- ROM data this TU reads --- */
extern int  data_0209d4b8;
extern u8   data_0209d45c;
extern u8   data_0209d454;
extern s16  data_02082214[];
extern u8   data_020a0e40;
extern u8   data_020a0de8[];
extern u8   data_020a0de9[];
extern u8   data_020a0dea[];
extern u8   data_020a0deb[];
extern int  data_ov006_02136b80[];
extern int  data_ov006_02136bd4[];
extern int  data_ov006_0213386c;
extern int  data_ov006_021389ec;
extern int  data_ov006_02138d08[];
extern u8   data_ov006_0212ebac[];
extern u8   data_ov006_0212ebb0[];
extern unsigned char data_ov006_0212ebb4[];
extern unsigned char data_ov006_0212ebb8[];
extern u8   data_ov006_0212ebc0[];
extern u8   data_ov006_0212ebc8[];
extern unsigned char data_ov006_0212ebd8[];
extern unsigned char data_ov006_0212ebe0[];
extern int  data_ov006_0212ec08[];
extern int  data_ov006_0212ec30[];
extern int  data_ov006_0212ec80[];
extern unsigned short data_ov006_0212ecac[];
extern s32  data_ov006_0212ecbc[];
extern s32  data_ov006_0212ecd4[];
extern unsigned short *data_ov006_0213daa4[];
extern int *data_ov006_0213db6c[];
extern int *data_ov006_0213db84[];
extern int *data_ov006_0213db9c[];
extern int  data_ov006_0213dbbc[];

/* The three pointer-to-member tables the helpers dispatch through. */
extern Entry data_ov006_021426cc[];
extern Entry data_ov006_021426f4[];
extern PMF   data_ov006_02142734[];

/* This TU's own helpers, forward-declared for the callers above them. */
extern void func_ov006_020ff47c(struct Obj* o);
extern void func_ov006_020ff4ec(struct Obj4ec *self);
extern void func_ov006_020ff534(char *c, int k);
extern void func_ov006_020ff690(char *c, int n);
extern void func_ov006_020ff8c8(char *c, int i);
extern void func_ov006_020ffb54(char *c, int i);
extern void func_ov006_020ffde4(char *c, int k);
extern void func_ov006_020fff54(char *base, int i);
extern void func_ov006_020fff84(C *c);
extern void func_ov006_020fffec(char *c);
extern void func_ov006_02100058(unsigned char (*c)[0x20]);
extern void func_ov006_02100084(char *c);
extern void func_ov006_02100140(char *c);
extern void func_ov006_021001ac(char* p);
extern void func_ov006_02100278(char *c, int r1, int r2, int r3);
extern void func_ov006_02100314(char *c);
extern void func_ov006_02100380(char* c);
extern void func_ov006_02100408(char *c, int a2, int a3);
extern void func_ov006_02100488(char *c);
extern void func_ov006_021004c0(struct Obj4c0 *o);
extern void func_ov006_021004f4(char *c, int a);
extern void func_ov006_02100554(void *self);
extern void func_ov006_0210068c(struct Obj68c *o);
extern void func_ov006_021006f4(unsigned char* o);
extern void func_ov006_02100734(char* c, int idx);
extern void func_ov006_0210076c(char *c, int idx);
extern void func_ov006_021009b8(char *p, int i);
extern void func_ov006_02100b08(char *self, int idx);
extern void func_ov006_02100bac(char* self, int i);
extern void func_ov006_02100d90(char *base, int idx);
extern void func_ov006_02100e3c(struct Obje3c *obj, int idx);
extern void func_ov006_02100f7c(char *c, int i);
extern void func_ov006_02101088(char *p, int i);
extern void func_ov006_02101148(char *c, int i);
extern void func_ov006_02101224(char *c, int i);
extern void func_ov006_021012cc(char* p, int i);
extern void func_ov006_021016ec(char* p, int i);
extern void func_ov006_021019e0(char *c, int i);
extern void func_ov006_02101af0(char* p, int i);
extern void func_ov006_02101e88(char* p, int i);
extern void func_ov006_021020c4(char* p, int i);
extern void func_ov006_02102274(char *c, int idx);
extern void func_ov006_0210246c(C *self);
extern void func_ov006_021024e0(char *c);
extern void func_ov006_02102564(char (*c)[64]);
extern void func_ov006_0210258c(char* c);
extern void func_ov006_02102624(char* c);
extern void func_ov006_0210265c(char *c);
extern void func_ov006_02102718(char* c);
extern void func_ov006_021027e4(int a0, int a1, int a2, int a3);
extern void func_ov006_02102864(char *c);
extern void func_ov006_02102c3c(char *o, int x, int z, int d);
extern void func_ov006_02102d6c(struct Objd6c *self, int i);
extern void func_ov006_02102dbc(char (*c)[64]);
extern void func_ov006_02102de4(char *p);
extern void func_ov006_02102e8c(C *c);
extern void func_ov006_02102ef4(unsigned char *r0);
extern void func_ov006_02102f3c(char *base, int arg1);
extern void func_ov006_02102fe8(dScMgPachinko2_c *self, int i);
extern void func_ov006_02103360(char *c, int i);
extern void func_ov006_02103608(char *o, int i);
extern void func_ov006_0210371c(char *c, int i);
extern void func_ov006_02103870(char *c, int i);
extern void func_ov006_0210397c(unsigned char* base, int idx);
extern void func_ov006_02103994(char *c, int i);
extern void func_ov006_02103ac0(char *c);
extern void func_ov006_02103bfc(char *c);

}  /* extern "C" */

// @symbol _ZN16dScMgPachinko2_cD1Ev
// @symbol _ZN16dScMgPachinko2_cD0Ev
/* Both variants come from this one definition: D1 stores this class's
   vtable and calls dScMgBase_c's D2; D0 adds dScMgBase_c::operator delete. */
dScMgPachinko2_c::~dScMgPachinko2_c()
{
}

// @symbol func_ov006_020ff47c
void func_ov006_020ff47c(struct Obj* o) {
    int i;
    for (i = 0; i < 2; i++) {
        if (o->arr[i].flag) {
            func_ov004_020afdd0((void *)data_ov006_02136b80[o->arr[i].idx],
                                o->arr[i].x >> 12, o->arr[i].y >> 12, -1, 2);
        }
    }
}

// @symbol func_ov006_020ff4ec
/* Resets two 0x20-byte slot entries: flag=1, id=0xff, two counters=0. */
void func_ov006_020ff4ec(struct Obj4ec *self)
{
    int i;

    for (i = 0; i < 2; i++) {
        self->slots[i].active = 1;
        self->slots[i].id = 0xff;
        self->slots[i].count = 0;
        self->slots[i].timer = 0;
    }
}

// @symbol func_ov006_020ff534
#pragma push
#pragma opt_strength_reduction off
#pragma opt_common_subs off
void func_ov006_020ff534(char *c, int k)
{
    int found = 0;
    int i = 0;
    int *px = (int *)(c + k * 32 + 0x5620);
    int *py = (int *)(c + k * 32 + 0x5624);
    for (; i < 0x30; i++) {
        int dx, dy;
        if (*(unsigned char *)(c + i * 64 + 0x4698) == 0) continue;
        if (*(unsigned char *)(c + i * 64 + 0x4699) < 3) continue;
        dx = (*(int *)(c + i * 64 + 0x4660) - *px) >> 12;
        dy = (*(int *)(c + i * 64 + 0x4664) - *py) >> 12;
        if (dx < -0x18) continue;
        if (dx > 0x18) continue;
        if (dy < 0) continue;
        if (dy <= 0x40) { found++; break; }
    }
    if (found == 0) return;
    if ((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16 & 0x7fff) * 2) >> 15) {
        *(unsigned char *)(c + k * 32 + 0x5635) = 3;
        *(unsigned char *)(c + k * 32 + 0x5636) = 0;
        *(int *)(c + k * 32 + 0x5628) = 0x1800;
        *(unsigned char *)(c + k * 32 + 0x5637) = 0x10;
        *(unsigned char *)(c + k * 32 + 0x5638) = 0;
        *(unsigned short *)(c + k * 32 + 0x5630) = 0;
        *(unsigned char *)(c + k * 32 + 0x563a) = 1;
    } else {
        *(unsigned char *)(c + k * 32 + 0x5635) = 2;
        *(unsigned char *)(c + k * 32 + 0x5636) = 0;
        *(int *)(c + k * 32 + 0x5628) = -0x1800;
        *(unsigned char *)(c + k * 32 + 0x5637) = 0x10;
        *(unsigned char *)(c + k * 32 + 0x5638) = 0;
        *(unsigned short *)(c + k * 32 + 0x5630) = 0;
        *(unsigned char *)(c + k * 32 + 0x563a) = 0;
    }
}
#pragma pop

// @symbol func_ov006_020ff690
#pragma push
#pragma opt_common_subs off
void func_ov006_020ff690(char *c, int n)
{
    unsigned int r;
    int flag;

    flag = 0;
    r = (unsigned int)RandomIntInternal(&data_0209d4b8);
    if ((((r >> 16) & 0x7fff) << 1) >> 0xf)
    {
        flag = 1;
        r = (unsigned int)RandomIntInternal(&data_0209d4b8);
        if ((((r >> 16) & 0x7fff) << 2) >> 0xf)
        {
            *(char *)(c + (n << 5) + 0x5000 + 0x635) = 3;
            *(int *)(c + (n << 5) + 0x5000 + 0x628) = 0x1800;
        }
        else
        {
            *(char *)(c + (n << 5) + 0x5000 + 0x635) = 2;
            *(int *)(c + (n << 5) + 0x5000 + 0x628) = -0x1800;
        }
        r = (unsigned int)RandomIntInternal(&data_0209d4b8);
        *(char *)(c + (n << 5) + 0x5000 + 0x63c) = (char)((((r >> 16) & 0x7fff) * 6 >> 0xf) + 1);
    }
    else
    {
        *(char *)(c + (n << 5) + 0x5000 + 0x635) = 1;
        r = (unsigned int)RandomIntInternal(&data_0209d4b8);
        *(char *)(c + (n << 5) + 0x5000 + 0x63c) = (char)((((r >> 16) & 0x7fff) << 1) >> 0xf);
    }

    {
        unsigned int state = *(unsigned char *)(c + (n << 5) + 0x5000 + 0x63d);
        if (state != 0xff)
        {
            if (flag == state)
            {
                *(unsigned char *)(c + 0x563e + (n << 5)) += 1;
                if (*(unsigned char *)(c + (n << 5) + 0x5000 + 0x63e) >= 2)
                {
                    if (flag != 0)
                    {
                        *(char *)(c + (n << 5) + 0x5000 + 0x635) = 1;
                        r = (unsigned int)RandomIntInternal(&data_0209d4b8);
                        *(char *)(c + (n << 5) + 0x5000 + 0x63c) = (char)((((r >> 16) & 0x7fff) << 1) >> 0xf);
                        flag = 0;
                    }
                    else
                    {
                        r = (unsigned int)RandomIntInternal(&data_0209d4b8);
                        if ((((r >> 16) & 0x7fff) << 1) >> 0xf)
                        {
                            *(char *)(c + (n << 5) + 0x5000 + 0x635) = 3;
                            *(int *)(c + (n << 5) + 0x5000 + 0x628) = 0x1800;
                        }
                        else
                        {
                            *(char *)(c + (n << 5) + 0x5000 + 0x635) = 2;
                            *(int *)(c + (n << 5) + 0x5000 + 0x628) = -0x1800;
                        }
                        r = (unsigned int)RandomIntInternal(&data_0209d4b8);
                        *(char *)(c + (n << 5) + 0x5000 + 0x63c) = (char)((((r >> 16) & 0x7fff) * 6 >> 0xf) + 1);
                        flag = 1;
                    }
                }
            }
            else
            {
                *(char *)(c + (n << 5) + 0x5000 + 0x63e) = 0;
            }
        }
    }

    *(char *)(c + (n << 5) + 0x5000 + 0x63d) = (char)flag;
    *(char *)(c + (n << 5) + 0x5000 + 0x636) = 0;
    *(char *)(c + (n << 5) + 0x5000 + 0x637) = 0x10;
    *(char *)(c + (n << 5) + 0x5000 + 0x638) = 0;
    *(short *)(c + (n << 5) + 0x5600 + 0x30) = 0;
    *(char *)(c + (n << 5) + 0x5000 + 0x63a) = 1;
}
#pragma pop

// @symbol func_ov006_020ff8c8
#pragma push
#pragma opt_common_subs off
void func_ov006_020ff8c8(char *c, int i)
{
    int v;

    if (*(u8 *)(c + (i << 5) + 0x5000 + 0x636) == 1) {
        (*(u16 *)(c + 0x5630 + (i << 5)))++;
        if (*(u16 *)(c + (i << 5) + 0x5630) < 4)
            return;
        *(u16 *)(c + (i << 5) + 0x5600 + 0x30) = 0;
        (*(u8 *)(c + 0x5638 + (i << 5)))++;
        if (*(u8 *)(c + (i << 5) + 0x5000 + 0x638) >= 3) {
            *(u8 *)(c + (i << 5) + 0x5000 + 0x638) = 0;
            *(u8 *)(c + (i << 5) + 0x5000 + 0x635) = 2;
            if (*(u8 *)(c + (i << 5) + 0x5000 + 0x63b) != 0) {
                if (*(u8 *)(c + (i << 5) + 0x5000 + 0x63c) != 0) {
                    *(u8 *)(c + (i << 5) + 0x5000 + 0x636) = 0;
                    (*(u8 *)(c + 0x563c + (i << 5)))--;
                    return;
                }
            }
            *(u8 *)(c + (i << 5) + 0x5000 + 0x636) = 2;
            return;
        }
        *(u8 *)(c + (i << 5) + 0x5000 + 0x637) =
            data_ov006_0212ebac[*(u8 *)(c + (i << 5) + 0x5000 + 0x638)];
        return;
    }

    (*(u16 *)(c + 0x5630 + (i << 5)))++;
    if (*(u16 *)(c + (i << 5) + 0x5630) >= 4) {
        *(u16 *)(c + (i << 5) + 0x5600 + 0x30) = 0;
        (*(u8 *)(c + 0x5638 + (i << 5)))++;
        if (*(u8 *)(c + (i << 5) + 0x5000 + 0x638) >= 6)
            *(u8 *)(c + (i << 5) + 0x5000 + 0x638) = 0;
        *(u8 *)(c + (i << 5) + 0x5000 + 0x637) =
            data_ov006_0212ebc8[*(u8 *)(c + (i << 5) + 0x5000 + 0x638)];
    }

    *(s32 *)(c + 0x5620 + (i << 5)) =
        *(s32 *)(c + 0x5620 + (i << 5)) +
        *(s32 *)(c + (i << 5) + 0x5000 + 0x628);
    v = *(s32 *)(c + (i << 5) + 0x5000 + 0x620) >> 0xc;

    if (*(u8 *)(c + (i << 5) + 0x5000 + 0x636) == 2) {
        if (v >= data_ov006_0212ecbc[i + 4]) {
            *(s32 *)(c + (i << 5) + 0x5000 + 0x620) =
                data_ov006_0212ecbc[i + 4] << 0xc;
            *(s32 *)(c + (i << 5) + 0x5000 + 0x628) = 0;
            *(u8 *)(c + (i << 5) + 0x5000 + 0x636) = 0;
            *(u8 *)(c + (i << 5) + 0x5000 + 0x635) = 0;
            return;
        }
    }

    if (v >= data_ov006_0212ecbc[i + 2]) {
        if (*(s32 *)(c + (i << 5) + 0x5000 + 0x628) >= 0x800) {
            *(s32 *)(c + 0x5628 + (i << 5)) =
                *(s32 *)(c + 0x5628 + (i << 5)) - 0x80;
        }
    } else {
        *(s32 *)(c + 0x5628 + (i << 5)) =
            *(s32 *)(c + 0x5628 + (i << 5)) + 0x80;
    }

    if (v < data_ov006_0212ecbc[i])
        return;

    *(s32 *)(c + (i << 5) + 0x5000 + 0x620) = data_ov006_0212ecbc[i] << 0xc;
    *(s32 *)(c + (i << 5) + 0x5000 + 0x628) = -0x1000;
    (*(u8 *)(c + 0x5636 + (i << 5)))++;
    *(u16 *)(c + (i << 5) + 0x5600 + 0x30) = 0;
    *(u8 *)(c + (i << 5) + 0x5000 + 0x638) = 0;
    *(u8 *)(c + (i << 5) + 0x5000 + 0x637) = data_ov006_0212ebac[0];
}
#pragma pop

// @symbol func_ov006_020ffb54
#pragma push
#pragma opt_common_subs off
void func_ov006_020ffb54(char *c, int i)
{
    int v;

    if (*(u8 *)(c + (i << 5) + 0x5000 + 0x636) == 1) {
        (*(u16 *)(c + 0x5630 + (i << 5)))++;
        if (*(u16 *)(c + (i << 5) + 0x5630) < 4)
            return;
        *(u16 *)(c + (i << 5) + 0x5600 + 0x30) = 0;
        (*(u8 *)(c + 0x5638 + (i << 5)))++;
        if (*(u8 *)(c + (i << 5) + 0x5000 + 0x638) >= 3) {
            *(u8 *)(c + (i << 5) + 0x5000 + 0x638) = 0;
            *(u8 *)(c + (i << 5) + 0x5000 + 0x635) = 3;
            if (*(u8 *)(c + (i << 5) + 0x5000 + 0x63b) != 0) {
                if (*(u8 *)(c + (i << 5) + 0x5000 + 0x63c) != 0) {
                    *(u8 *)(c + (i << 5) + 0x5000 + 0x636) = 0;
                    (*(u8 *)(c + 0x563c + (i << 5)))--;
                    return;
                }
            }
            *(u8 *)(c + (i << 5) + 0x5000 + 0x636) = 2;
            return;
        }
        *(u8 *)(c + (i << 5) + 0x5000 + 0x637) =
            data_ov006_0212ebb0[*(u8 *)(c + (i << 5) + 0x5000 + 0x638)];
        return;
    }

    (*(u16 *)(c + 0x5630 + (i << 5)))++;
    if (*(u16 *)(c + (i << 5) + 0x5630) >= 4) {
        *(u16 *)(c + (i << 5) + 0x5600 + 0x30) = 0;
        (*(u8 *)(c + 0x5638 + (i << 5)))++;
        if (*(u8 *)(c + (i << 5) + 0x5000 + 0x638) >= 6)
            *(u8 *)(c + (i << 5) + 0x5000 + 0x638) = 0;
        *(u8 *)(c + (i << 5) + 0x5000 + 0x637) =
            data_ov006_0212ebc0[*(u8 *)(c + (i << 5) + 0x5000 + 0x638)];
    }

    *(s32 *)(c + 0x5620 + (i << 5)) =
        *(s32 *)(c + 0x5620 + (i << 5)) +
        *(s32 *)(c + (i << 5) + 0x5000 + 0x628);
    v = *(s32 *)(c + (i << 5) + 0x5000 + 0x620) >> 0xc;

    if (*(u8 *)(c + (i << 5) + 0x5000 + 0x636) == 2) {
        if (v <= data_ov006_0212ecd4[i + 4]) {
            *(s32 *)(c + (i << 5) + 0x5000 + 0x620) =
                data_ov006_0212ecd4[i + 4] << 0xc;
            *(s32 *)(c + (i << 5) + 0x5000 + 0x628) = 0;
            *(u8 *)(c + (i << 5) + 0x5000 + 0x636) = 0;
            *(u8 *)(c + (i << 5) + 0x5000 + 0x635) = 0;
            return;
        }
    }

    if (v <= data_ov006_0212ecd4[i + 2]) {
        if (*(s32 *)(c + (i << 5) + 0x5000 + 0x628) <= -0x800) {
            *(s32 *)(c + 0x5628 + (i << 5)) =
                *(s32 *)(c + 0x5628 + (i << 5)) + 0x80;
        }
    } else {
        *(s32 *)(c + 0x5628 + (i << 5)) =
            *(s32 *)(c + 0x5628 + (i << 5)) - 0x80;
    }

    if (v > data_ov006_0212ecd4[i])
        return;

    *(s32 *)(c + (i << 5) + 0x5000 + 0x620) = data_ov006_0212ecd4[i] << 0xc;
    *(s32 *)(c + (i << 5) + 0x5000 + 0x628) = 0x1000;
    (*(u8 *)(c + 0x5636 + (i << 5)))++;
    *(u16 *)(c + (i << 5) + 0x5600 + 0x30) = 0;
    *(u8 *)(c + (i << 5) + 0x5000 + 0x638) = 0;
    *(u8 *)(c + (i << 5) + 0x5000 + 0x637) = data_ov006_0212ebb0[0];
}
#pragma pop

// @symbol func_ov006_020ffde4
#pragma push
#pragma opt_common_subs off
void func_ov006_020ffde4(char *c, int k)
{
    if (*(unsigned char *)(c + k * 32 + 0x563b) != 0) {
        (*(unsigned short *)(c + 0x5630 + k * 32))++;
        if (*(unsigned short *)(c + k * 32 + 0x5630) >= data_ov006_0212ebe0[*(unsigned char *)(c + k * 32 + 0x5638)]) {
            *(unsigned short *)(c + k * 32 + 0x5630) = 0;
            (*(unsigned char *)(c + 0x5638 + k * 32))++;
            if (*(unsigned char *)(c + k * 32 + 0x5638) >= 8) {
                *(unsigned char *)(c + k * 32 + 0x5638) = 0;
                if (*(unsigned char *)(c + k * 32 + 0x563c) == 0) {
                    *(unsigned char *)(c + k * 32 + 0x5635) = 4;
                    *(unsigned char *)(c + k * 32 + 0x5637) = 0x10;
                    return;
                }
                (*(unsigned char *)(c + 0x563c + k * 32))--;
            }
        }
        *(unsigned char *)(c + k * 32 + 0x5637) = data_ov006_0212ebd8[*(unsigned char *)(c + k * 32 + 0x5638)];
    } else {
        (*(unsigned short *)(c + 0x5630 + k * 32))++;
        if ((unsigned int)*(unsigned short *)(c + k * 32 + 0x5630) >= (unsigned int)data_ov006_0212ecac[*(unsigned char *)(c + k * 32 + 0x5638)]) {
            *(unsigned short *)(c + k * 32 + 0x5630) = 0;
            (*(unsigned char *)(c + 0x5638 + k * 32))++;
            if (*(unsigned char *)(c + k * 32 + 0x5638) >= 7) {
                *(unsigned char *)(c + k * 32 + 0x5638) = 0;
            }
            *(unsigned char *)(c + k * 32 + 0x5637) = data_ov006_0213daa4[k][*(unsigned char *)(c + k * 32 + 0x5638)];
        }
        func_ov006_020ff534(c, k);
    }
}
#pragma pop

// @symbol func_ov006_020fff54
void func_ov006_020fff54(char *base, int i)
{
  char *p;
  *((base + (i << 5)) + 0x5637) = 0x10;
  *((unsigned char *) ((base + (i << 5)) + 0x5638)) = 0;
  *((short *) ((base + (i << 5)) + 0x5630)) = 0;
  *((unsigned char *) ((base + (i << 5)) + 0x5639)) = 1;
  *((unsigned char *) ((base + (i << 5)) + 0x5635)) = 1;
}

// @symbol func_ov006_020fff84
#pragma push
#pragma opt_strength_reduction off
void func_ov006_020fff84(C *c) {
    int i;
    for (i = 0; i < 2; i++) {
        char *b = (char *)c + (i << 5);
        if (*(unsigned char *)(b + 0x5634) != 0) {
            (c->*data_ov006_021426cc[*(unsigned char *)(b + 0x5635)].pmf)(i);
        }
    }
}
#pragma pop

// @symbol func_ov006_020fffec
#pragma push
#pragma opt_strength_reduction off
void func_ov006_020fffec(char *c){
    int i;
    for(i=0;i<2;i++){
        *(unsigned char*)(c+(i<<5)+0x5000+0x634) = 1;
        *(unsigned char*)(c+(i<<5)+0x5000+0x639) = 0;
        *(unsigned char*)(c+(i<<5)+0x5000+0x635) = 0;
        *(unsigned char*)(c+(i<<5)+0x5000+0x636) = 0;
        *(unsigned char*)(c+(i<<5)+0x5000+0x637) = 0;
        *(unsigned char*)(c+(i<<5)+0x5000+0x638) = 0;
        *(unsigned char*)(c+(i<<5)+0x5000+0x63a) = 0;
        *(int*)(c+(i<<5)+0x5000+0x620) = data_ov006_0212ec08[i] << 12;
        *(int*)(c+(i<<5)+0x5000+0x624) = 0x68000;
    }
}
#pragma pop

// @symbol func_ov006_02100058
void func_ov006_02100058(unsigned char (*c)[0x20])
{
    int i;
    for (i = 0; i < 2; i++) {
        c[i][0x5634] = 0;
        c[i][0x5639] = 0;
        c[i][0x563b] = 0;
    }
}

// @symbol func_ov006_02100084
void func_ov006_02100084(char *c)
{
    unsigned int a, b;
    if (*(int *)(c + 0xbc) != 0) {
        a = ((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 6) >> 0xf;
        if (*(unsigned int *)(c + 0x5668) == a) {
            b = (((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 5) >> 0xf) + 1;
            a = a + b;
            if ((int)a >= 6) a -= 6;
        }
        *(unsigned int *)(c + 0x5668) = a;
        return;
    }
    *(unsigned int *)(c + 0x5668) = ((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 6) >> 0xf;
}

// @symbol func_ov006_02100140
void func_ov006_02100140(char *c){
    int i;
    for(i=0;i<0x10;i++){
        if(*(unsigned char*)(c+0x5000+0x4b5) != 0){
            func_ov004_020b2444(
                *(int*)(c+0x5000+0x4a0) >> 12,
                *(int*)(c+0x5000+0x4a4) >> 12,
                *(unsigned char*)(c+0x5000+0x4b6),
                -1, -1, 0, 0);
        }
        c += 0x18;
    }
}

// @symbol func_ov006_021001ac
void func_ov006_021001ac(char* p)
{
    int i;
    for (i = 0; i < 0x10; i++, p += 0x18) {
        if (*(u8*)(p + 0x54b4) != 0) {
            if (*(u8*)(p + 0x54b7) == 0) {
                *(int*)(((int)p + 0x54a4)) += *(int*)(p + 0x54a8);
                *(int*)(((int)p + 0x54a8)) -= 0x200;
                if (*(u16*)(p + 0x54b0) == 0x30) {
                    *(u8*)(((int)p + 0x54b7)) += 1;
                }
            }
            if (*(u16*)(p + 0x54b0) != 0) {
                *(u16*)(((int)p + 0x54b0)) -= 1;
            } else {
                *(u8*)(p + 0x54b5) = 0;
                *(u8*)(p + 0x54b4) = 0;
                *(u16*)(p + 0x54b0) = 0;
            }
        }
    }
}

// @symbol func_ov006_02100278
void func_ov006_02100278(char *c, int r1, int r2, int r3)
{
    int i;
    char *p = c;
    for (i = 0; i < 0x10; i++, p += 0x18) {
        if (*(unsigned char *)(p + 0x54b4) == 0) {
            int off = i * 0x18;
            char *q = (char *)((int)c + off);
            *(unsigned char *)(q + 0x54b4) = 1;
            *(unsigned char *)(q + 0x54b5) = 1;
            if (r1 >= 0x80000)
                *(int *)(q + 0x54a0) = 0xa4000;
            else
                *(int *)(q + 0x54a0) = 0x5c000;
            {
                char *w = (char *)(int)(c + off);
                *(int *)(w + 0x54a4) = r2 + 0x48000;
                *(int *)(w + 0x54ac) = 0;
                *(int *)(w + 0x54a8) = 0;
                *(unsigned char *)(w + 0x54b6) = (unsigned char)r3;
                *(unsigned char *)(w + 0x54b7) = 0;
                *(short *)(w + 0x54b0) = 0x40;
            }
            return;
        }
    }
}

// @symbol func_ov006_02100314
void func_ov006_02100314(char *c){
    int i;
    for(i=0;i<0x10;i++){
        if(*(unsigned char*)(c+0x5000+0x334) != 0){
            func_ov004_020afdd0(
                (void *)data_ov006_02136bd4[*(unsigned char*)(c+0x5000+0x335)],
                *(int*)(c+0x5000+0x320) >> 12,
                *(int*)(c+0x5000+0x324) >> 12,
                -1, -1);
        }
        c += 0x18;
    }
}

// @symbol func_ov006_02100380
/* For each of 16 active entries (stride 0x18 array at 0x5330): every 8
 * ticks bump a stage byte; after 4 stages clear the entry. */
void func_ov006_02100380(char* c)
{
    int i;
    for (i = 0; i < 16; i++, c += 0x18) {
        View *v = (View*)c;
        if (v->active == 0)
            continue;
        (*(u16*)(c + 0x5330))++;
        if (v->timer < 8)
            continue;
        (*(u8*)(c + 0x5335))++;
        if (v->stage >= 4) {
            v->active = 0;
            v->stage = 0;
            v->f4 = 0;
        }
    }
}

// @symbol func_ov006_02100408
void func_ov006_02100408(char *c, int a2, int a3)
{
    int i = 0;
    char *p = c;
    do {
        if (*(unsigned char *)(p + 0x5336) == 0) {
            char *e = c + i * 0x18;
            *(int *)(e + 0x5320) = a2;
            *(int *)(e + 0x5324) = a3;
            *(unsigned char *)(e + 0x5336) = 1;
            *(unsigned short *)(e + 0x5330) = 0;
            *(int *)(e + 0x5328) = 0;
            *(int *)(e + 0x532c) = 0;
            *(unsigned char *)(e + 0x5334) = 1;
            *(unsigned char *)(e + 0x5335) = 0;
            return;
        }
        i++;
        p += 0x18;
    } while (i < 0x10);
}

// @symbol func_ov006_02100488
void func_ov006_02100488(char *c){
  if (*(int *)(c + 0x5660) < 2) return;
  func_ov004_020b1a5c(func_ov004_020adbc0(), 6);
}

// @symbol func_ov006_021004c0
void func_ov006_021004c0(struct Obj4c0 *o)
{
    if (o->f < 2)
        return;
    func_ov004_020adc1c();
    func_ov004_020b19f0();
}

// @symbol func_ov006_021004f4
void func_ov006_021004f4(char *c, int a)
{
    char *dst;
    volatile unsigned short v;
    *(int *)(c + 0x5660) = 3;
    *(short *)(c + 0x566e) = 0x40;
    dst = (char *)_ZN3G2S13GetBG2CharPtrEv();
    v = 0;
    MultiStore16(v, dst, 0x6000);
    *(unsigned char *)(c + 0x5677) = (unsigned char)a;
    func_ov006_02102dbc((char (*)[64])c);
}

// @symbol func_ov006_02100554
void func_ov006_02100554(void *self) {
    char *s = (char *)self;
    int count;
    int i;

    if (*(u8 *)(s + 0x5299) && *(u8 *)(s + 0x5294)) return;
    if (*(u8 *)(s + 0x52d9) && *(u8 *)(s + 0x52d4)) return;
    if (*(u8 *)(s + 0x5319) && *(u8 *)(s + 0x5314)) return;
    if (*(u8 *)(s + 0x5296) != 0xd && *(u8 *)(s + 0x5294)) return;
    if (*(u8 *)(s + 0x52d6) != 0xd && *(u8 *)(s + 0x52d4)) return;
    if (*(u8 *)(s + 0x5316) != 0xd && *(u8 *)(s + 0x5314)) return;

    count = 0;
    for (i = 0; i < 0x30; i++) {
        if (((u8 (*)[0x40])(s + 0x4698))[i][0]) {
            count++;
            break;
        }
    }
    if (count) return;

    if (func_ov004_020adbe0())
        func_ov006_021004f4((char *)self, 1);
    else
        func_ov006_021004f4((char *)self, 0);
    func_ov006_021006f4((unsigned char *)self);
}

// @symbol func_ov006_0210068c
void func_ov006_0210068c(struct Obj68c *o){
    int i;
    for(i=0;i<3;i++){
        if(o->arr[i].flag){
            func_ov004_020afdd0(
                (void *)data_ov006_0213386c,
                o->arr[i].x >> 12,
                o->arr[i].y >> 12,
                -1, -1);
        }
    }
}

// @symbol func_ov006_021006f4
#pragma push
#pragma opt_strength_reduction off
void func_ov006_021006f4(unsigned char* o){
  int i;
  for(i=0;i<3;i++){
    unsigned char* b = o + (i<<6);
    if(b[0x5000+0x294] != 0){
      b[0x5000+0x296] = 0xe;
      *(unsigned short*)(b + 0x5200 + 0x92) = 0x88;
    }
  }
}
#pragma pop

// @symbol func_ov006_02100734
void func_ov006_02100734(char* c, int idx){
    char* e = c + idx * 0x40;
    if (*(unsigned char*)(e + 0x5294) == 0) return;
    *(unsigned short*)(e + 0x5290) = 0;
    *(int*)(e + 0x5270) = 0;
    *(unsigned short*)(e + 0x5292) = 0x40;
    *(unsigned char*)(e + 0x5296) = 0xc;
}

// @symbol func_ov006_0210076c
#pragma push
#pragma opt_strength_reduction off
#pragma opt_common_subs off
void func_ov006_0210076c(char *c, int idx)
{
    int i;
    u8 (*arr)[0x40];
    u8 *slot;
    u8 st;
    int *px;
    int *py;
    int k1800;
    int kn1800;
    u8 *pcnt;
    int k2800;
    int kn2800;
    int k2000;
    int kn2000;
    int *pvx;
    u8 *row;
    u8 *pflag;
    u8 *base4k;
    int *p4664;
    int dx;
    int dy;
    int *p466c;
    int *pxrow;
    int z;

    arr = (u8 (*)[0x40])c;
    slot = arr[idx];
    st = slot[0x5296];
    if (st == 0)
        return;
    if (st >= 0xc)
        return;

    px = (int *)(slot + 0x5260);
    py = (int *)(slot + 0x5264);
    k1800 = 0x1800;
    kn1800 = -k1800;
    pcnt = slot + 0x5299;
    k2800 = 0x2800;
    kn2800 = -k2800;
    k2000 = 0x2000;
    kn2000 = -k2000;
    i = 0;
    pvx = (int *)(slot + 0x5268);
    z = i;

    do {
        row = arr[i];
        pflag = row + 0x4698;
        if (*pflag != 0) {
            base4k = row + 0x4000;
            if (base4k[0x699] >= 2) {
                p466c = (int *)(row + 0x466c);
                if (*p466c > 0x2000) {
                    dx = (*px - *(int *)(row + 0x4660)) >> 12;
                    p4664 = (int *)(row + 0x4664);
                    dy = (*py - *p4664) >> 12;
                    if (dx >= -0x30 && dx <= 0x30 && dy >= 0 && dy <= 8) {
                        if (dx >= -0x10 && dx <= 0x10) {
                            *pcnt = (u8)(*pcnt + 1);
                            row[0x469a] = (u8)z;
                            pxrow = (int *)(row + 0x4660);
                            *pflag = (u8)z;
                            func_ov006_02100408(c, *pxrow, *p4664);
                            func_02012718(0x19a, *pxrow);
                            func_ov006_02101148(c, idx);
                        } else if (dx >= -0x20 && dx <= 0x20) {
                            if (dx < 0)
                                *(int *)(arr[i] + 0x4668) = *pvx - 0x1800;
                            else
                                *(int *)(arr[i] + 0x4668) = *pvx + 0x1800;
                            *p466c = kn2000;
                            Sound::PlayBank2_2D(0x19b);
                        } else {
                            if (dx < 0)
                                *(int *)(arr[i] + 0x4668) = k1800;
                            else
                                *(int *)(arr[i] + 0x4668) = kn1800;
                            *p466c = kn2800;
                            Sound::PlayBank2_2D(0x19b);
                        }
                    }
                }
            }
        }
        i++;
    } while (i < 0x30);
}
#pragma pop

// @symbol func_ov006_021009b8
#pragma push
#pragma opt_common_subs off
void func_ov006_021009b8(char *p, int i)
{
    int v;
    u8 state;

    if (*(u16 *)(p + 0x5200 + (i << 6) + 0x92) != 0) {
        (*(u16 *)(p + 0x5292 + (i << 6)))--;
        if (*(short *)(p + 0x5200 + (i << 6) + 0x92) > 0)
            return;
        *(int *)(p + 0x5000 + (i << 6) + 0x26c) = 0;
        *(int *)(p + 0x5000 + (i << 6) + 0x268) = data_ov006_0212ec80[i];
        *(u8 *)(p + 0x5000 + (i << 6) + 0x298) = i;
        return;
    }

    *(int *)(p + 0x5260 + (i << 6)) += *(int *)(p + 0x5000 + (i << 6) + 0x268);
    *(int *)(p + 0x5264 + (i << 6)) += *(int *)(p + 0x5000 + (i << 6) + 0x26c);
    *(int *)(p + 0x526c + (i << 6)) -= 0x20;
    state = *(u8 *)(p + 0x5000 + (i << 6) + 0x298);
    if (state == 0) {
        if (*(int *)(p + 0x5000 + (i << 6) + 0x268) <= 0xc00)
            *(int *)(p + 0x5268 + (i << 6)) += 0x80;
    } else if (state == 1) {
        if (*(int *)(p + 0x5000 + (i << 6) + 0x268) >= -0xc00)
            *(int *)(p + 0x5268 + (i << 6)) -= 0x80;
    }

    v = *(int *)(p + 0x5000 + (i << 6) + 0x264) >> 12;
    if (v <= -0x120) {
        *(u8 *)(p + 0x5000 + (i << 6) + 0x294) = 0;
        *(u8 *)(p + 0x5000 + (i << 6) + 0x295) = 0;
    }
}
#pragma pop

// @symbol func_ov006_02100b08
void func_ov006_02100b08(char *self, int idx)
{
    char *base = self + idx * 64;

    if (*(unsigned short *)(base + 0x5292) != 0) {
        *(unsigned short *)(self + 0x5292 + idx * 64) -= 1;
        if (*(short *)(base + 0x5292) < 0) {
            *(unsigned short *)(base + 0x5292) = 0;
        }
        return;
    }
    if (*(unsigned char *)(base + 0x5299) != 0) {
        int x;
        int z;

        *(unsigned char *)(self + 0x5299 + idx * 64) -= 1;
        *(unsigned short *)(base + 0x5292) = 0x30;
        x = *(int *)(base + 0x5260) >> 12;
        z = *(int *)(base + 0x5264) >> 12;
        func_ov006_02102c3c(self, x, z, *(unsigned char *)(base + 0x5299) & 1);
    }
}

// @symbol func_ov006_02100bac
#pragma push
#pragma opt_common_subs off
void func_ov006_02100bac(char* self, int i)
{
    int b = i << 6;

    if (*(u16*)(self + 0x5292 + b) != 0) {
        (*(u16*)(self + 0x5292 + b))--;
        if (*(s16*)(self + b + 0x5292) < 0)
            *(s16*)(self + b + 0x5292) = 0;
        return;
    }

    {
        int r4 = data_ov006_0213db9c[*(int*)(self + 0x5668)][i];
        int ax = 0x80 - (*(int*)(self + (i << 6) + 0x5260) >> 12);
        int y_raw = *(int*)(self + (i << 6) + 0x5264);

        y_raw = ax ? y_raw : y_raw;
        *(s16*)(self + (i << 6) + 0x5290) = (s16)_ZN4cstd5atan2E5Fix12IiES1_(
            r4 - (y_raw >> 12), ax);

        {
            s16 tv = data_02082214[((*(u16*)(self + (i << 6) + 0x5290) >> 4) << 1) + 1];
            int spd = *(int*)(self + (i << 6) + 0x5270);
            *(int*)(self + 0x5260 + (i << 6)) += (int)(((s64)tv * spd + 0x800) >> 0xc);
        }

        {
            s16 tv = data_02082214[(*(u16*)(self + (i << 6) + 0x5290) >> 4) << 1];
            int spd = *(int*)(self + (i << 6) + 0x5270);
            *(int*)(self + 0x5264 + (i << 6)) += (int)(((s64)tv * spd + 0x800) >> 0xc);
        }

        *(int*)(self + 0x5270 + (i << 6)) += 0x100;

        {
            int dx = (*(int*)(self + (i << 6) + 0x5260) >> 12) - 0x80;
            int dy = (*(int*)(self + (i << 6) + 0x5264) >> 12) - r4;
            if (dx < -3)
                return;
            if (dx > 3)
                return;
            if (dy < -3)
                return;
            if (dy > 3)
                return;
        }

        *(int*)(self + (i << 6) + 0x5260) = 0x80000;
        *(int*)(self + (i << 6) + 0x5264) = r4 << 12;
        *(unsigned char*)(self + (i << 6) + 0x5296) = 0xd;
        func_ov006_020ff4ec((struct Obj4ec *)self);
        *(u16*)(self + (i << 6) + 0x5292) = 0x40;
        if (i & 1) {
            *(u16*)(self + 0x5292 + (i << 6)) += 0x20;
        }
    }
}
#pragma pop

// @symbol func_ov006_02100d90
void func_ov006_02100d90(char *base, int idx)
{
  char *r3 = base + (idx * 0x40);
  if ((*((unsigned short *) (r3 + 0x5292))) != 0)
  {
    unsigned short *h = (unsigned short *) ((base + 0x5292) + (idx * 0x40));
    *h = (*h) - 1;
    return;
  }
  char *q;
  *((int *) ((base + 0x5264) + (idx * 0x40))) = (*((int *) ((base + 0x5264) + (idx * 0x40)))) + (*((int *) ((r3 + 0x5000) + 0x26c)));
  *((int *) ((base + 0x526c) + (idx * 0x40))) = (*((int *) ((base + 0x526c) + (idx * 0x40)))) - 0x100;
  int a = *((int *) ((r3 + 0x5000) + 0x280));
  int b = *((int *) ((r3 + 0x5000) + 0x264));
  if (a < b)
  {
    return;
  }
  *((int *) ((r3 + 0x5000) + 0x264)) = a;
  *((unsigned char *) ((r3 + 0x5000) + (b = 0x296))) = *((unsigned char *) ((r3 + 0x5000) + 0x297));
  *((int *) ((r3 + 0x5000) + 0x268)) = *((int *) ((r3 + 0x5000) + 0x284));
  *((int *) ((r3 + 0x5000) + 0x26c)) = *((int *) ((r3 + 0x5000) + 0x288));
}

// @symbol func_ov006_02100e3c
void func_ov006_02100e3c(struct Obje3c *obj, int idx)
{
    obj->sub[idx].unk00 += obj->sub[idx].unk08;

    if (obj->sub[idx].unk3a == 0) {
        obj->sub[idx].unk08 += 0x200;
        if (obj->sub[idx].unk08 >= 0x1800) {
            obj->sub[idx].unk3a = 1;
        }
    } else if (obj->sub[idx].unk3a == 1) {
        obj->sub[idx].unk08 -= 0x200;
        if (obj->sub[idx].unk08 <= -0x1800) {
            obj->sub[idx].unk3a = 0;
        }
    }

    if (obj->sub[idx].unk32 != 0) {
        obj->sub[idx].unk32--;
        return;
    }

    if ((obj->sub[idx].unk00 - obj->sub[idx].unk1c) >> 12 < -2) {
        return;
    }
    if ((obj->sub[idx].unk00 - obj->sub[idx].unk1c) >> 12 > 2) {
        return;
    }

    obj->sub[idx].unk32 = 0x10;
    obj->sub[idx].unk0c = 0;
    obj->sub[idx].unk36 = 0xb;
    obj->sub[idx].unk00 = obj->sub[idx].unk1c;
    obj->sub[idx].unk08 = 0;
}

// @symbol func_ov006_02100f7c
#pragma push
#pragma opt_common_subs off
void func_ov006_02100f7c(char *c, int i)
{
    int d;
    *(int *)(c + 0x5264 + (i << 6)) += *(int *)(c + 0x5000 + (i << 6) + 0x26c);
    d = *(int *)(c + 0x5000 + (i << 6) + 0x264) >> 12;
    if (*(int *)(c + 0x5000 + (i << 6) + 0x26c) <= 0x4000)
        *(int *)(c + 0x526c + (i << 6)) += 0x200;
    if (d < 0xa0)
        return;
    *(int *)(c + 0x5000 + (i << 6) + 0x264) = 0xa0000;
    *(unsigned char *)(c + 0x5000 + (i << 6) + 0x296) = 0xa;
    *(int *)(c + 0x5000 + (i << 6) + 0x26c) = 0;
    *(unsigned short *)(c + 0x5200 + (i << 6) + 0x92) = 0x40;
    if ((((((unsigned int)RandomIntInternal(&data_0209d4b8)) >> 16) & 0x7fff) << 1) >> 15) {
        *(int *)(c + 0x5000 + (i << 6) + 0x268) = 0x1000;
        *(unsigned char *)(c + 0x5000 + (i << 6) + 0x29a) = 0;
    } else {
        *(int *)(c + 0x5000 + (i << 6) + 0x268) = -0x1000;
        *(unsigned char *)(c + 0x5000 + (i << 6) + 0x29a) = 1;
    }
}
#pragma pop

// @symbol func_ov006_02101088
#pragma push
#pragma opt_common_subs off
void func_ov006_02101088(char *p, int i)
{
    int d;
    *(int *)(p + 0x5264 + (i << 6)) += *(int *)(p + 0x5000 + (i << 6) + 0x26c);
    d = (*(int *)(p + 0x5000 + (i << 6) + 0x264) - *(int *)(p + 0x5000 + (i << 6) + 0x280)) >> 12;
    if (d >= 0x40) {
        if (*(int *)(p + 0x5000 + (i << 6) + 0x26c) >= 0x1000)
            *(int *)(p + 0x526c + (i << 6)) -= 0x100;
    } else if (*(int *)(p + 0x5000 + (i << 6) + 0x26c) <= 0x4000) {
        *(int *)(p + 0x526c + (i << 6)) += 0x200;
    }
    if (d < 0x60) return;
    *(unsigned short *)(p + 0x5200 + (i << 6) + 0x92) = 0x10;
    *(int *)(p + 0x5000 + (i << 6) + 0x26c) = 0;
    *(unsigned char *)(p + 0x5000 + (i << 6) + 0x296) = 0xb;
}
#pragma pop

// @symbol func_ov006_02101148
#pragma push
#pragma opt_common_subs off
void func_ov006_02101148(char *c, int i)
{
    if ((int)*(unsigned char*)(c + i * 0x40 + 0x5000 + 0x296) >= 8) return;

    *(unsigned char*)(c + 0x529c + i * 0x40) += 1;

    if (*(unsigned char*)(c + i * 0x40 + 0x5000 + 0x29c) < 4) return;

    *(int*)(c + i * 0x40 + 0x5000 + 0x27c) = *(int*)(c + i * 0x40 + 0x5000 + 0x260);
    *(int*)(c + i * 0x40 + 0x5000 + 0x280) = *(int*)(c + i * 0x40 + 0x5000 + 0x264);
    *(unsigned char*)(c + i * 0x40 + 0x5000 + 0x297) = *(unsigned char*)(c + i * 0x40 + 0x5000 + 0x296);
    *(int*)(c + i * 0x40 + 0x5000 + 0x284) = *(int*)(c + i * 0x40 + 0x5000 + 0x268);
    *(int*)(c + i * 0x40 + 0x5000 + 0x288) = *(int*)(c + i * 0x40 + 0x5000 + 0x26c);
    *(int*)(c + i * 0x40 + 0x5000 + 0x268) = 0;
    *(int*)(c + i * 0x40 + 0x5000 + 0x268) = 0;
    *(int*)(c + i * 0x40 + 0x5000 + 0x26c) = 0x2000;

    if (*(unsigned char*)(c + i * 0x40 + 0x5000 + 0x29d) != 0)
        *(unsigned char*)(c + i * 0x40 + 0x5000 + 0x296) = 9;
    else
        *(unsigned char*)(c + i * 0x40 + 0x5000 + 0x296) = 8;

    *(unsigned char*)(c + i * 0x40 + 0x5000 + 0x29c) = 0;

    *(unsigned char*)(c + 0x529d + i * 0x40) ^= 1;
}
#pragma pop

// @symbol func_ov006_02101224
#pragma push
#pragma opt_common_subs off
void func_ov006_02101224(char *c, int i)
{
    *(int *)(c + (i << 6) + 0x5000 + 0x28c) = Sound_PlayIfNotActive(*(int *)(c + (i << 6) + 0x5000 + 0x28c), 2, 0x198, 0);

    if (*(unsigned short *)(c + (i << 6) + 0x5200 + 0x92) != 0) {
        *(unsigned short *)(c + 0x5292 + (i << 6)) -= 1;
        if (*(short *)(c + (i << 6) + 0x5200 + 0x92) < 0)
            *(short *)(c + (i << 6) + 0x5200 + 0x92) = 0;
        return;
    }

    if (*(int *)(c + 0x5000 + 0x668) == 3)
        *(unsigned char *)(c + (i << 6) + 0x5000 + 0x296) = 3;
    else
        *(unsigned char *)(c + (i << 6) + 0x5000 + 0x296) = 2;
}
#pragma pop

// @symbol func_ov006_021012cc
#pragma push
#pragma opt_common_subs off
void func_ov006_021012cc(char* p, int i)
{
    int v1;
    int v2;
    u8 state;

    if (*(u16*)(p + 0x5200 + (i << 6) + 0x92) != 0) {
        (*(u16*)(p + 0x5292 + (i << 6)))--;
        return;
    }

    *(int*)(p + 0x5260 + (i << 6)) += *(int*)(p + 0x5000 + (i << 6) + 0x268);
    *(int*)(p + 0x5264 + (i << 6)) += *(int*)(p + 0x5000 + (i << 6) + 0x26c);
    v1 = *(int*)(p + 0x5000 + (i << 6) + 0x260) >> 12;
    state = *(u8*)(p + 0x5000 + (i << 6) + 0x29b);
    v2 = *(int*)(p + 0x5000 + (i << 6) + 0x264) >> 12;

    if (state == 0) {
        if (v1 >= 0xe0) {
            *(u16*)(p + 0x5200 + (i << 6) + 0x92) = 0x40;
            (*(u8*)(p + 0x529b + (i << 6)))++;
            *(int*)(p + 0x5000 + (i << 6) + 0x268) = 0;
            *(int*)(p + 0x5000 + (i << 6) + 0x26c) = 0;
            return;
        }
        if (v1 >= 0x80) {
            if (*(int*)(p + 0x5000 + (i << 6) + 0x268) >= 0x400)
                *(int*)(p + 0x5268 + (i << 6)) -= 8;
            if (i == 0) {
                if (*(int*)(p + 0x5000 + (i << 6) + 0x26c) <= 0xc00)
                    *(int*)(p + 0x526c + (i << 6)) += 0x10;
            } else {
                if (*(int*)(p + 0x5000 + (i << 6) + 0x26c) >= -0xc00)
                    *(int*)(p + 0x526c + (i << 6)) -= 0x10;
            }
        } else {
            if (*(int*)(p + 0x5000 + (i << 6) + 0x268) <= 0xc00)
                *(int*)(p + 0x5268 + (i << 6)) += 0x20;
            if (i == 0) {
                if (v2 != data_ov006_0212ec30[i]) {
                    if (*(int*)(p + 0x5000 + (i << 6) + 0x26c) >= -0xc00)
                        *(int*)(p + 0x526c + (i << 6)) -= 0x30;
                } else {
                    *(int*)(p + 0x5000 + (i << 6) + 0x26c) = 0;
                }
            } else {
                if (v2 != data_ov006_0212ec30[i]) {
                    if (*(int*)(p + 0x5000 + (i << 6) + 0x26c) <= 0xc00)
                        *(int*)(p + 0x526c + (i << 6)) += 0x30;
                } else {
                    *(int*)(p + 0x5000 + (i << 6) + 0x26c) = 0;
                }
            }
        }
    } else if (state == 1) {
        if (v1 <= 0x20) {
            *(u16*)(p + 0x5200 + (i << 6) + 0x92) = 0x40;
            *(u8*)(p + 0x5000 + (i << 6) + 0x29b) = 0;
            *(int*)(p + 0x5000 + (i << 6) + 0x268) = 0;
            *(int*)(p + 0x5000 + (i << 6) + 0x26c) = 0;
            return;
        }
        if (v1 <= 0x80) {
            if (*(int*)(p + 0x5000 + (i << 6) + 0x268) <= -0x400)
                *(int*)(p + 0x5268 + (i << 6)) += 8;
            if (i == 0) {
                if (*(int*)(p + 0x5000 + (i << 6) + 0x26c) <= 0xc00)
                    *(int*)(p + 0x526c + (i << 6)) += 0x10;
            } else {
                if (*(int*)(p + 0x5000 + (i << 6) + 0x26c) >= -0xc00)
                    *(int*)(p + 0x526c + (i << 6)) -= 0x10;
            }
        } else {
            if (*(int*)(p + 0x5000 + (i << 6) + 0x268) >= -0xc00)
                *(int*)(p + 0x5268 + (i << 6)) -= 0x20;
            if (i == 0) {
                if (v2 != data_ov006_0212ec30[i]) {
                    if (*(int*)(p + 0x5000 + (i << 6) + 0x26c) >= -0xc00)
                        *(int*)(p + 0x526c + (i << 6)) -= 0x30;
                } else {
                    *(int*)(p + 0x5000 + (i << 6) + 0x26c) = 0;
                }
            } else {
                if (v2 != data_ov006_0212ec30[i]) {
                    if (*(int*)(p + 0x5000 + (i << 6) + 0x26c) <= 0xc00)
                        *(int*)(p + 0x526c + (i << 6)) += 0x30;
                } else {
                    *(int*)(p + 0x5000 + (i << 6) + 0x26c) = 0;
                }
            }
        }
    }

    *(int*)(p + 0x5000 + (i << 6) + 0x28c) = Sound_PlayIfNotActive(*(int*)(p + 0x5000 + (i << 6) + 0x28c), 2, 0x198, 0);
}
#pragma pop

// @symbol func_ov006_021016ec
#pragma push
#pragma opt_common_subs off
void func_ov006_021016ec(char* p, int i)
{
    int v;
    u8 flag;

    if (*(u16*)(p + (i << 6) + 0x5292) != 0) {
        *(u16*)(p + 0x5292 + (i << 6)) -= 1;
        return;
    }

    *(int*)(p + 0x5260 + (i << 6)) += *(int*)(p + 0x5000 + (i << 6) + 0x268);
    *(int*)(p + 0x5264 + (i << 6)) += *(int*)(p + 0x5000 + (i << 6) + 0x26c);
    flag = *(u8*)(p + 0x5000 + (i << 6) + 0x298);
    v = *(int*)(p + 0x5000 + (i << 6) + 0x260) >> 12;

    if (flag != 0) {
        if (v >= 0x80) {
            if (*(int*)(p + 0x5000 + (i << 6) + 0x268) >= 0x400)
                *(int*)(p + 0x5268 + (i << 6)) -= 8;
        } else {
            if (*(int*)(p + 0x5000 + (i << 6) + 0x268) <= 0x1000)
                *(int*)(p + 0x5268 + (i << 6)) += 0x80;
            if (*(u8*)(p + 0x5000 + (i << 6) + 0x29b) == 0) {
                if (*(int*)(p + 0x5000 + (i << 6) + 0x26c) <= 0x1000)
                    *(int*)(p + 0x526c + (i << 6)) += 0xc;
            } else {
                if (*(int*)(p + 0x5000 + (i << 6) + 0x26c) >= -0x1000)
                    *(int*)(p + 0x526c + (i << 6)) -= 0xc;
            }
        }
        if (v >= 0xe0) {
            *(int*)(p + 0x5000 + (i << 6) + 0x260) = 0xe0000;
            *(int*)(p + 0x5000 + (i << 6) + 0x268) = 0;
            *(int*)(p + 0x5000 + (i << 6) + 0x26c) = 0;
            *(u8*)(p + 0x5298 + (i << 6)) ^= 1;
            *(u8*)(p + 0x529b + (i << 6)) ^= 1;
            *(u16*)(p + (i << 6) + 0x5292) = 0x40;
        }
    } else {
        if (v <= 0x80) {
            if (*(int*)(p + 0x5000 + (i << 6) + 0x268) <= -0x400)
                *(int*)(p + 0x5268 + (i << 6)) += 8;
        } else {
            if (*(int*)(p + 0x5000 + (i << 6) + 0x268) >= -0x1000)
                *(int*)(p + 0x5268 + (i << 6)) -= 0x80;
            if (*(u8*)(p + 0x5000 + (i << 6) + 0x29b) == 0) {
                if (*(int*)(p + 0x5000 + (i << 6) + 0x26c) <= 0x1000)
                    *(int*)(p + 0x526c + (i << 6)) += 0xc;
            } else {
                if (*(int*)(p + 0x5000 + (i << 6) + 0x26c) >= -0x1000)
                    *(int*)(p + 0x526c + (i << 6)) -= 0xc;
            }
        }
        if (v <= 0x20) {
            *(int*)(p + 0x5000 + (i << 6) + 0x260) = 0x20000;
            *(int*)(p + 0x5000 + (i << 6) + 0x268) = 0;
            *(int*)(p + 0x5000 + (i << 6) + 0x26c) = 0;
            *(u8*)(p + 0x5298 + (i << 6)) ^= 1;
            *(u8*)(p + 0x529b + (i << 6)) ^= 1;
            *(u16*)(p + (i << 6) + 0x5292) = 0x40;
        }
    }

    *(int*)(p + 0x5000 + (i << 6) + 0x28c) = Sound_PlayIfNotActive(*(int*)(p + 0x5000 + (i << 6) + 0x28c), 2, 0x198, 0);
}
#pragma pop

// @symbol func_ov006_021019e0
#pragma push
#pragma opt_common_subs off
void func_ov006_021019e0(char *c, int i)
{
    {
        unsigned short angle = *(unsigned short *)(c + (i << 6) + 0x5200 + 0x90);
        long long t0 = (long long)data_02082214[(angle >> 4) * 2 + 1] * 0x50000 + 0x800;
        *(int *)(c + (i << 6) + 0x5000 + 0x260) = (int)(t0 >> 12) + 0x80000;
    }

    {
        unsigned short angle = *(unsigned short *)(c + (i << 6) + 0x5200 + 0x90);
        long long t1 = (long long)data_02082214[(angle >> 4) * 2] * 0x50000 + 0x800;
        *(int *)(c + (i << 6) + 0x5000 + 0x264) = (int)(t1 >> 12) - 0x80000;
    }

    if (*(unsigned char *)(c + (i << 6) + 0x5000 + 0x29b) != 0) {
        *(unsigned short *)(c + 0x5290 + (i << 6)) -= 0x40;
    } else {
        *(unsigned short *)(c + 0x5290 + (i << 6)) += 0x40;
    }

    *(int *)(c + (i << 6) + 0x5000 + 0x28c) = Sound_PlayIfNotActive(*(int *)(c + (i << 6) + 0x5000 + 0x28c), 2, 0x198, 0);
}
#pragma pop

// @symbol func_ov006_02101af0
#pragma push
#pragma opt_common_subs off
void func_ov006_02101af0(char* p, int i)
{
    int v;
    u8 state;

    if (*(u16*)(p + 0x5200 + (i << 6) + 0x92) != 0) {
        (*(u16*)(p + 0x5292 + (i << 6)))--;
        return;
    }

    state = *(u8*)(p + 0x5000 + (i << 6) + 0x29b);

    if (state == 0) {
        *(int*)(p + 0x5264 + (i << 6)) += *(int*)(p + 0x5000 + (i << 6) + 0x26c);
        v = *(int*)(p + 0x5000 + (i << 6) + 0x264) >> 12;
        if (v >= -0x60) {
            if (*(int*)(p + 0x5000 + (i << 6) + 0x26c) >= 0x400)
                *(int*)(p + 0x526c + (i << 6)) -= 8;
        } else {
            if (*(int*)(p + 0x5000 + (i << 6) + 0x26c) <= 0x1000)
                *(int*)(p + 0x526c + (i << 6)) += 0x80;
        }
        if (v >= -0x40) {
            *(int*)(p + 0x5000 + (i << 6) + 0x264) = -0x40000;
            *(int*)(p + 0x5000 + (i << 6) + 0x26c) = 0;
            *(int*)(p + 0x5000 + (i << 6) + 0x268) = 0;
            *(u16*)(p + 0x5200 + (i << 6) + 0x92) = 0;
            (*(u8*)(p + 0x529b + (i << 6)))++;
            return;
        }
    } else if (state == 1) {
        *(int*)(p + 0x5260 + (i << 6)) += *(int*)(p + 0x5000 + (i << 6) + 0x268);
        v = *(int*)(p + 0x5000 + (i << 6) + 0x260) >> 12;
        if (v >= 0x80) {
            if (*(int*)(p + 0x5000 + (i << 6) + 0x268) >= 0x400)
                *(int*)(p + 0x5268 + (i << 6)) -= 8;
        } else {
            if (*(int*)(p + 0x5000 + (i << 6) + 0x268) <= 0x1000)
                *(int*)(p + 0x5268 + (i << 6)) += 0x80;
        }
        if (v >= 0xe0) {
            *(int*)(p + 0x5000 + (i << 6) + 0x260) = 0xe0000;
            *(int*)(p + 0x5000 + (i << 6) + 0x26c) = 0;
            *(int*)(p + 0x5000 + (i << 6) + 0x268) = 0;
            *(u16*)(p + 0x5200 + (i << 6) + 0x92) = 0;
            (*(u8*)(p + 0x529b + (i << 6)))++;
            return;
        }
    } else if (state == 2) {
        *(int*)(p + 0x5264 + (i << 6)) += *(int*)(p + 0x5000 + (i << 6) + 0x26c);
        v = *(int*)(p + 0x5000 + (i << 6) + 0x264) >> 12;
        if (v <= -0xa0) {
            if (*(int*)(p + 0x5000 + (i << 6) + 0x26c) <= -0x400)
                *(int*)(p + 0x526c + (i << 6)) += 8;
        } else {
            if (*(int*)(p + 0x5000 + (i << 6) + 0x26c) >= -0x1000)
                *(int*)(p + 0x526c + (i << 6)) -= 0x80;
        }
        if (v <= -0xc0) {
            *(int*)(p + 0x5000 + (i << 6) + 0x264) = -0xc0000;
            *(int*)(p + 0x5000 + (i << 6) + 0x268) = 0;
            *(int*)(p + 0x5000 + (i << 6) + 0x26c) = 0;
            *(u16*)(p + 0x5200 + (i << 6) + 0x92) = 0;
            (*(u8*)(p + 0x529b + (i << 6)))++;
        }
    } else {
        *(int*)(p + 0x5260 + (i << 6)) += *(int*)(p + 0x5000 + (i << 6) + 0x268);
        v = *(int*)(p + 0x5000 + (i << 6) + 0x260) >> 12;
        if (v <= 0x80) {
            if (*(int*)(p + 0x5000 + (i << 6) + 0x268) <= -0x400)
                *(int*)(p + 0x5268 + (i << 6)) += 8;
        } else {
            if (*(int*)(p + 0x5000 + (i << 6) + 0x268) >= -0x1000)
                *(int*)(p + 0x5268 + (i << 6)) -= 0x80;
        }
        if (v <= 0x20) {
            *(int*)(p + 0x5000 + (i << 6) + 0x260) = 0x20000;
            *(int*)(p + 0x5000 + (i << 6) + 0x268) = 0;
            *(int*)(p + 0x5000 + (i << 6) + 0x26c) = 0;
            *(u16*)(p + 0x5200 + (i << 6) + 0x92) = 0;
            *(u8*)(p + 0x5000 + (i << 6) + 0x29b) = 0;
        }
    }

    *(int*)(p + 0x5000 + (i << 6) + 0x28c) = Sound_PlayIfNotActive(*(int*)(p + 0x5000 + (i << 6) + 0x28c), 2, 0x198, 0);
}
#pragma pop

// @symbol func_ov006_02101e88
#pragma push
#pragma opt_common_subs off
void func_ov006_02101e88(char* p, int i)
{
    int v;
    u8 flag;

    *(int*)(p + 0x5260 + (i << 6)) += *(int*)(p + 0x5000 + (i << 6) + 0x268);
    flag = *(u8*)(p + 0x5000 + (i << 6) + 0x298);
    v = *(int*)(p + 0x5000 + (i << 6) + 0x260) >> 12;

    if (flag != 0) {
        if (v >= 0x80) {
            if (*(int*)(p + 0x5000 + (i << 6) + 0x268) >= 0x400)
                *(int*)(p + 0x5268 + (i << 6)) -= 8;
        } else {
            int idx = *(int*)(p + 0x5668);
            if (*(int*)(p + 0x5000 + (i << 6) + 0x268) <= data_ov006_0213db84[idx][i])
                *(int*)(p + 0x5268 + (i << 6)) += 0x80;
        }
        if (v >= 0xe0) {
            u32 a;
            *(int*)(p + 0x5000 + (i << 6) + 0x260) = 0xe0000;
            *(int*)(p + 0x5000 + (i << 6) + 0x268) = 0;
            *(u8*)(p + 0x5298 + (i << 6)) ^= 1;
            a = ((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
            *(u16*)(p + (i << 6) + 0x5292) = (u16)(((a << 3) >> 15) * 3 + 0x40);
            *(u8*)(p + 0x5000 + (i << 6) + 0x296) = 7;
        }
    } else {
        if (v <= 0x80) {
            if (*(int*)(p + 0x5000 + (i << 6) + 0x268) <= -0x400)
                *(int*)(p + 0x5268 + (i << 6)) += 8;
        } else {
            int idx = *(int*)(p + 0x5668);
            if (*(int*)(p + 0x5000 + (i << 6) + 0x268) >= -data_ov006_0213db84[idx][i])
                *(int*)(p + 0x5268 + (i << 6)) -= 0x80;
        }
        if (v <= 0x20) {
            u32 a;
            *(int*)(p + 0x5000 + (i << 6) + 0x260) = 0x20000;
            *(int*)(p + 0x5000 + (i << 6) + 0x268) = 0;
            *(u8*)(p + 0x5298 + (i << 6)) ^= 1;
            a = ((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
            *(u16*)(p + (i << 6) + 0x5292) = (u16)(((a << 3) >> 15) * 3 + 0x40);
            *(u8*)(p + 0x5000 + (i << 6) + 0x296) = 7;
        }
    }

    *(int*)(p + 0x5000 + (i << 6) + 0x28c) = Sound_PlayIfNotActive(*(int*)(p + 0x5000 + (i << 6) + 0x28c), 2, 0x198, 0);
}
#pragma pop

// @symbol func_ov006_021020c4
#pragma push
#pragma opt_common_subs off
void func_ov006_021020c4(char* p, int i)
{
    int limit;

    *(int*)(p + 0x5264 + (i << 6)) += *(int*)(p + 0x5000 + (i << 6) + 0x26c);
    limit = data_ov006_0213db6c[*(int*)(p + 0x5668)][i];

    if ((*(int*)(p + 0x5000 + (i << 6) + 0x264) >> 12) >= limit) {
        *(int*)(p + 0x5000 + (i << 6) + 0x264) = limit << 12;
        *(int*)(p + 0x5000 + (i << 6) + 0x26c) = 0;

        if (i & 2) {
            if (*(int*)(p + 0x5000 + (i << 6) + 0x268) > 0)
                *(int*)(p + 0x5000 + (i << 6) + 0x268) = 0xc80;
            else
                *(int*)(p + 0x5000 + (i << 6) + 0x268) = -0xc80;
        }

        if (*(int*)(p + 0x5668) == 1) {
            *(u8*)(p + 0x5000 + (i << 6) + 0x296) = 4;
            if (*(int*)(p + 0x5000 + (i << 6) + 0x268) > 0)
                *(u16*)(p + 0x5000 + (i << 6) + 0x290) = 0x8000;
            else
                *(u16*)(p + 0x5000 + (i << 6) + 0x290) = 0;
        }

        if (*(int*)(p + 0x5668) == 3) {
            *(u8*)(p + 0x5000 + (i << 6) + 0x296) = 3;
            *(int*)(p + 0x5000 + (i << 6) + 0x268) = 0;
            if (*(u8*)(p + 0x5000 + (i << 6) + 0x298) != 0) {
                *(int*)(p + 0x5000 + (i << 6) + 0x26c) = 0xc00;
                *(u8*)(p + 0x5000 + (i << 6) + 0x29b) = 0;
            } else {
                *(int*)(p + 0x5000 + (i << 6) + 0x26c) = -0xc00;
                *(u8*)(p + 0x5000 + (i << 6) + 0x29b) = 2;
            }
            return;
        }

        if (*(int*)(p + 0x5668) == 4) {
            *(u8*)(p + 0x5000 + (i << 6) + 0x296) = 5;
            if (i == 0)
                *(u8*)(p + 0x5000 + (i << 6) + 0x29b) = 0;
            else
                *(u8*)(p + 0x5000 + (i << 6) + 0x29b) = 1;
        }

        if (*(int*)(p + 0x5668) == 5) {
            *(u8*)(p + 0x5000 + (i << 6) + 0x296) = 6;
            if (*(u8*)(p + 0x5000 + (i << 6) + 0x298) != 0)
                *(u8*)(p + 0x5000 + (i << 6) + 0x29b) = 0;
            else
                *(u8*)(p + 0x5000 + (i << 6) + 0x29b) = 1;
            return;
        }
    } else {
        *(int*)(p + 0x526c + (i << 6)) -= 0x20;
    }

    func_ov006_02101e88(p, i);
}
#pragma pop

// @symbol func_ov006_02102274
#pragma push
#pragma opt_common_subs off
void func_ov006_02102274(char *c, int idx)
{
    char *slot;
    char *ip;
    char *base;
    int bit;
    int r;
    int off;

    slot = c + (idx << 6);
    off = idx << 6;

    if (*(unsigned short *)(slot + 0x5292) != 0) {
        base = c + 0x5292;
        *(unsigned short *)(base + off) =
            (unsigned short)(*(unsigned short *)(base + off) - 1);

        if (*(short *)(slot + 0x5292) < 0)
            *(unsigned short *)(slot + 0x5292) = 0;

        return;
    }

    ip = slot + 0x5000;
    *(unsigned char *)(ip + 0x296) = 1;

    r = RandomIntInternal(&data_0209d4b8);
    bit = (int)(((unsigned int)r >> 16 & 0x7fffu) * 2u >> 15);

    if (idx & 1)
        bit = *(unsigned char *)(c + 0x5298) ^ 1;

    ip = c + (idx << 6);
    ip += 0x5000;
    *(unsigned char *)(ip + 0x298) = bit;

    if (bit != 0) {
        *(unsigned char *)(ip + 0x295) = 1;
        *(int *)(ip + 0x260) = -0x10000;
        *(int *)(ip + 0x268) = 0x200;

        if (*(int *)(c + 0x5668) == 1) {
            *(int *)(ip + 0x268) = 0xf00;

            if (idx) {
                *(unsigned char *)(ip + 0x29b) =
                    *(unsigned char *)(c + 0x529b);
            } else {
                r = RandomIntInternal(&data_0209d4b8);
                bit = (int)(((unsigned int)r >> 16 & 0x7fffu) * 2u >> 15);
                ip = c + (idx << 6);
                ip += 0x5000;
                *(unsigned char *)(ip + 0x29b) = bit;
            }
        }
    } else {
        *(unsigned char *)(ip + 0x295) = 1;
        *(int *)(ip + 0x260) = 0x110000;
        *(int *)(ip + 0x268) = -0x200;

        if (*(int *)(c + 0x5668) == 1) {
            *(int *)(ip + 0x268) = -0xf00;

            if (idx) {
                *(unsigned char *)(ip + 0x29b) =
                    *(unsigned char *)(c + 0x529b);
            } else {
                r = RandomIntInternal(&data_0209d4b8);
                bit = (int)(((unsigned int)r >> 16 & 0x7fffu) * 2u >> 15);
                ip = c + (idx << 6);
                ip += 0x5000;
                *(unsigned char *)(ip + 0x29b) = bit;
            }
        }
    }

    *(int *)(c + (idx << 6) + 0x5264) = -0xf8000;
    ip = c + (idx << 6);
    ip += 0x5000;
    *(unsigned char *)(ip + 0x299) = 0;
    *(int *)(ip + 0x26c) = 0x2000;

    r = func_020126e8(*(int *)(ip + 0x260));
    *(int *)(c + (idx << 6) + 0x528c) = func_02012468(
        *(int *)(c + (idx << 6) + 0x528c),
        2, 0x198, 4, 0, 0, r, 0);
}
#pragma pop

// @symbol func_ov006_0210246c
void func_ov006_0210246c(C *self)
{
    Row *rows = (Row *)self;
    for (int i = 0; i < 3; i++) {
        if (rows[i].d[0x5294]) {
            (self->*data_ov006_02142734[rows[i].d[0x5296]])(i);
            func_ov006_0210076c((char *)self, i);
        }
    }
}

// @symbol func_ov006_021024e0
#pragma push
#pragma opt_strength_reduction off
void func_ov006_021024e0(char *c) {
    int n = data_ov006_0212ebb8[*(int*)(c + 0x5000 + 0x668)];
    int i = 0;
    if (n <= 0) return;
    do {
        char *b = c + i * 0x40;
        *(unsigned char*)(b + 0x5294) = 1;
        *(unsigned char*)(b + 0x5296) = 0;
        *(unsigned char*)(b + 0x529b) = 0;
        *(short*)(b + 0x5292) = 0x20;
        *(int*)(b + 0x528c) = 0;
        *(int*)(b + 0x5274) = 0;
        *(int*)(b + 0x5278) = 0;
        *(unsigned char*)(b + 0x529c) = 0;
        *(unsigned char*)(b + 0x529d) = 0;
        i++;
    } while (i < n);
}
#pragma pop

// @symbol func_ov006_02102564
void func_ov006_02102564(char (*c)[64]){
  int i;
  for(i=0;i<3;i++){ c[i][0x5294]=0; c[i][0x5295]=0; }
}

// @symbol func_ov006_0210258c
void func_ov006_0210258c(char* c) {
  volatile unsigned short v;
  char* dst;
  if (*(unsigned short*)(c + 0x5674) != 0) return;
  func_ov006_02100734(c, 0);
  func_ov006_02100734(c, 1);
  func_ov006_02100734(c, 2);
  *(int*)(c + 0x5660) = 2;
  *(short*)(c + 0x566e) = 0x40;
  dst = (char*)_ZN3G2S13GetBG2CharPtrEv();
  v = 0;
  MultiStore16(v, dst, 0x6000);
  *(unsigned char*)(c + 0x5677) = 0;
  func_ov006_02102dbc((char (*)[64])c);
}

// @symbol func_ov006_02102624
void func_ov006_02102624(char* c){
    if (*(unsigned char*)(c + 0x567a) == 0) return;
    func_ov004_020b1e44(*(unsigned short*)(c + 0x5674));
}

// @symbol func_ov006_0210265c
void func_ov006_0210265c(char *c)
{
    unsigned char *q;
    unsigned short *h;
    unsigned short t;
    if (*(unsigned short *)(c + 0x5674) == 0) return;
    q = (unsigned char *)(((int)c + 0x5679));
    *q += 1;
    if (*(unsigned char *)(c + 0x5679) < 0x3c) return;
    *(unsigned char *)(c + 0x5679) = 0;
    h = (unsigned short *)(((int)c + 0x5674));
    *h -= 1;
    t = *(unsigned short *)(c + 0x5674);
    if (t > 0xa)
        Sound::PlayBank2_2D(0xa8);
    else if (t > 3)
        Sound::PlayBank2_2D(0xa7);
    else
        Sound::PlayBank2_2D(0xa6);
    if (*(unsigned short *)(c + 0x5674) == 0)
        *(unsigned char *)(c + 0x567a) = 0;
}

// @symbol func_ov006_02102718
void func_ov006_02102718(char* c)
{
    volatile unsigned short tmp;
    int n = *(unsigned char*)(c + 0x5676);
    int a, b;
    char* base;

    if (n == 0) return;

    base = c + (n - 1) * 0x40;
    a = 0x80 - (*(int*)(base + 0x4660) >> 12);
    b = 0x20 - (*(int*)(base + 0x4664) >> 12);

    if (a < -6) return;
    if (a > 6) return;
    if (b < -6) return;
    if (b > 6) return;
    if (*(unsigned char*)(base + 0x4699) != 2) return;

    *(unsigned char*)(c + 0x5676) = 0;
    {
        char* dst = (char*)_ZN3G2S13GetBG2CharPtrEv();
        tmp = 0;
        MultiStore16(tmp, dst, 0x6000);
    }
}

// @symbol func_ov006_021027e4
#pragma push
#pragma opt_loop_invariants off
void func_ov006_021027e4(int a0, int a1, int a2, int a3)
{
    int i, j, x, lo, hi, col, row, yb, s;
    u32 *bp;
    s = a1;
    for (i = 0; i < 2; i++)
    {
        x = a2 - 1 + i;
        lo = x & 7;
        hi = x >> 3;
        col = lo * 4;
        for (j = 0; j < 2; j++)
        {
            row = hi * 32;
            yb = s - 1;
            a1 = yb + j;
            bp = (u32 *)_ZN3G2S13GetBG2CharPtrEv();
            *(u32 *)((char *)(bp + (row + (a1 >> 3)) * 8) + col) |= a3 << ((a1 & 7) * 4);
        }
    }
}
#pragma pop

// @symbol func_ov006_02102864
#define LA(p) (*(int *)((((int)(p)) + 0x5664)))
#define LB(p) (*(int *)((((unsigned int)(p)) + 0x5664)))
#define LC(p) (*(int *)(((int)((p) + 0x5664))))
#define LD(p) (*(int *)(((unsigned int)((p) + 0x5664))))
#define LE(p) (*(int *)(((long long)((int)(p) + 0x5664))))
#define LF(p) (*(int *)(((long long)((unsigned int)(p) + 0x5664))))
#define LG(p) (*(int *)(((unsigned long long)((int)(p) + 0x5664))))
#define LH(p) (*(int *)(((unsigned long long)((unsigned int)(p) + 0x5664))))
#pragma push
#pragma opt_loop_invariants off
#pragma opt_dead_assignments off
void func_ov006_02102864(char *c)
{
    int k;                       /* sp+0x00 */
    int tx;                      /* sp+0x04 */
    int ty;                      /* sp+0x08 */
    int *p1;                     /* sp+0x0c */
    int *p2;                     /* sp+0x10 */
    int idx;                     /* sp+0x14 */
    int v2[2];                   /* sp+0x18..0x1c */
    int x1;                      /* sp+0x20 */
    volatile unsigned short tmp; /* sp+0x24 */
    int cx;                      /* sb */
    int cy;                      /* r8 */
    int dx;                      /* r7 */
    int dy;                      /* fp */
    int adx;                     /* r6 */
    int ady;                     /* r5 */
    int layer;                   /* r4 */
    int n;

    n = *(unsigned char *)(c + 0x5676);
    if (n == 0) return;
    idx = n - 1;
    if (*(int *)(c + idx * 0x40 + 0x4000 + 0x660) != *(int *)(c + idx * 0x40 + 0x4000 + 0x678)
     || *(int *)(c + idx * 0x40 + 0x4000 + 0x664) != *(int *)(c + idx * 0x40 + 0x4000 + 0x67c)) {
        char *dst = (char *)_ZN3G2S13GetBG2CharPtrEv();
        int i2;
        tmp = 0;
        MultiStore16(tmp, dst, 0x6000);

        i2 = (int)((long long)idx);
        p1 = (int *)(c + i2 * 0x40 + 0x4660);
        p2 = (int *)(c + i2 * 0x40 + 0x4664);
        k = 0;
        v2[0] = 0x6c;
        v2[1] = 0x22;
        layer = 2;
        x1 = 0x94;
        do {
            if (k == 0) {
                int v;
                cx = v2[0];
                v = *p2 >> 12;
                tx = *p1 >> 12;
                if (v >= 0x22) ty = v + 4; else ty = v - 4;
                cy = v2[1];
                tx = tx - 4;
            } else {
                int v;
                cx = x1;
                v = *p2 >> 12;
                tx = *p1 >> 12;
                if (v >= 0x22) ty = v + 4; else ty = v - 4;
                cy = v2[1];
                tx = tx + 4;
            }
            dx = tx - cx;
            adx = dx;
            if (dx < 0) adx = -dx;
            dy = ty - cy;
            ady = dy;
            if (dy < 0) ady = -dy;

            if (adx >= ady) {
                *(int *)(c + 0x5664) = adx / 2;
            LX:
                if (dx == 0) {
                    func_ov006_021027e4((int)c, cx, cy, layer);
                } else if (dx > 0) {
                    cx++;
                    LA(c) += ady;
                    if (*(int *)(c + 0x5664) > adx) {
                        if (dy >= 0) cy++; else cy--;
                        LB(c) -= adx;
                    }
                    func_ov006_021027e4((int)c, cx, cy, layer);
                    if (cx == tx) goto DONEK;
                    goto LX;
                } else {
                    cx--;
                    LC(c) += ady;
                    if (*(int *)(c + 0x5664) > adx) {
                        if (dy >= 0) cy++; else cy--;
                        LD(c) -= adx;
                    }
                    func_ov006_021027e4((int)c, cx, cy, layer);
                    if (cx == tx) goto DONEK;
                    goto LX;
                }
            } else {
                *(int *)(c + 0x5664) = ady / 2;
            LY:
                if (dy == 0) {
                    func_ov006_021027e4((int)c, cx, cy, layer);
                } else if (dy > 0) {
                    cy++;
                    LE(c) += adx;
                    if (*(int *)(c + 0x5664) > ady) {
                        if (dx >= 0) cx++; else cx--;
                        LF(c) -= ady;
                    }
                    func_ov006_021027e4((int)c, cx, cy, layer);
                    if (cy == ty) goto DONEK;
                    goto LY;
                } else {
                    cy--;
                    LG(c) += adx;
                    if (*(int *)(c + 0x5664) > ady) {
                        if (dx >= 0) cx++; else cx--;
                        LH(c) -= ady;
                    }
                    func_ov006_021027e4((int)c, cx, cy, layer);
                    if (cy != ty) goto LY;
                }
            }
        DONEK:
            k++;
        } while (k < 2);
    }
    {
        int i3 = (int)((unsigned long long)idx);
        *(int *)(c + i3 * 0x40 + 0x4000 + 0x678) = *(int *)(c + i3 * 0x40 + 0x4000 + 0x660);
        *(int *)(c + i3 * 0x40 + 0x4000 + 0x67c) = *(int *)(c + i3 * 0x40 + 0x4000 + 0x664);
    }
}
#pragma pop

// @symbol func_ov006_02102c3c
#pragma push
#pragma opt_strength_reduction off
#pragma opt_common_subs off
void func_ov006_02102c3c(char *o, int x, int z, int d)
{
    int i;
    for (i = 0; i < 0x30; i++) {
        char *q = o + i * 0x40;
        if (*(u8 *)(q + 0x4698) == 0) {
            *(u8 *)(q + 0x4698) = 1;
            *(u8 *)(q + 0x4699) = 7;
            *(u16 *)(q + 0x4696) = 0x40;
            *(u8 *)(q + 0x469a) = 1;
            *(u8 *)(q + 0x469c) = 0;
            if (d != 0)
                *(u16 *)(q + 0x4694) = 0xd000;
            else
                *(u16 *)(q + 0x4694) = 0xb000;
            *(int *)(o + i * 0x40 + 0x4660) = x << 12;
            *(int *)(o + i * 0x40 + 0x4664) = (z - 0x12) << 12;
            *(int *)(o + i * 0x40 + 0x4668) = (int)(((s64)data_02082214[((*(u16 *)(o + i * 0x40 + 0x4694)) >> 4) * 2 + 1] * 0x2000 + 0x800) >> 12);
            *(int *)(o + i * 0x40 + 0x466c) = (int)(((s64)data_02082214[((*(u16 *)(o + i * 0x40 + 0x4694)) >> 4) * 2] * 0x2000 + 0x800) >> 12);
            *(int *)(o + i * 0x40 + 0x4684) = 0;
            Sound::PlayBank2_2D(0x199);
            return;
        }
    }
}
#pragma pop

// @symbol func_ov006_02102d6c
/* Resets entry i of a 0x40-stride array at this+0x4660: reverses and
 * quarters its velocity, clears counters, and tail-calls func_02012718
 * (sound: id 0x19c) with the entry's handle. */
void func_ov006_02102d6c(struct Objd6c *self, int i)
{
    self->entries[i].state = 4;
    self->entries[i].vel = -self->entries[i].vel >> 2;
    self->entries[i].b = 0;
    self->entries[i].half = 0;
    self->entries[i].flag = 0;
    self->entries[i].a = 0xa8000;
    func_02012718(0x19c, self->entries[i].handle);
}

// @symbol func_ov006_02102dbc
void func_ov006_02102dbc(char (*c)[64]){
  int i;
  for(i=0;i<0x30;i++){ c[i][0x469a]=0; c[i][0x4698]=0; }
}

// @symbol func_ov006_02102de4
#pragma push
#pragma opt_strength_reduction off
void func_ov006_02102de4(char *p)
{
    int i;
    for (i = 0; i < 0x30; i++) {
        if (*(unsigned char *)(p + i * 64 + 0x469a) != 0) {
            int x = *(int *)(p + i * 64 + 0x4660) >> 12;
            unsigned char t = *(unsigned char *)(p + i * 64 + 0x469c);
            int y = *(int *)(p + i * 64 + 0x4664) >> 12;
            if (t != 0) {
                func_ov004_020afdd0((void *)data_ov006_02138d08[t - 1], x, y, -1, -1);
            } else {
                func_ov004_020aff38(data_ov006_021389ec, x, y, -1, -1, 0x1000,
                                    *(unsigned short *)(p + i * 64 + 0x4690));
            }
        }
    }
}
#pragma pop

// @symbol func_ov006_02102e8c
#pragma push
#pragma opt_strength_reduction off
void func_ov006_02102e8c(C *c) {
    int i;
    for (i = 0; i < 0x30; i++) {
        if (*(unsigned char *)((char *)c + i * 0x40 + 0x4698) != 0) {
            unsigned char k = *(unsigned char *)((char *)c + i * 0x40 + 0x4699);
            (c->*data_ov006_021426f4[k].pmf)(i);
        }
    }
}
#pragma pop

// @symbol func_ov006_02102ef4
void func_ov006_02102ef4(unsigned char *r0) {
    if (r0[0x5678] != 0) return;
    *(unsigned short *)(r0 + 0x5674) = 0x1e;
    r0[0x5679] = 0;
    r0[0x567a] = 1;
    {
        unsigned char *p = (unsigned char *)(r0 + 0x5678);
        *p += 1;
    }
}

// @symbol func_ov006_02102f3c
void func_ov006_02102f3c(char *base, int arg1)
{
    u8 idx;
    int off;
    int has;
    char *e;
    int a, b;

    idx = data_020a0e40;
    off = idx * 4;
    has = 0;
    if (data_020a0de8[off])
    {
        if (data_020a0de9[off])
            has = 1;
    }
    if (has != 0)
    {
        e = base + arg1 * 0x40 + 0x4000;
        a = (*(int *)(e + 0x660) >> 12) - data_020a0dea[idx * 4];
        b = (*(int *)(e + 0x664) >> 12) - data_020a0deb[idx * 4];
        *(u8 *)(e + 0x699) = 1;
        *(int *)(e + 0x670) = a << 12;
        *(int *)(e + 0x674) = b << 12;
        *(u8 *)(e + 0x69b) = 0;
        arg1 = (int)e;
    }
    ((void (*)(char *, int))func_ov006_02102864)(base, arg1);
}

// @symbol func_ov006_02102fe8
/* dScMgPachinko2_c ball i under a live pen record: while the pen is down the
   ball follows the pen (x/y = pen + held offset, clamped to the field, offset
   re-derived); on release it aims the ball at (0x80, 0x20) and launches it.
   Touch record reads: data_020a0dea (pen x) is a volatile byte read and
   data_020a0deb (pen y) is read through a plain u8 pointer. Measured under
   2004/b56: both volatile 4 div (ldrb/add and literal/ldr order), both plain
   arrays 18 div (the y read sinks past the x store), both pointer reads 7 div
   (r3/r6 colouring); only this pairing reproduces the ROM's literal hoisting
   and load order at both sites. */
void func_ov006_02102fe8(dScMgPachinko2_c *self, int i)
{
    int idx = data_020a0e40;
    int off = idx * 4;

    if (data_020a0de8[off] != 0) {
        int sx;
        int sy;
        int dx;
        int dy;
        int dist;
        int sq;
        int prev;
        int tx = ((volatile u8 *)data_020a0dea)[off];
        int ty = ((u8 *)data_020a0deb)[off];

        self->mBalls[i].x = self->mBalls[i].px + (tx << 12);
        self->mBalls[i].y = self->mBalls[i].py + (ty << 12);
        sx = self->mBalls[i].x >> 12;
        sy = self->mBalls[i].y >> 12;
        if (sx >= 0xf8) {
            self->mBalls[i].x = 0xf8000;
        }
        if (sx <= 8) {
            self->mBalls[i].x = 0x8000;
        }
        if (sy >= 0xb8) {
            self->mBalls[i].y = 0xb8000;
        }
        if (sy <= 8) {
            self->mBalls[i].y = 0x8000;
        }
        {
            int nx = (self->mBalls[i].x >> 12) - ((volatile u8 *)data_020a0dea)[data_020a0e40 * 4];
            int ny = (self->mBalls[i].y >> 12) - ((u8 *)data_020a0deb)[data_020a0e40 * 4];
            self->mBalls[i].px = nx << 12;
            self->mBalls[i].py = ny << 12;
        }

        dy = 0x80 - (self->mBalls[i].x >> 12);
        dx = 0x20 - (self->mBalls[i].y >> 12);
        dist = dy * dy + dx * dx;
        sq = _ZN4cstd4sqrtEy(dist);
        prev = self->mBalls[i].prevDist;
        self->mBalls[i].prevDist = sq;
        if (sq > prev) {
            if (sq > prev + 10) {
                self->mBalls[i].sound = Sound_PlayIfNotActive(self->mBalls[i].sound, 2, 0x17b, 0);
            } else {
                self->mBalls[i].sound = Sound_PlayIfNotActive(self->mBalls[i].sound, 2, 0x17c, 0);
            }
        }
    } else {
        int dy;
        int dx;
        int dist;
        int sq;

        self->mBalls[i].state = 2;
        dy = 0x80 - (self->mBalls[i].x >> 12);
        dx = 0x20 - (self->mBalls[i].y >> 12);
        dist = dy * dy + dx * dx;
        sq = _ZN4cstd4sqrtEy(dist);
        if (sq >= 0x10) {
            self->mBalls[i].angle = _ZN4cstd5atan2E5Fix12IiES1_(dx, dy);
            self->mBalls[i].speed = _ZN4cstd4sqrtEy(dist) << 8;
            self->mBalls[i].speed += self->mBalls[i].speed >> 3;
            self->mBalls[i].speed += 0x1800;
            if (self->mBalls[i].speed >= 0x9400) {
                self->mBalls[i].speed = 0x9400;
            }
            self->unk_566c = self->mBalls[i].speed >> 11;
            self->unk_566c += self->unk_566c >> 1;
            if (self->unk_566c == 0) {
                self->unk_566c = 1;
            }
            {
                s16 c = data_02082214[((u16)self->mBalls[i].angle >> 4) * 2 + 1];
                self->mBalls[i].vx = (s32)(((s64)c * self->mBalls[i].speed + 0x800) >> 12);
            }
            {
                s16 s = data_02082214[((u16)self->mBalls[i].angle >> 4) * 2];
                self->mBalls[i].vy = (s32)(((s64)s * self->mBalls[i].speed + 0x800) >> 12);
            }
            self->mBalls[i].unk_36 = 0;
            self->mBalls[i].unk_32 = self->mBalls[i].vx >> 2;
            if (sq >= 0x40) {
                Sound::PlayBank2_2D(0x17e);
            } else {
                Sound::PlayBank2_2D(0x17d);
            }
        } else {
            self->mBalls[i].x = 0x80000;
            self->mBalls[i].y = 0x28000;
            self->mBalls[i].state = 0;
        }
    }
    func_ov006_02102864((char *)self);
}

// @symbol func_ov006_02103360
#pragma push
#pragma opt_common_subs off
void func_ov006_02103360(char *c, int i)
{
    int idx;
    int velX, velY;
    int idx2;

    idx2 = (*(unsigned short*)(c + i * 0x40 + 0x4600 + 0x94) >> 4) * 2;

    if (data_02082214[idx2] <= 0) {
        {
            short tv = data_02082214[idx2 + 1];
            int spd = *(int*)(c + i * 0x40 + 0x4000 + 0x680);
            *(int*)(c + 0x4660 + i * 0x40) += (int)(((long long)tv * spd + 0x800) >> 12);
        }

        idx = *(unsigned short*)(c + i * 0x40 + 0x4600 + 0x94) >> 4;
        {
            short tv = data_02082214[idx * 2];
            int spd = *(int*)(c + i * 0x40 + 0x4000 + 0x680);
            *(int*)(c + 0x4664 + i * 0x40) += (int)(((long long)tv * spd + 0x800) >> 12);
        }

        *(int*)(c + 0x4684 + i * 0x40) += 0x10;

        *(int*)(c + 0x4680 + i * 0x40) -= *(int*)(c + i * 0x40 + 0x4000 + 0x684);

        if (*(int*)(c + i * 0x40 + 0x4000 + 0x684) >= 0x600) {
            *(int*)(c + i * 0x40 + 0x4000 + 0x684) = 0x600;
        }

        if (*(int*)(c + i * 0x40 + 0x4000 + 0x680) < 0) {
            *(unsigned char*)(c + i * 0x40 + 0x4000 + 0x699) = 3;
            *(int*)(c + i * 0x40 + 0x4000 + 0x680) = 0;
            *(int*)(c + i * 0x40 + 0x4000 + 0x684) = 0;
            *(int*)(c + i * 0x40 + 0x4000 + 0x66c) = 0;
            *(int*)(c + i * 0x40 + 0x4000 + 0x668) = 0;
        }
    } else {
        *(int*)(c + 0x4660 + i * 0x40) += *(int*)(c + i * 0x40 + 0x4000 + 0x668);
        *(int*)(c + 0x4664 + i * 0x40) += *(int*)(c + i * 0x40 + 0x4000 + 0x66c);
        *(int*)(c + 0x4684 + i * 0x40) += 0x10;
        *(int*)(c + 0x466c + i * 0x40) += *(int*)(c + i * 0x40 + 0x4000 + 0x684);

        {
            int v = *(int*)(c + i * 0x40 + 0x4000 + 0x668);
            int nv;
            if (v > 0) {
                nv = v - 0x200;
                if (nv <= 0) nv = 0;
            } else if (v < 0) {
                nv = v + 0x200;
                if (nv >= 0) nv = 0;
            } else {
                *(unsigned char*)(c + i * 0x40 + 0x4000 + 0x699) = 3;
                *(int*)(c + i * 0x40 + 0x4000 + 0x680) = 0;
                *(int*)(c + i * 0x40 + 0x4000 + 0x684) = 0;
                *(int*)(c + i * 0x40 + 0x4000 + 0x668) = 0;
                return;
            }
            *(int*)(c + i * 0x40 + 0x4000 + 0x668) = nv;
        }
    }

    {
        int rawX = *(int*)(c + i * 0x40 + 0x4000 + 0x660);
        int rawY = *(int*)(c + i * 0x40 + 0x4000 + 0x664);
        velY = rawY >> 12;
        *(unsigned short*)(c + 0x4690 + i * 0x40) += *(unsigned short*)(c + i * 0x40 + 0x4600 + 0x92);
        velX = rawX >> 12;
    }

    if (velY <= -0x140 || velY >= 0xd0) {
        *(unsigned char*)(c + i * 0x40 + 0x4000 + 0x699) = 6;
    }
    if (velX >= 0x140 || velX <= -0x40) {
        *(unsigned char*)(c + i * 0x40 + 0x4000 + 0x699) = 6;
    }

    func_ov006_02102718(c);
    if (*(unsigned char*)(c + 0x5676) != 0) {
        func_ov006_02102864(c);
    }
}
#pragma pop

// @symbol func_ov006_02103608
#pragma push
#pragma opt_common_subs off
void func_ov006_02103608(char *o, int i)
{
    int v660, v664, t660;
    *(int *)((char *)(((int)o + 0x4660)) + i * 0x40) += *(int *)(o + i * 0x40 + 0x4668);
    *(int *)((char *)(((int)o + 0x4664)) + i * 0x40) += *(int *)(o + i * 0x40 + 0x466c);
    *(int *)((char *)(((int)o + 0x4684)) + i * 0x40) += 0x20;
    *(int *)((char *)(((int)o + 0x466c)) + i * 0x40) += *(int *)(o + i * 0x40 + 0x4684);
    *(u16 *)((char *)(((int)o + 0x4690)) + i * 0x40) += *(u16 *)(o + i * 0x40 + 0x4692);
    if (*(int *)(o + i * 0x40 + 0x466c) >= 0x8000)
        *(int *)(o + i * 0x40 + 0x466c) = 0x8000;
    t660 = *(int *)(o + i * 0x40 + 0x4664);
    v660 = *(int *)(o + i * 0x40 + 0x4660) >> 12;
    v664 = t660 >> 12;
    if (v664 <= -0x140)
        *(u8 *)(o + i * 0x40 + 0x4699) = 6;
    if (v660 >= 0x140 || v660 <= -0x40)
        *(u8 *)(o + i * 0x40 + 0x4699) = 6;
    if (v664 >= 0xa8)
        func_ov006_02102d6c((struct Objd6c *)o, i);
}
#pragma pop

// @symbol func_ov006_0210371c
#pragma push
#pragma opt_common_subs off
void func_ov006_0210371c(char *c, int i)
{
    int old;
    int v;
    int w;

    *(int *)(c + 0x4664 + i * 0x40) += *(int *)(c + i * 0x40 + 0x4000 + 0x66c);
    old = *(int *)(c + i * 0x40 + 0x4000 + 0x66c);
    *(int *)(c + 0x466c + i * 0x40) += 0x200;

    if (old < 0 && *(int *)(c + i * 0x40 + 0x4000 + 0x66c) >= 0) {
        *(u16 *)(c + i * 0x40 + 0x4600 + 0x90) = 0;
        v = *(int *)(c + i * 0x40 + 0x4000 + 0x660) >> 12;
        if (v > 0x80) {
            *(u8 *)(c + i * 0x40 + 0x4000 + 0x69e) = 1;
            *(u8 *)(c + i * 0x40 + 0x4000 + 0x69c) = 5;
            *(int *)(c + i * 0x40 + 0x4000 + 0x668) = 0x1000;
        } else if (v < 0x80) {
            *(u8 *)(c + i * 0x40 + 0x4000 + 0x69e) = 0;
            *(u8 *)(c + i * 0x40 + 0x4000 + 0x69c) = 1;
            *(int *)(c + i * 0x40 + 0x4000 + 0x668) = -0x1000;
        } else {
            unsigned int r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
            if (((r << 1) >> 15) != 0) {
                *(u8 *)(c + i * 0x40 + 0x4000 + 0x69e) = 1;
                *(u8 *)(c + i * 0x40 + 0x4000 + 0x69c) = 5;
                *(int *)(c + i * 0x40 + 0x4000 + 0x668) = 0x1000;
            } else {
                *(u8 *)(c + i * 0x40 + 0x4000 + 0x69e) = 0;
                *(u8 *)(c + i * 0x40 + 0x4000 + 0x69c) = 1;
                *(int *)(c + i * 0x40 + 0x4000 + 0x668) = -0x1000;
            }
        }
    }

    w = *(int *)(c + i * 0x40 + 0x4000 + 0x664) >> 12;
    if (w >= 0xa8) {
        *(int *)(c + i * 0x40 + 0x4000 + 0x664) = 0xa8000;
        *(u8 *)(c + i * 0x40 + 0x4000 + 0x699) = 5;
    }
}
#pragma pop

// @symbol func_ov006_02103870
#pragma push
#pragma opt_common_subs off
void func_ov006_02103870(char *c, int i)
{
    *(unsigned short *)(c + 0x4696 + (i << 6)) += 1;

    if (*(unsigned short *)(c + (i << 6) + 0x4600 + 0x96) >= 4) {
        *(unsigned short *)(c + (i << 6) + 0x4600 + 0x96) = 0;
        *(unsigned char *)(c + 0x469d + (i << 6)) += 1;
        if (*(unsigned char *)(c + (i << 6) + 0x4000 + 0x69d) >= 4)
            *(unsigned char *)(c + (i << 6) + 0x4000 + 0x69d) = 0;
    }

    {
        unsigned char idx = *(unsigned char *)(c + (i << 6) + 0x4000 + 0x69d);
        *(unsigned char *)(c + (i << 6) + 0x4000 + 0x69c) = data_ov006_0212ebb4[idx];
        if (*(unsigned char *)(c + (i << 6) + 0x4000 + 0x69e) != 0)
            *(unsigned char *)(c + 0x469c + (i << 6)) += 4;
    }

    *(int *)(c + 0x4660 + (i << 6)) += *(int *)(c + (i << 6) + 0x4000 + 0x668);

    {
        int val = *(int *)(c + (i << 6) + 0x4000 + 0x660);
        int scaled = val >> 12;
        if (scaled >= 0x110 || scaled <= -16) {
            *(unsigned char *)(c + (i << 6) + 0x4000 + 0x699) = 6;
        }
    }
}
#pragma pop

// @symbol func_ov006_0210397c
void func_ov006_0210397c(unsigned char* base, int idx) {
    *(unsigned char*)(base + (idx << 6) + 0x4000 + 0x698) = 0;
    *(unsigned char*)(base + (idx << 6) + 0x4000 + 0x69a) = 0;
}

// @symbol func_ov006_02103994
#pragma push
#pragma opt_common_subs off
void func_ov006_02103994(char *c, int i)
{
    *(int*)(c + 0x4660 + i * 0x40) += *(int*)(c + i * 0x40 + 0x4000 + 0x668);
    *(int*)(c + 0x4664 + i * 0x40) += *(int*)(c + i * 0x40 + 0x4000 + 0x66c);
    *(int*)(c + 0x4684 + i * 0x40) += 0x10;
    *(int*)(c + 0x466c + i * 0x40) += *(int*)(c + i * 0x40 + 0x4000 + 0x684);

    if (*(int*)(c + i * 0x40 + 0x4000 + 0x66c) >= 0x8000)
        *(int*)(c + i * 0x40 + 0x4000 + 0x66c) = 0x8000;

    if (*(unsigned short*)(c + i * 0x40 + 0x4600 + 0x96) != 0)
    {
        *(unsigned short*)(c + 0x4696 + i * 0x40) -= 1;

        if (*(unsigned short*)(c + i * 0x40 + 0x4600 + 0x96) == 0x20)
        {
            int r3 = func_ov004_020adbc0();
            int f660 = *(int*)(c + i * 0x40 + 0x4000 + 0x660);
            int f664 = *(int*)(c + i * 0x40 + 0x4000 + 0x664);
            func_ov006_02100278(c, f660, f664, r3 + 1);
            func_ov004_020adb1c(func_ov004_020adbc0() + 1);
        }
    }

    if ((*(int*)(c + i * 0x40 + 0x4000 + 0x664) >> 12) < 0xa8) return;

    func_ov006_02102d6c((struct Objd6c *)c, i);
}
#pragma pop

// @symbol func_ov006_02103ac0
#pragma push
#pragma opt_strength_reduction off
void func_ov006_02103ac0(char *c)
{
    int i;
    unsigned short *q;
    if (*(unsigned short *)(c + 0x566c) == 0) return;
    q = (unsigned short *)(c + 0x566c);
    *q = *q - 1;
    if (*(short *)(c + 0x566c) > 0) return;
    *(unsigned short *)(c + 0x566c) = 0;
    for (i = 0; i < 0x30; i++) {
        char *b = c + (i << 6);
        if (*(unsigned char *)(b + 0x4698) == 0) {
            *(unsigned char *)(b + 0x4698) = 1;
            *(unsigned char *)(b + 0x469a) = 1;
            *(unsigned char *)(b + 0x4699) = 0;
            *(int *)(b + 0x4684) = 0;
            *(int *)(b + 0x4660) = 0x80000;
            *(int *)(b + 0x4664) = 0x28000;
            *(int *)(b + 0x4670) = 0;
            *(int *)(b + 0x4674) = 0;
            *(unsigned char *)(b + 0x469b) = 0;
            *(int *)(b + 0x4668) = 0;
            *(int *)(b + 0x466c) = 0;
            *(short *)(b + 0x4696) = 0;
            *(short *)(b + 0x4690) = 0;
            *(short *)(b + 0x4692) = 0;
            *(unsigned char *)(b + 0x469c) = 0;
            *(unsigned char *)(b + 0x469d) = 0;
            *(unsigned char *)(b + 0x469e) = 0;
            *(int *)(b + 0x4688) = 0;
            *(int *)(b + 0x468c) = 4;
            *(unsigned char *)(c + 0x5676) = (unsigned char)(i + 1);
            if (*(unsigned short *)(c + 0x5670) != 0) {
                Sound::PlayBank2_2D(0x19d);
                return;
            }
            *(unsigned short *)(c + 0x5670) += 1;
            return;
        }
    }
}
#pragma pop

// @symbol func_ov006_02103bfc
#pragma push
#pragma opt_strength_reduction off
void func_ov006_02103bfc(char *c)
{
    int i;
    int j;
    char *p;
    *(int *)(c + 0x5660) = 0;
    for (i = 0; i < 0x30; i++) {
        char *r = c + (i << 6);
        *(int *)(r + 0x4660) = 0;
        *(int *)(r + 0x4664) = 0;
        *(int *)(r + 0x4678) = 0;
        *(int *)(r + 0x467c) = 0;
        *(int *)(r + 0x4680) = 0;
        *(int *)(r + 0x4684) = 0;
        *(unsigned short *)(r + 0x4694) = 0;
        *(unsigned short *)(r + 0x4696) = 0;
        *(unsigned char *)(r + 0x4698) = 0;
        *(unsigned char *)(r + 0x4699) = 0;
        *(unsigned char *)(r + 0x469a) = 0;
    }
    p = c;
    for (j = 0; j < 0x10; j++) {
        *(unsigned char *)(p + 0x5336) = 0;
        *(unsigned char *)(p + 0x5334) = 0;
        p += 0x18;
    }
    *(unsigned short *)(c + 0x566e) = 0;
    *(unsigned short *)(c + 0x5672) = 0;
    *(unsigned char *)(c + 0x5677) = 0;
    *(unsigned char *)(c + 0x5678) = 0;
    *(unsigned char *)(c + 0x5679) = 0;
    *(unsigned char *)(c + 0x567a) = 0;
    *(unsigned short *)(c + 0x5670) = 0;
    func_ov004_020adb1c(0);
    func_ov006_02102564((char (*)[64])c);
    func_ov006_02100058((unsigned char (*)[0x20])c);
}
#pragma pop

// @symbol _ZN16dScMgPachinko2_c13OnYoshiTryEatEi
/* Slot 18, overriding dScMgBase_c::OnYoshiTryEat(int). That name is a
   placeholder borrowed from dActor_c by slot index. */
void dScMgPachinko2_c::OnYoshiTryEat(int n)
{
    /* Reading through `ro` keeps mwcc from holding &unk_0bc in a register,
       which costs a word. */
    const dScMgPachinko2_c *ro = this;

    unk_5660 = 0;
    if (n == 0x10) {
        unk_0bc = ro->unk_0bc + 1;
        if (unk_0bc > 0x270e) unk_0bc = 0x270e;
    } else {
        unk_0bc = 0;
        /* dead clamp, kept: dropping it changes the code */
        if (unk_0bc > 0x270e) unk_0bc = 0x270e;
    }
    FreeGfxSlotsById(0x1d);
    func_ov006_02103bfc((char *)this);
}

// @symbol _ZN16dScMgPachinko2_c6RenderEv
/* Slot 9. Its eight helpers are still free functions. */
s32 dScMgPachinko2_c::Render()
{
    func_ov006_021004c0((struct Obj4c0 *)this);
    func_ov006_02100488((char *)this);
    func_ov006_02102624((char *)this);
    func_ov006_02100314((char *)this);
    func_ov006_02100140((char *)this);
    func_ov006_02102de4((char *)this);
    func_ov006_0210068c((struct Obj68c *)this);
    func_ov006_020ff47c((struct Obj *)this);
    return 1;
}

// @symbol _ZN16dScMgPachinko2_c8BehaviorEv
/* Slot 6. unk_5660 selects which helpers run this frame. */
s32 dScMgPachinko2_c::Behavior()
{
    switch (unk_5660) {
    case 0:
        func_ov006_02100084((char *)this);
        func_ov006_021024e0((char *)this);
        func_ov006_020fffec((char *)this);
        unk_566c = 0x10;
        unk_5660 = 1;
        break;
    case 1:
        if (mPromptBlinkCount == 0) {
            mPromptEnabled = 1;
            mPromptBlinkCount = 1;
            mPromptBlinkTimer = 0;
        }
        func_ov006_0210265c((char *)this);
        func_ov006_02102ef4((unsigned char *)this);
        func_ov006_02103ac0((char *)this);
        func_ov006_02102e8c((C *)this);
        func_ov006_0210246c((C *)this);
        func_ov006_020fff84((C *)this);
        func_ov006_02100380((char *)this);
        func_ov006_0210258c((char *)this);
        break;
    case 2:
        func_ov006_02102e8c((C *)this);
        func_ov006_0210246c((C *)this);
        func_ov006_020fff84((C *)this);
        func_ov006_02100380((char *)this);
        func_ov006_02100554(this);
        func_ov006_021001ac((char *)this);
        break;
    case 3:
        func_ov006_02102e8c((C *)this);
        func_ov006_0210246c((C *)this);
        func_ov006_020fff84((C *)this);
        func_ov006_021001ac((char *)this);
        if (unk_566e != 0) {
            unk_566e--;
            if ((s16)unk_566e <= 0) {
                func_ov004_020b0a54(0x10);
                mPromptEnabled = 0;
            }
        }
        break;
    }
    return 1;
}

// @symbol _ZN16dScMgPachinko2_c13InitResourcesEv
/* Slot 0. Loads both screens' backgrounds and OBJ graphics, then runs the
   three helpers and the two stores of Behavior's case 0. */
s32 dScMgPachinko2_c::InitResources()
{
    char *b;
    char *dst;
    /* volatile: the ROM stores each fill value to the stack and reloads it */
    volatile u16 spC;
    volatile u16 spE;
    int sp4;
    int sp8;
    int f;
    int n;
    int y;
    int x;

    data_0209d45c |= 8; /* main BG3 on */
    *(volatile u16 *)0x400000e = (*(volatile u16 *)0x400000e & ~3) | 1;
    *(volatile u16 *)0x400000e &= ~0x40;
    *(volatile u32 *)0x400001c = 0;
    *(volatile u16 *)0x400000e = (*(volatile u16 *)0x400000e & 0x43) | 0x1210;

    f = LoadFile(0x5f);
    DecompressLZ16(f, (void *)func_02054d88());
    Deallocate((void *)f);

    f = LoadFile(0x60);
    _ZN2GX10LoadBGPlttEPKvjj((const void *)f, 0x60, 0x1a0);
    Deallocate((void *)f);

    f = LoadFile(0x61);
    func_02056314((void *)f, 0, 0x800);
    Deallocate((void *)f);

    sp4 = LoadFile(0xe0);
    sp8 = LoadFile(0xe1);
    DecompressLZ16(sp4, (void *)0x6400000);
    _ZN2GX11LoadOBJPlttEPKvjj((const void *)sp8, 0, 0x100);

    data_0209d454 |= 0xd; /* sub BG0, BG2 and BG3 on */
    *(volatile u16 *)0x400100c &= ~3;
    *(volatile u16 *)0x400100c &= ~0x40;
    *(volatile u32 *)0x4001018 = 0;
    *(volatile u16 *)0x400100c = (*(volatile u16 *)0x400100c & 0x43) | 0x418;

    b = (char *)_ZN3G2S13GetBG2CharPtrEv();
    spC = 0;
    MultiStore16(spC, b, 0x6000);

    n = 0;
    for (y = 0; y < 0x18; y++) {
        for (x = 0; x < 0x20; x++) {
            dst = (char *)((u16 *)_ZN3G2S12GetBG2ScrPtrEv() + x + y * 0x20);
            spE = n;
            MultiStore16(spE, dst, 2);
            n++;
        }
    }

    func_ov004_020af2f8((char *)this, 0, 2, 0);

    *(volatile u16 *)0x400100e = (*(volatile u16 *)0x400100e & ~3) | 2;
    *(volatile u16 *)0x400100e &= ~0x40;
    *(volatile u32 *)0x400101c = 0;
    *(volatile u16 *)0x400100e = (*(volatile u16 *)0x400100e & 0x43) | 0x208;

    f = LoadFile(0x5c);
    DecompressLZ16(f, (void *)_ZN3G2S13GetBG3CharPtrEv());
    Deallocate((void *)f);

    f = LoadFile(0x5d);
    _ZN3GXS10LoadBGPlttEPKvjj((const void *)f, 0x60, 0x1a0);
    Deallocate((void *)f);

    f = LoadFile(0x5e);
    func_020562b4((const void *)f, 0, 0x800);
    Deallocate((void *)f);

    *(volatile u16 *)0x4001008 = (*(volatile u16 *)0x4001008 & ~3) | 1;
    *(volatile u16 *)0x4001008 &= ~0x40;
    *(volatile u32 *)0x4001010 = 0;
    *(volatile u16 *)0x4001008 = (*(volatile u16 *)0x4001008 & 0x43) | 0x608;

    f = LoadFile(5);
    func_020564f4((const void *)f, 0, 0x800);
    Deallocate((void *)f);

    DecompressLZ16(sp4, (void *)0x6600000);
    _ZN3GXS11LoadOBJPlttEPKvjj((const void *)sp8, 0, 0x100);
    Deallocate((void *)sp4);
    Deallocate((void *)sp8);

    func_ov006_02103bfc((char *)this);
    func_ov006_02100084((char *)this);
    func_ov006_021024e0((char *)this);
    func_ov006_020fffec((char *)this);
    func_ov004_020b04d0(0x20);

    unk_0a4 = 1;
    unk_566c = 0x10;
    unk_5660 = 1;
    return 1;
}

// @symbol dScMgPachinko2_c_classInit
/* The MG_TAMAIRE factory, kept hand-built: fBase_c::operator new for
   0x567c bytes, the base constructor, then the vtable address point
   0x0213dbbc stored as data. The classInit spelling follows later EAD
   lineage; the historical alias is MgLakituLaunch_Spawn. */
extern "C" int *dScMgPachinko2_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(22140);
    if (p) {
        _ZN11dScMgBase_cC2Ev(p);
        p[0] = (int)data_ov006_0213dbbc;
    }
    return p;
}
