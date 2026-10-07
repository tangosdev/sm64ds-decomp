//cpp
/* Bob-omb Squad. The player aims a Bob-omb with the stylus and fires it
 * up the board; a shot that reaches a target knocks the balls loose.
 * 75 functions (.text 0x020fa75c..0x020ff3ec), the class's whole linker
 * unit. Below it is the pair-match card game (d_s_mg_m_carlo2.cpp); above
 * it is dScMgPachinko_c_classInit, which stays in d_s_mg_pachinko.cpp, and
 * then the next class's destructor.
 *
 * The 30 functions from 0x020fc8c0 up were folded in from one-function
 * sources. The ball drawer at 0x020fc8c0 was the last to match: its
 * products go through FX_Mul, under opt_prelinearize off. 0x020fcb4c,
 * 0x020fdaf0 and 0x020fe394 depend on spellings that look interchangeable
 * (declaration order, which of two locals is read first); each one is
 * noted at its function.
 *
 * The destructor is declared first in the header, so this TU owns the key
 * function and emits the vtable and RTTI; the ROM's own copies live
 * elsewhere and these are discarded at link time.
 *
 * Functions run in ROM order here, lowest address first, under `#pragma
 * defer_codegen off`. Do not reorder.
 *
 * Still raw: the func_ and data_ helpers are unnamed in symbols.txt. The
 * targets at 0x5bcc, the effects at 0x5958, the score popups at 0x4cf0,
 * the scrolling background at 0x5bfc and the pipes at 0x4ea0 are padding
 * in the header, so they are reached by offset. Shot and ball fields go
 * through `p + i * 0x38`: indexing mShot[i]/mBall[i] is one word different,
 * so the multiply forms stay.
 */

#pragma defer_codegen off

#include "types.h"
#include "dScMgPachinko_c.h"

/* Two helpers dispatch through the ov006 pointer-to-member tables, each on
   its own view of the scene. Both pointer-to-member types are formed while
   the view is still incomplete: the compiler picks the pointer-to-member
   layout from that, and completing the class first changes the code. */
struct C;
typedef void (C::*PMF)(int);
struct Entry { PMF pmf; };
struct C { char pad[0x5c0e]; unsigned char guard; unsigned char idx; };

struct C2;
typedef void (C2::*PMF2)(int);
struct C2 { char pad[0x5bc6]; unsigned char g; char gap; unsigned char idx; };

/* func_ov006_020fb45c's typed view of the rows at 0x5bb4. `volatile` on the
   position is needed: the ROM reloads it after the store. */
typedef struct Ent {
    volatile int a; /* 0x00 position (20.12) */
    int unk4;       /* 0x04 */
    int b;          /* 0x08 velocity (20.12) */
    char _padC[0x10];
} Ent;

typedef struct Obj {
    char _pad0[0x5bb4]; /* 0x0000 */
    Ent entries[8];     /* 0x5bb4 */
} Obj;

/* The {code pointer, adjustment} records func_ov006_020fc7d0,
   func_ov006_020fda7c and func_ov006_020fe248 walk. */
struct Ent7d0 { int a; int b; };

/* func_ov006_020fb4e0's stride-only view of the same 0x1c-byte rows. */
struct E { char pad[0x1c]; };

/* The sprite record func_ov004_020aff38 draws; only handled by pointer here. */
struct OamAttr;

/* The 2x2 rotate-and-scale matrix func_ov006_020fc8c0 hands the sprite
   drawer. */
struct Matrix2x2 { int _00, _01, _10, _11; };

/* Launder macros: they make the compiler materialise a base address on its
   own instead of folding it into a neighbouring add, as the ROM does. */
#define AT(p, off) ((void*)(int)((char*)(p) + (off)))
/* func_ov006_020fd2d8's two spellings of the same address: one through a
   signed and one through an unsigned integer. */
#define ATI(p, off) ((char *)(int)((char *)(p) + (off)))
#define ATU(p, off) ((char *)(unsigned int)((char *)(p) + (off)))
#define M1(a) (a)
#define M2(a) (a)
#define AC (*(int *)(((long long)((int)a + 0x5c14))))

/* Pipe row accessors for func_ov006_020fc2ec. */
#define ST(c,b)       (*(u8*)((c) + 0x4eb4 + (b)))
#define CTA(c,b)      (*(u16*)((c) + 0x4eb0 + (b)))
#define FB(c,b)       (*(u8*)((c) + 0x4eb6 + (b)))
#define OUTFLAG(c,b)  (*(u8*)((c) + 0x4eb5 + (b)))
#define OUTFLAG2(c,b) (*(u8*)((c) + 0x4eb3 + (b)))
#define ACC(c,b)      (*(s32*)((c) + 0x4ea0 + (b)))
#define VEL(c,b)      (*(s32*)((c) + 0x4ea8 + (b)))

/* The scene seen through a char pointer, for the free helpers below. */
#define PACHINKO(p) ((dScMgPachinko_c *)(p))
/* One record, already stepped by i * 0x38; indexing the array or saving an
 * element pointer is one word different. */
#define SHOT_BASE ((int)&((dScMgPachinko_c *)0)->mShot)
#define BALL_BASE ((int)&((dScMgPachinko_c *)0)->mBall)
#define SHOT(p) ((dScMgPachinko_shot *)((p) + SHOT_BASE))
#define BALL(p) ((dScMgPachinko_ball *)((p) + BALL_BASE))
/* func_ov006_020fbd38 adds the field first, then the index. `&SHOT(p)->f`
 * shares one base and does not match. */
#define SHOT_OFF(member) (SHOT_BASE + (int)&((dScMgPachinko_shot *)0)->member)

/* 20.12 multiply with rounding, for func_ov006_020fdd40. */
#define FMUL(a, b) ((int)(((long long)(a) * (b) + 0x800) >> 12))

namespace G2 {
    void *GetBG2CharPtr();
    void *GetBG2ScrPtr();
}

namespace G2S {
    unsigned GetBG2CharPtr();
    unsigned GetBG2ScrPtr();
    unsigned GetBG3CharPtr();
}

namespace cstd {
    int sqrt(unsigned long long val);
}

namespace Sound {
    void PlayBank2_2D(unsigned int id);
}

extern "C" {

/* --- shared ov004, main and library helpers --- */
extern void func_ov004_020afdd0(void *a0, int a1, int a2, int a3, int a4);
extern void func_ov004_020b2444(int x, int y, int h, int a, int b, int c, int d);
extern int  func_ov004_020adbc0(void);
extern int  func_ov004_020b1a5c(int a, int b);
extern int  func_ov004_020adc1c(void);
extern int  func_ov004_020b19f0(int score);
extern void func_ov004_020adb1c(int self);
extern int  func_020126e8(int a);
extern int  func_02012468(int a, int b, int c, int d, int e, int f, int g, short h);
extern void SetBg2Offset(int a, int b);
extern void MultiStore16(unsigned short val, char *dst, int nbytes);
extern int  RandomIntInternal(int *seed);
extern int  _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(int, void*, int, int, int, int, int, void*, int, int);
extern void func_ov004_020b023c(void *sprite, int x, int y, int a3, void *m);
extern int  func_ov004_020aff38(OamAttr *a, int b, int c, int d, int e, int f, int g);
extern void func_ov004_020b0a54(int a);
extern void func_ov004_020b04d0(int a);
extern void func_ov004_020af2f8(char *a, char b, int c, int d);
extern void func_020126ac(int a0, int a1, int a2, int a3, int s0);
extern void func_02012718(int a, int b);
extern int  Sound_PlayIfNotActive(int handle, int player, int id, int unused);
extern s16  _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
extern void FreeGfxSlotsById(int arg);
extern int  LoadFile(int handle);
extern void DecompressLZ16(void *src, void *dst);
extern void Deallocate(void *p);
extern int  func_02054d88(void);
extern void func_02056314(void *dst, u32 offset, u32 len);
extern void func_020563d4(const void *src, u32 offset, u32 count);
extern void func_020562b4(const void *src, u32 offset, u32 count);
extern void func_020564f4(const void *src, int offset, int count);
extern void _ZN2GX10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN2GX11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN3GXS10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN3GXS11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);

/* --- ROM data this TU reads --- */
extern int   data_0209d4b8;
extern unsigned char data_0209d45c;
extern u8    data_0209d454;
extern s16   data_02082214[];
// local extern: this file needs a record-view spelling of one of the touch lanes (the ROM scales the slot in the addressing mode), which conflicts with PlayerInput.h; the header is not included and all five symbols are declared here.
extern u8    gActivePlayerSlot;
// local extern: see above.
extern u8    gTouchHeld[][4];
// local extern: see above.
extern u8    gTouchEdge[][4];
// local extern: see above.
extern u8    gTouchX[][4];
// local extern: see above.
extern u8    gTouchY[][4];
extern void *data_ov006_0213ac24;
extern unsigned char data_ov006_0212eb0c[];
extern unsigned char data_ov006_0212eb10[];
extern unsigned char data_ov006_0212eb14[];
extern unsigned char data_ov006_0212eb1c[];
extern int   data_ov006_0212eb24[];
extern int   data_ov006_0212eb2c[];
extern u8    data_ov006_0212eb34[];
extern u8    data_ov006_0212eb3c[];
extern s32   data_ov006_0212eb44[];
extern int   data_ov006_0212eb50[];
extern s32   data_ov006_0212eb60[];
extern int   data_ov006_0212eb70[];
extern int   data_ov006_02133e10[];
extern int   data_ov006_02133e7c[];
extern int   data_ov006_02136b80[];
extern int   data_ov006_02136e0c[];
extern int   data_ov006_021375f4[];
extern int   data_ov006_0212eb80[];
extern int   data_ov006_0212eb94[];
extern int   data_ov006_02136cd4[];
extern int   data_ov006_02137614;
extern u16   data_ov006_0213d954[];
extern u8    data_ov006_0213d974[];

/* The two pointer-to-member tables the helpers dispatch through, and the
   record tables func_ov006_020fc7d0, func_ov006_020fe248 and
   func_ov006_020fda7c walk. Static initialisers fill them from {code
   pointer, adjustment} records in ov006 .data. */
extern Entry  data_ov006_02142604[];
extern Ent7d0 data_ov006_02142624[];
extern Ent7d0 data_ov006_02142644[];
extern PMF2   data_ov006_0214266c[];
extern Ent7d0 data_ov006_02142694[];

/* This TU's own members, forward-declared for the callers below them. */
extern void func_ov006_020fa7b8(char *thiz);
extern void func_ov006_020fa844(char *self);
extern void func_ov006_020fa924(char *p);
extern void func_ov006_020fa9a0(char *p);
extern void func_ov006_020fa9c8(char *p, int i);
extern void func_ov006_020faac8(char *c, int i);
extern void func_ov006_020fab70(char *c, int i);
extern void func_ov006_020fac48(char *c, int idx);
extern void func_ov006_020fad34(C *c);
extern void func_ov006_020fad90(char *c);
extern void func_ov006_020fadfc(char *p);
extern void func_ov006_020fae20(char *base);
extern void func_ov006_020fae90(u8 *c);
extern void func_ov006_020faeec(char *p);
extern void func_ov006_020faf14(char *c);
extern void func_ov006_020faf6c(char *c, int idx);
extern void func_ov006_020fb0fc(char *c, int i);
extern void func_ov006_020fb1c4(char *c, int i);
extern void func_ov006_020fb230(char *p, int i);
extern void func_ov006_020fb45c(Obj *self, int i);
extern void func_ov006_020fb4e0(char *c, int idx);
extern void func_ov006_020fb60c(C2 *c);
extern void func_ov006_020fb670(char *obj);
extern void func_ov006_020fb74c(void *base);
extern void func_ov006_020fb7e0(char *thiz);
extern void func_ov006_020fb8fc(char *c, int a2, int a3, int a4, int a5, int a6);
extern void func_ov006_020fb97c(char *c);
extern void func_ov006_020fba28(void *);
extern void func_ov006_020fba48(void *scene);
extern void func_ov006_020fba64(char *base);
extern void func_ov006_020fbad4(char *c);
extern void func_ov006_020fbb2c(char *c, int idx, unsigned short val);
extern void func_ov006_020fbbe8(char *c);
extern void func_ov006_020fbcb8(void *a0, int a1, int a2, int a3);
extern void func_ov006_020fbd38(void *arg0);
extern void func_ov006_020fc144(char *base);
extern void func_ov006_020fc1b4(char *base, int val);
extern int  func_ov006_020fc1f8(char *self, int idx);
extern void func_ov006_020fc2ec(char *c, int i);
extern void func_ov006_020fc500(char *c, int i);
extern void func_ov006_020fc718(char *thiz, int n);
extern void func_ov006_020fc7d0(char *c);
extern void func_ov006_020fc844(unsigned char *c);
extern void func_ov006_020fc8c0(char *scene);
extern void func_ov006_020fc9b0(char *base, int i);
extern void func_ov006_020fca1c(char *c, int idx);
extern void func_ov006_020fcb4c(char *base, int i);
extern void func_ov006_020fcd8c(char *thiz, int idx);
extern void func_ov006_020fce04(char *c, int i);
extern void func_ov006_020fcec4(char *c, int i);
extern void func_ov006_020fd088(char *self, int idx);
extern void func_ov006_020fd17c(char *c, int i);
extern void func_ov006_020fd2d8(char *o, int i);
extern void func_ov006_020fd894(char *o, int i);
extern void func_ov006_020fd9cc(char *base, int idx);
extern void func_ov006_020fda7c(char *c);
extern void func_ov006_020fdaf0(char *base, int i);
extern void func_ov006_020fdd40(dScMgPachinko_c *self);
extern void func_ov006_020fe1a8(char *p);
extern void func_ov006_020fe1d0(char *thiz);
extern void func_ov006_020fe248(char *c);
extern void func_ov006_020fe2bc(char *c);
extern void func_ov006_020fe2e4(char *self, int i);
extern void func_ov006_020fe394(dScMgPachinko_c *self, int i);
extern void func_ov006_020fe750(char *c, int idx);
extern void func_ov006_020fe90c(char *base, int i);
extern void func_ov006_020fea54(char *p, int idx);
extern void func_ov006_020fea70(char *o);
extern void func_ov006_020feba8(char *self);

}  /* extern "C" */

// @symbol _ZN15dScMgPachinko_cD1Ev
// @symbol _ZN15dScMgPachinko_cD0Ev
/* Both D1 and D0 come from this one definition. */
dScMgPachinko_c::~dScMgPachinko_c()
{
}

extern "C" {  /* .c-derived members: C linkage for everything below */

// @symbol func_ov006_020fa7b8
void func_ov006_020fa7b8(char* row)
{
    for (int i = 0; i < 3; i++) {
        if (*(unsigned char*)(row + 0x4000 + 0xe6d) != 0) {
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(false, data_ov006_0213ac24,
                *(int*)(row + 0x4000 + 0xe58) >> 12,
                *(int*)(row + 0x4000 + 0xe5c) >> 12,
                -1, -1, 0x1000, (void*)0x1000,
                *(unsigned short*)(row + 0x4e00 + 0x68), -1);
        }
        row += 0x18;
    }
}

// @symbol func_ov006_020fa844
void func_ov006_020fa844(char *raw)
{
    int i;
    char *row;

    row = raw;
    for (i = 0; i < 3; i++) {
        *(s32 *)(row + 0x4e58) = (data_ov006_0212eb44[i] << 12) - *(s32 *)(raw + 0x5bfc);
        *(s32 *)(row + 0x4e5c) = -*(s32 *)(raw + 0x5c00);
        if (*(u8 *)(row + 0x4e6e) != 0) {
            *(u16 *)AT(row, 0x4e68) -= 0x40;
            if (*(s16 *)(row + 0x4e68) <= -0x1000) {
                *(u16 *)(row + 0x4e68) = 0xf000;
                *(u8 *)(row + 0x4e6e) = 0;
            }
        } else {
            *(u16 *)AT(row, 0x4e68) += 0x40;
            if (*(s16 *)(row + 0x4e68) >= 0x1000) {
                *(s16 *)(row + 0x4e68) = 0x1000;
                *(u8 *)AT(row, 0x4e6e) += 1;
            }
        }
        row += 0x18;
    }
}

// @symbol func_ov006_020fa924
void func_ov006_020fa924(char *row)
{
    int i;
    char *raw = row;
    for (i = 0; i < 3; i++, row += 0x18)
    {
        if (*(unsigned char *)(row + 0x4e6c) == 0)
        {
            *(unsigned char *)(row + 0x4e6c) = 1;
            *(unsigned char *)(row + 0x4e6d) = 1;
            *(int *)(row + 0x4e58) = (data_ov006_0212eb44[i] << 12) - *(int *)(raw + 0x5bfc);
            *(int *)(row + 0x4e5c) = -*(int *)(raw + 0x5c00);
            *(short *)(row + 0x4e6a) = 0;
            *(short *)(row + 0x4e68) = 0;
            *(unsigned char *)(row + 0x4e6e) = 0;
        }
    }
}

// @symbol func_ov006_020fa9a0
void func_ov006_020fa9a0(char *row)
{
    int i;
    for (i = 0; i < 3; i++) {
        *(unsigned char *)(row + 0x4e6c) = 0;
        *(unsigned char *)(row + 0x4e6d) = 0;
        row += 0x18;
    }
}

// @symbol func_ov006_020fa9c8
void func_ov006_020fa9c8(char *raw, int i)
{
    int off = i * 0x14;
    int y;
    *(int *)(raw + 0x5bfc + off) += *(int *)(raw + 0x5c04 + off);
    *(int *)(raw + 0x5c00 + off) += *(int *)(raw + 0x5c08 + off);
    if (*(int *)(raw + 0x5c04 + off) <= -0x1000)
        *(int *)(raw + 0x5c04 + off) += 0x40;
    if (*(int *)(raw + 0x5c08 + off) >= 0x3000)
        *(int *)(raw + 0x5c08 + off) += 0x100;
    y = *(int *)(raw + 0x5c00 + off) >> 12;
    SetBg2Offset(*(int *)(raw + 0x5bfc + off) >> 12, y);
    if (y < 0x40) return;
    *(short *)(raw + 0x5c0c + off) = 0x258;
    *(unsigned char *)(raw + 0x5c0f + off) = 0;
    data_0209d45c &= ~4;
    func_ov006_020fa9a0(raw);
}

// @symbol func_ov006_020faac8
void func_ov006_020faac8(char* raw, int i)
{
    int* vel = (int*)(raw + 0x5c04 + i * 0x14);
    int* pos = (int*)(raw + 0x5bfc + i * 0x14);
    int x;

    *pos = *pos + *vel;

    if (*vel <= -0x2000) {
        *vel = *vel - 0x100;
    }

    x = *pos >> 12;
    SetBg2Offset(x, *(int*)(raw + i * 0x14 + 0x5c00) >> 12);

    if (x > -0x90) return;

    *(int*)(raw + i * 0x14 + 0x5c08) = 0x800;
    *(unsigned char*)(raw + i * 0x14 + 0x5c0f) = 3;
}

// @symbol func_ov006_020fab70
void func_ov006_020fab70(char* raw, int i)
{
    int off = i * 0x14;
    int* velX;
    int* velY;
    int* posX;
    int* posY;
    int y;
    int x;

    *(int*)(raw + 0x5bfc + off) += *(int*)(raw + 0x5c04 + off);
    *(int*)(raw + 0x5c00 + off) += *(int*)(raw + 0x5c08 + off);
    velX = (int*)(raw + 0x5c04 + off);
    posX = (int*)(raw + 0x5bfc + off);
    posY = (int*)(raw + 0x5c00 + off);
    velY = (int*)(raw + 0x5c08 + off);
    if (*velX <= -0x2000) *velX -= 0x100;
    if (*velY <= -0xc00) *velY += 0x40;
    y = *posY >> 12;
    x = *posX >> 12;
    if (y <= 0) {
        y = 0;
        *posY = 0;
        *velY = 0;
        *(unsigned char*)(raw + off + 0x5c0f) = 2;
    }
    SetBg2Offset(x, y);
}

// @symbol func_ov006_020fac48
void func_ov006_020fac48(char *raw, int idx)
{
    int off;
    unsigned short delay;
    char *p_row;
    int neg;
    int b;
    int a;
    int lo;
    int hi;
    unsigned char flag;

    if (*(int *)((raw + 0x5000) + 0xc10) == 2)
        return;

    off = idx * 0x14;
    delay = *(unsigned short *)((raw + 0x5c0c) + off);
    if (delay != 0) {
        *(unsigned short *)((raw + 0x5c0c) + off) = delay - 1;
        return;
    }

    flag = data_0209d45c;
    p_row = raw + off;
    neg = 0x1000;
    data_0209d45c = flag | 4;
    *(int *)((raw + 0x5bfc) + off) = 0x100000;
    *(int *)((raw + 0x5c00) + off) = 0x40000;
    neg = -neg;
    /* Mix (raw+off) and p_row so mwccarm colors r4 and r5 like the ROM. */
    *(int *)(((raw + off) + 0x5000) + 0xc04) = neg;
    *(int *)((p_row + 0x5000) + 0xc08) = neg;
    a = *(int *)((raw + 0x5c00) + off);
    b = *(int *)((raw + 0x5bfc) + off);
    lo = b >> 12;
    hi = a >> 12;
    *(unsigned char *)(((raw + off) + 0x5000) + 0xc0f) = 1;
    *(volatile int *)0x4000018 = (0x1ff & lo) | (0x1ff0000 & (hi << 16));
    func_ov006_020fa924(raw);
}

// @symbol func_ov006_020fad34
/* Dispatches through the ov006 pointer-to-member table at 0x02142604. */
void func_ov006_020fad34(C* c) {
    if (c->guard) {
        (c->*(data_ov006_02142604[c->idx].pmf))(0);
    }
    func_ov006_020fa844((char *)c);
}

// @symbol func_ov006_020fad90
void func_ov006_020fad90(char *raw){
    *(unsigned char*)(raw+0x5000+0xc0e) = 1;
    *(int*)(raw+0x5000+0xbfc) = 0x100000;
    *(int*)(raw+0x5000+0xc00) = 0x40000;
    *(int*)(raw+0x5000+0xc04) = 0;
    *(int*)(raw+0x5000+0xc08) = 0;
    *(unsigned char*)(raw+0x5000+0xc0f) = 0;
    *(short*)(raw+0x5c00+0xc) = 0;
    *(volatile int*)0x4000018 =
        (0x1ff & (*(int*)(raw+0x5000+0xbfc) >> 12)) |
        (0x1ff0000 & ((*(int*)(raw+0x5000+0xc00) >> 12) << 16));
}

// @symbol func_ov006_020fadfc
void func_ov006_020fadfc(char *raw)
{
    data_0209d45c &= ~4;
    *(unsigned char *)(raw + 0x5c0e) = 0;
}

// @symbol func_ov006_020fae20
void func_ov006_020fae20(char *raw)
{
    int i;
    char *row = raw;
    for (i = 0; i < 4; i++) {
        if (*(unsigned char *)(row + 0x5000 + 0xbd5) != 0) {
            func_ov004_020afdd0(
                (void *)data_ov006_02136e0c[*(unsigned char *)(row + 0x5000 + 0xbd6)],
                *(int *)(row + 0x5000 + 0xbcc) >> 0xc,
                *(int *)(row + 0x5000 + 0xbd0) >> 0xc,
                -1, 1);
        }
        row += 0xc;
    }
}

// @symbol func_ov006_020fae90
void func_ov006_020fae90(u8 *row)
{
    int i;
    for (i = 0; i < 4; i++, row += 0xc) {
        *(u8 *)(row + 0x5bd4) = 1;
        *(u8 *)(row + 0x5bd5) = 1;
        *(int *)(row + 0x5bcc) = data_ov006_0212eb70[i] << 12;
        *(int *)(row + 0x5bd0) = 0xa0000;
        *(u8 *)(row + 0x5bd6) = 0;
    }
}

// @symbol func_ov006_020faeec
void func_ov006_020faeec(char *row)
{
    int i;
    for (i = 0; i < 4; i++) {
        *(unsigned char *)(row + 0x5bd4) = 0;
        *(unsigned char *)(row + 0x5bd5) = 0;
        row += 0xc;
    }
}

// @symbol func_ov006_020faf14
void func_ov006_020faf14(char* raw){
  if(*(unsigned char*)(raw+0x5000+0xbc7)==0) return;
  int x=*(int*)(raw+0x5000+0xbb0);
  int y=*(int*)(raw+0x5000+0xbb4);
  func_ov004_020afdd0((void *)data_ov006_021375f4[*(unsigned char*)(raw+0x5000+0xbc9)],
    x>>0xc, y>>0xc, -1, -1);
}

#pragma push
#pragma opt_common_subs off
// @symbol func_ov006_020faf6c
void func_ov006_020faf6c(char *raw, int idx)
{
    int i;
    int j;
    char *shot;
    char *ball;
    int dx;
    int dy;
    u8 state;
    int n;

    if (*(u8 *)(raw + idx * 0x1c + 0x5bc8) == 0)
        return;

    shot = raw;
    for (i = 0; i < 0x30; i++, shot += 0x38) {
        if (SHOT(shot)->active == 0)
            continue;
        if (SHOT(shot)->state < 2)
            continue;
        dx = (*(int *)(raw + idx * 0x1c + 0x5bb0) - SHOT(shot)->x) >> 12;
        dy = (*(int *)(raw + idx * 0x1c + 0x5bb4) - SHOT(shot)->y) >> 12;
        if (cstd::sqrt(dx * dx + dy * dy) > 0x18)
            continue;

        n = idx * 0x1c;
        PACHINKO(raw)->unk_5c2a = 0x20;
        SHOT(raw + i * 0x38)->active = 0;
        SHOT(raw + i * 0x38)->unk36 = 0;
        func_ov006_020fb8fc(raw,
                            *(int *)(raw + n + 0x5bb0),
                            *(int *)(raw + n + 0x5bb4),
                            2, 0, 0);
        ball = raw;
        for (j = 0; j < 0x1e; j++, ball += 0x38) {
            if (BALL(ball)->active == 0)
                continue;
            state = BALL(ball)->state;
            if (state == 0)
                continue;
            if (state < 5) {
                BALL(ball)->state = 6;
                BALL(ball)->timer = 0x10;
            }
        }
        *(u8 *)(raw + n + 0x5bc7) = 0;
        *(u8 *)(raw + n + 0x5bc6) = 0;
        *(u8 *)(raw + 0x5c34) = 0;
        Sound::PlayBank2_2D(0x18a);
        return;
    }
}
#pragma pop

// @symbol func_ov006_020fb0fc
void func_ov006_020fb0fc(char *raw, int i){
  int n = i*0x1c;
  int *pos = (int*)(raw + 0x5bb0);
  int *sound = (int*)(raw + 0x5bc0);
  *(int*)((char*)sound + n) = func_02012468(*(int*)((char*)sound + n), 2, 0x189, 4, 0, 0, func_020126e8(*(int*)((char*)pos + n)), 0);
  *(int*)((char*)pos + n) = *(int*)((char*)pos + n) + *(int*)(raw + 0x5bb8 + n);
  {
    int v = *(int*)((char*)pos + n) >> 12;
    if (v >= 0x110) goto clear;
    if (v > -0x10) return;
  }
clear:
  *(unsigned char*)(raw + 0x5bc6 + n) = 0;
  *(unsigned char*)(raw + 0x5bc7 + n) = 0;
  *(unsigned char*)(raw + 0x5c34) = 0;
}

// @symbol func_ov006_020fb1c4
void func_ov006_020fb1c4(char *raw, int i){
    short *delay = (short*)(raw + 0x5bc4 + i*0x1c);
    if(*(unsigned short*)delay != 0){
        *delay = *(unsigned short*)delay - 1;
        if(*delay < 0) *delay = 0;
        return;
    }
    *(unsigned char*)(raw+i*0x1c+0x5000+0xbc8)=2;
    *(unsigned char*)(raw+0x5bca+i*0x1c) = *(unsigned char*)(raw+0x5bca+i*0x1c) - 1;
}

// @symbol func_ov006_020fb230
void func_ov006_020fb230(char *raw, int i)
{
  int n = i * 0x1c;
  int x;
  u8 flag;
  *((int *) ((raw + 0x5bc0) + n)) = func_02012468(*((int *) ((raw + 0x5bc0) + n)), 2, 0x189, 4, 0, 0, func_020126e8(*((int *) ((raw + 0x5bb0) + n))), 0);
  *((int *) ((raw + 0x5bb0) + n)) += *((int *) ((raw + 0x5bb8) + n));
  flag = *((u8 *) ((raw + 0x5bc9) + n));
  x = (*((int *) ((raw + 0x5bb0) + n))) >> 12;
  if (flag != 0)
  {
    if (x >= 0xc0)
      *((int *) ((raw + 0x5bb8) + n)) -= 0x80;
    else if ((*((int *) ((raw + 0x5bb8) + n))) <= 0x3000)
      *((int *) ((raw + 0x5bb8) + n)) += 0x200;
    if (x >= 0xf0)
    {
      u32 a;
      if ((*((u8 *) ((raw + 0x5bca) + n))) == 0) { *((u8 *) ((raw + 0x5bc8) + n)) = 4; return; }
      *((int *) ((raw + 0x5bb0) + n)) = 0xf0000;
      *((int *) ((raw + 0x5bb8) + n)) = 0;
      *((u8 *) ((raw + 0x5bc9) + n)) ^= 1;
      a = (((u32) RandomIntInternal(&data_0209d4b8)) >> 16) & 0x7fff;
      *((u16 *) ((raw + 0x5bc4) + n)) = (u16) ((((a << 3) >> 15) * 2) + 0x10);
      *((u8 *) ((raw + 0x5bc8) + n)) = 3;
      return;
    }
  }
  else
  {
    if (x <= 0x40)
      *((int *) ((raw + 0x5bb8) + n)) += 0x80;
    else if ((*((int *) ((raw + 0x5bb8) + n))) >= (-0x3000))
      *((int *) ((raw + 0x5bb8) + n)) -= 0x200;
    if (x <= 0x10)
    {
      u32 a;
      if ((*((u8 *) ((raw + 0x5bca) + n))) == 0) { *((u8 *) ((raw + 0x5bc8) + n)) = 4; return; }
      *((int *) ((raw + 0x5bb0) + n)) = 0x10000;
      *((int *) ((raw + 0x5bb8) + n)) = 0;
      *((u8 *) ((raw + 0x5bc9) + n)) ^= 1;
      a = (((u32) RandomIntInternal(&data_0209d4b8)) >> 16) & 0x7fff;
      *((u16 *) ((raw + 0x5bc4) + n)) = (u16) ((((a << 3) >> 15) * 2) + 0x10);
      *((u8 *) ((raw + 0x5bc8) + n)) = 3;
      return;
    }
  }
}

// @symbol func_ov006_020fb45c
/* Moves row i: pos += vel. While below -0xc0 it keeps accelerating
   (vel -= 0x60); once it reaches -0xc0 it snaps to -0xc0000 and stops.
   Then hands on to func_ov006_020fb230. */
void func_ov006_020fb45c(Obj* self, int i) {
    self->entries[i].a += self->entries[i].b;
    if (self->entries[i].a >> 12 >= -0xc0) {
        self->entries[i].a = -0xc0000;
        self->entries[i].b = 0;
    } else {
        self->entries[i].b -= 0x60;
    }
    func_ov006_020fb230((char *)self, i);
}

#pragma push
#pragma opt_common_subs off
// @symbol func_ov006_020fb4e0
void func_ov006_020fb4e0(char* raw, int idx)
{
    int off;
    short* delay;
    unsigned int side;
    off = idx * 0x1c;
    delay = (short*)((char*)&((struct E*)(raw + 0x5bc4))[idx]);
    if (*(unsigned short*)delay != 0) {
        *delay = *(unsigned short*)delay - 1;
        if (*delay < 0) *delay = 0;
        return;
    }
    Sound::PlayBank2_2D(0x188);
    *(char*)(raw + off + 0x5000 + 0xbc8) = 1;
    side = (((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 1;
    side = side >> 0xf;
    *(char*)(raw + off + 0x5000 + 0xbc9) = side;
    if (side != 0) {
        *(char*)(raw + off + 0x5000 + 0xbc7) = 1;
        *(int*)(raw + off + 0x5000 + 0xbb0) = -0x10000;
        *(int*)(raw + off + 0x5000 + 0xbb8) = 0x400;
    } else {
        *(char*)(raw + off + 0x5000 + 0xbc7) = 1;
        *(int*)(raw + off + 0x5000 + 0xbb0) = 0x110000;
        *(int*)(raw + off + 0x5000 + 0xbb8) = -0x400;
    }
    *(int*)(raw + off + 0x5000 + 0xbb4) = -0xf8000;
    *(char*)(raw + off + 0x5000 + 0xbca) = ((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 3 >> 0xf) + 2;
    *(int*)(raw + off + 0x5000 + 0xbbc) = 0x2000;
}
#pragma pop

// @symbol func_ov006_020fb60c
/* Dispatches through the ov006 pointer-to-member table at 0x0214266c. */
void func_ov006_020fb60c(C2 *c){
  if (c->g == 0) return;
  (c->*data_ov006_0214266c[c->idx])(0);
  func_ov006_020faf6c((char *)c, 0);
}

// @symbol func_ov006_020fb670
void func_ov006_020fb670(char *raw)
{
  int targets;
  int i;
  unsigned int rand;
  char *target;
  if ((*((unsigned char *) ((raw + 0x5000) + 0xc34))) != 0)
  {
    return;
  }
  *((unsigned char *) ((raw + 0x5000) + 0xbc6)) = 1;
  *((unsigned char *) ((raw + 0x5000) + 0xbc8)) = 0;
  *((int *) ((raw + 0x5000) + 0xbc0)) = 0;
  rand = (((unsigned int) RandomIntInternal(&data_0209d4b8)) >> 16) & 0x7fff;
  *((short *) ((raw + 0x5b00) + 0xc4)) = (short) ((((rand << 5) >> 15) << 3) + 0x200);
  targets = 0;
  for (i = 0, target = raw; i < 4; i++)
  {
    if ((*((unsigned char *) ((target + 0x5000) + 0xbd4))) != 0)
    {
      targets++;
    }
    target += 0xc;
  }

  if (targets == 1)
  {
    rand = (((unsigned int) RandomIntInternal(&data_0209d4b8)) >> 16) & 0x7fff;
    *((short *) ((raw + 0x5b00) + 0xc4)) = (short) ((((rand << 5) >> 15) << 2) + 0x200);
  }
  unsigned char *pending = (unsigned char *) (((int) raw + 0x5c34));
  *pending = *pending + 1;
}

// @symbol func_ov006_020fb74c
void func_ov006_020fb74c(void* raw) {
    int i;
    char* fx = (char*)raw;
    for (i = 0; i < 30; i++, fx += 0x14) {
        if (*(unsigned char*)(fx + 0x5966)) {
            int type = *(unsigned char*)(fx + 0x5967);
            int x = *(int*)(fx + 0x5958) >> 12;
            int y = *(int*)(fx + 0x595c) >> 12;
            int idx = *(unsigned char*)(fx + 0x5965);
            if (type != 2) {
                func_ov004_020afdd0((void*)data_ov006_02133e10[idx], x, y, -1, -1);
            } else {
                func_ov004_020afdd0((void*)data_ov006_02133e7c[idx], x, y, -1, -1);
            }
        }
    }
}

// @symbol func_ov006_020fb7e0
/* The distinct launder macros force independent RMW base materialization
   (add plus pool).  A plain *(fx + 0x5960) for the compare and zero reloads
   folds to add #0x5900 + #0x60 and loses the ROM's shape. */
void func_ov006_020fb7e0(char *raw)
{
    int i;
    char *fx = raw;
    for (i = 0; i < 0x1e; i++) {
        if (*(u8 *)(fx + 0x5964) != 0) {
            u16 *timer = (u16 *)(int)M1((u32)fx + 0x5960);
            int lastFrame;
            u8 target;
            (*timer)++;
            if (*(u16 *)(fx + 0x5960) >= 6) {
                *(u16 *)(fx + 0x5960) = 0;
                (*(u8 *)(int)M2((u32)fx + 0x5965))++;
                lastFrame = 3;
                if (*(u8 *)(fx + 0x5967) == 2) lastFrame = 5;
                target = *(u8 *)(fx + 0x5968);
                if (target != 0) {
                    if (lastFrame - 2 == *(u8 *)(fx + 0x5965)) {
                        char *q = raw + (target - 1) * 0xc + 0x5000;
                        *(u8 *)(q + 0xbd4) = 0;
                        *(u8 *)(q + 0xbd6) = 1;
                    }
                }
                if (*(u8 *)(fx + 0x5965) >= lastFrame) {
                    *(u8 *)(fx + 0x5966) = 0;
                    *(u8 *)(fx + 0x5964) = 0;
                    if (*(u16 *)(fx + 0x5962) != 0) {
                        func_ov006_020fbb2c(raw, i, *(u16 *)(fx + 0x5962));
                    }
                }
            }
        }
        fx = fx + 0x14;
    }
}

// @symbol func_ov006_020fb8fc
void func_ov006_020fb8fc(char *raw, int x, int y, int kind, int points, int target)
{
    int i = 0;
    char *scan = raw;
    do {
        if (*(unsigned char *)(scan + 0x5964) == 0) {
            char *fx = raw + i * 0x14;
            *(int *)(fx + 0x5958) = x;
            *(int *)(fx + 0x595c) = y;
            *(unsigned char *)(fx + 0x5964) = 1;
            *(unsigned char *)(fx + 0x5966) = 1;
            *(unsigned short *)(fx + 0x5960) = 0;
            *(unsigned char *)(fx + 0x5965) = 0;
            *(unsigned char *)(fx + 0x5967) = kind;
            *(unsigned short *)(fx + 0x5962) = points;
            *(unsigned char *)(fx + 0x5968) = target;
            return;
        }
        i++;
        scan += 0x14;
    } while (i < 0x1e);
}

// @symbol func_ov006_020fb97c
void func_ov006_020fb97c(char* raw){
  volatile unsigned short zero;
  int count = 0;
  int i = 0;
  char* target = raw;
  do {
    if (*(unsigned char*)(target + 0x5bd4) != 0) {
      count++;
      break;
    }
    i++;
    target += 0xc;
  } while (i < 4);
  if (count != 0)
    return;
  PACHINKO(raw)->unk_5c10 = 2;
  PACHINKO(raw)->unk_5c18 = 0x40;
  {
    unsigned chars = G2S::GetBG2CharPtr();
    zero = 0;
    MultiStore16(zero, (char*)chars, 0x6000);
  }
  *(unsigned char*)(raw + 0x5c30) = 0;
  func_ov006_020fe1a8(raw);
  func_ov006_020fc1b4(raw, 1);
}

// @symbol func_ov006_020fba28
/* Render hands it the scene, which it does not read. */
void func_ov006_020fba28(void *){
  func_ov004_020b1a5c(func_ov004_020adbc0(), 6);
}

// @symbol func_ov006_020fba48
/* Draws the high score: the stored best comes back from func_ov004_020adc1c
   as a full word and goes straight to the HUD's number drawer. Render hands
   every drawer the scene; this one does not read it. */
void func_ov006_020fba48(void *)
{
    func_ov004_020b19f0(func_ov004_020adc1c());
}

// @symbol func_ov006_020fba64
void func_ov006_020fba64(char *raw)
{
    int i;
    char *popup = raw;
    for (i = 0; i < 0x1e; i++) {
        if (*(unsigned short *)(popup + 0x4c00 + 0xfa) != 0) {
            func_ov004_020b2444(
                *(int *)(popup + 0x4000 + 0xcf0) >> 0xc,
                *(int *)(popup + 0x4000 + 0xcf4) >> 0xc,
                *(unsigned short *)(popup + 0x4c00 + 0xf8),
                -1, -1, 0, 0);
        }
        popup += 0xc;
    }
}

// @symbol func_ov006_020fbad4
void func_ov006_020fbad4(char *popup)
{
  int i;
  for (i = 0; i < 0x1e; i++)
  {
    char *block = popup + 0x4c00;
    unsigned short v = *((unsigned short *) (block + 0xfa));
    if (v != 0)
    {
      unsigned short *timer = (unsigned short *) (((long long) ((int) (popup + 0x4cfa))));
      *timer = (*timer) - 1;
      if ((*((short *) ((popup + 0x4c00) + 0xfa))) < 0)
      {
        *((short *) ((popup + 0x4c00) + 0xfa)) = 0;
      }
    }
    popup += 0xc;
  }

}

// @symbol func_ov006_020fbb2c
void func_ov006_020fbb2c(char *raw, int idx, unsigned short points)
{
    int i;
    char *popup = raw + 0x15c;
    for (i = 0x1d; i >= 0; popup -= 0xc, i--) {
        if (*(unsigned short *)(popup + 0x4cfa) != 0) continue;
        *(int *)(raw + i * 0xc + 0x4cf0) = *(int *)(raw + idx * 0x14 + 0x5958);
        *(int *)(raw + i * 0xc + 0x4cf4) = *(int *)(raw + idx * 0x14 + 0x595c);
        *(unsigned short *)(raw + i * 0xc + 0x4cfa) = 0x30;
        *(unsigned short *)(raw + 0x4cf8 + i * 0xc) = points;
        func_ov004_020adb1c(*(unsigned short *)(raw + 0x4cf8 + i * 0xc) + func_ov004_020adbc0());
        if ((unsigned int)func_ov004_020adbc0() < 0xbb8) return;
        if (PACHINKO(raw)->mPromptEnabled != 0)
            PACHINKO(raw)->mPromptEnabled = 0;
        return;
    }
}

// @symbol func_ov006_020fbbe8
void func_ov006_020fbbe8(char* raw)
{
    volatile unsigned short zero;
    int shotNo = *(unsigned char*)(raw + 0x5c2f);
    int dx, dy;
    char* shot;

    if (shotNo == 0) return;

    shot = raw + (shotNo - 1) * 0x38;
    dx = 0x80 - (SHOT(shot)->x >> 12);
    dy = 0x20 - (SHOT(shot)->y >> 12);

    if (dx < -6) return;
    if (dx > 6) return;
    if (dy < -6) return;
    if (dy > 6) return;
    if (SHOT(shot)->state != 2) return;

    *(unsigned char*)(raw + 0x5c2f) = 0;
    {
        char* dst = (char*)G2S::GetBG2CharPtr();
        zero = 0;
        MultiStore16(zero, dst, 0x6000);
    }
}

#pragma push
#pragma opt_loop_invariants off
// @symbol func_ov006_020fbcb8
/* Sets a 2x2 block of pixels in the sub BG2 character data. The first
   parameter is never read. */
void func_ov006_020fbcb8(void *a0, int a1, int px, int colour)
{
    int i, j, x, lo, hi, col, row, yb, py;
    u32 *bp;
    py = a1;
    for (i = 0; i < 2; i++)
    {
        x = px - 1 + i;
        lo = x & 7;
        hi = x >> 3;
        col = lo * 4;
        for (j = 0; j < 2; j++)
        {
            row = hi * 32;
            yb = py - 1;
            a1 = yb + j;
            bp = (u32 *)G2S::GetBG2CharPtr();
            *(u32 *)((char *)(bp + (row + (a1 >> 3)) * 8) + col) |= colour << ((a1 & 7) * 4);
        }
    }
}
#pragma pop

#pragma push
#pragma opt_common_subs off
#pragma opt_loop_invariants off
// @symbol func_ov006_020fbd38
void func_ov006_020fbd38(void *arg0)
{
    char *a = (char *)arg0;
    int i;
    int tx;
    int ty;
    int *sx;
    int *sy;
    int *pf00;
    int *pf18;
    int idx;
    int sX0;
    int sX1;
    int sY;
    volatile u16 fill;
    int off;
    int py;
    int cx;
    int cy;
    int dx;
    int adx;
    int dy;
    int ady;
    char *buf;
    int n;

    n = *(u8 *)(a + 0x5c2f);
    if (n == 0)
        return;
    idx = n - 1;
    off = idx * 0x38;
    pf18 = (int *)(a + SHOT_OFF(prevX) + off);
    pf00 = (int *)(a + SHOT_OFF(x) + off);
    if (*pf00 != *pf18 || *(int *)(a + SHOT_OFF(y) + off) != *(int *)(a + SHOT_OFF(prevY) + off)) {
        buf = (char *)G2S::GetBG2CharPtr();
        fill = 0;
        MultiStore16(fill, buf, 0x6000);
        sx = (int *)(a + off + SHOT_OFF(x));
        sy = (int *)(a + off + SHOT_OFF(y));
        sX0 = 0x6c;
        sX1 = 0x94;
        sY = 0x22;
        for (i = 0; i < 2; i++) {
            if (i == 0) {
                cx = sX0;
                tx = *sx >> 12;
                cy = sY;
                py = *sy >> 12;
                if (py >= 0x22) ty = py + 4; else ty = py - 4;
                tx -= 4;
            } else {
                cx = sX1;
                tx = *sx >> 12;
                cy = sY;
                py = *sy >> 12;
                if (py >= 0x22) ty = py + 4; else ty = py - 4;
                tx += 4;
            }
            dx = tx - cx;
            adx = dx;
            if (dx < 0) adx = -dx;
            dy = ty - cy;
            ady = dy;
            if (dy < 0) ady = -dy;
            if (adx >= ady) {
                *(int *)(a + 0x5c14) = adx / 2;
                for (;;) {
                    if (dx == 0) {
                        func_ov006_020fbcb8(a, cx, cy, 2);
                        break;
                    } else if (dx > 0) {
                        cx++;
                        AC += ady;
                        if (*(int *)(a + 0x5c14) > adx) {
                            if (dy >= 0) cy++; else cy--;
                            AC -= adx;
                        }
                        func_ov006_020fbcb8(a, cx, cy, 2);
                        if (cx == tx)
                            break;
                    } else {
                        cx--;
                        AC += ady;
                        if (*(int *)(a + 0x5c14) > adx) {
                            if (dy >= 0) cy++; else cy--;
                            AC -= adx;
                        }
                        func_ov006_020fbcb8(a, cx, cy, 2);
                        if (cx == tx)
                            break;
                    }
                }
            } else {
                *(int *)(a + 0x5c14) = ady / 2;
                for (;;) {
                    if (dy == 0) {
                        func_ov006_020fbcb8(a, cx, cy, 2);
                        break;
                    } else if (dy > 0) {
                        cy++;
                        AC += adx;
                        if (*(int *)(a + 0x5c14) > ady) {
                            if (dx >= 0) cx++; else cx--;
                            AC -= ady;
                        }
                        func_ov006_020fbcb8(a, cx, cy, 2);
                        if (cy == ty)
                            break;
                    } else {
                        cy--;
                        AC += adx;
                        if (*(int *)(a + 0x5c14) > ady) {
                            if (dx >= 0) cx++; else cx--;
                            AC -= ady;
                        }
                        func_ov006_020fbcb8(a, cx, cy, 2);
                        if (cy == ty)
                            break;
                    }
                }
            }
        }
    }
    *pf18 = *pf00;
    *(int *)(a + SHOT_OFF(prevY) + idx * 0x38) = *(int *)(a + SHOT_OFF(y) + idx * 0x38);
}
#pragma pop

// @symbol func_ov006_020fc144
void func_ov006_020fc144(char *raw)
{
    int i;
    char *row = raw;
    for (i = 0; i < 2; i++) {
        if (*(unsigned char *)(row + 0x4000 + 0xeb8) != 0) {
            func_ov004_020afdd0(
                (void *)data_ov006_02136b80[*(unsigned char *)(row + 0x4000 + 0xeb5)],
                *(int *)(row + 0x4000 + 0xea0) >> 0xc,
                *(int *)(row + 0x4000 + 0xea4) >> 0xc,
                -1, 2);
        }
        row += 0x1c;
    }
}

// @symbol func_ov006_020fc1b4
void func_ov006_020fc1b4(char* row, int mode) {
    int i;
    for (i = 0; i < 2; i++) {
        if (*(unsigned char*)(row + 0x4eb7)) {
            *(unsigned char*)(row + 0x4eb3) = 3;
            *(unsigned char*)(row + 0x4eba) = 0;
            *(unsigned char*)(row + 0x4eb4) = (unsigned char)mode;
        }
        row += 0x1c;
    }
}

// @symbol func_ov006_020fc1f8
int func_ov006_020fc1f8(char* raw, int idx)
{
  u16* counter = (u16*)(raw + 0x4eb0 + idx*0x1c);
  u8* frame = (u8*)(raw + 0x4eb6 + idx*0x1c);
  u16 c = *counter;
  *counter = c + 1;
  u8 oldFrame = *frame;
  if (*counter >= data_ov006_0212eb3c[oldFrame]) {
    *counter = 0;
    *frame = *frame + 1;
    if (*frame >= 8) {
      *frame = 0;
      *(u8*)(raw + 0x4eba + idx*0x1c) = *(u8*)(raw + 0x4eba + idx*0x1c) + 1;
    }
    *(u8*)(raw + idx*0x1c + 0x4000 + 0xeb5) = data_ov006_0212eb34[*frame];
  }
  {
    u8* base = (u8*)(raw + idx*0x1c + 0x4000);
    if (base[0xeb4] != 0) {
      base[0xeba] = 0;
      return 0;
    }
  }
  {
    u8* laps = (u8*)(raw + 0x4eba);
    u8 c3 = laps[idx*0x1c];
    if (c3 >= 2) {
      c3 = 0;
      *(u8*)(raw + idx*0x1c + 0x4000 + 0xeb3) = c3;
      laps[idx*0x1c] = c3;
    }
    return c3;
  }
}

// @symbol func_ov006_020fc2ec
void func_ov006_020fc2ec(char* raw, int i)
{
    int off = i * 0x1c;
    int y;

    if (ST(raw, off) != 0) {
        CTA(raw, off)++;
        if (CTA(raw, off) < 4)
            return;
        CTA(raw, off) = 0;
        FB(raw, off)++;
        if (FB(raw, off) >= 3) {
            FB(raw, off) = 0;
            OUTFLAG2(raw, off) = 1;
            ST(raw, off) = 0;
            return;
        }
        OUTFLAG(raw, off) = data_ov006_0212eb10[FB(raw, off)];
        return;
    }

    CTA(raw, off)++;
    if (CTA(raw, off) >= 4) {
        CTA(raw, off) = 0;
        FB(raw, off)++;
        if (FB(raw, off) >= 6) {
            FB(raw, off) = 0;
        }
        OUTFLAG(raw, off) = data_ov006_0212eb1c[FB(raw, off)];
    }

    ACC(raw, off) += VEL(raw, off);
    y = ACC(raw, off) >> 0xc;
    if (y >= data_ov006_0212eb60[i + 2]) {
        if (VEL(raw, off) >= 0x800)
            VEL(raw, off) -= 0x80;
    } else {
        VEL(raw, off) += 0x80;
    }
    if (y < data_ov006_0212eb60[i])
        return;

    ACC(raw, off) = data_ov006_0212eb60[i] << 0xc;
    VEL(raw, off) = -0x1000;
    ST(raw, off)++;
    CTA(raw, off) = 0;
    FB(raw, off) = 0;
    OUTFLAG(raw, off) = data_ov006_0212eb10[0];
}

// @symbol func_ov006_020fc500
void func_ov006_020fc500(char* raw, int i) {
    int off = i * 0x1c;
    int y;

    if (*(u8*)(raw + 0x4eb4 + off) != 0) {
        (*(u16*)(raw + 0x4eb0 + off))++;
        if (*(u16*)(raw + 0x4eb0 + off) < 4) {
            return;
        }
        *(u16*)(raw + 0x4eb0 + off) = 0;
        (*(u8*)(raw + 0x4eb6 + off))++;
        if (*(u8*)(raw + 0x4eb6 + off) >= 3) {
            *(u8*)(raw + 0x4eb6 + off) = 0;
            *(u8*)(raw + 0x4eb3 + off) = 2;
            *(u8*)(raw + 0x4eb4 + off) = 0;
            return;
        }
        *(u8*)(raw + 0x4eb5 + off) = data_ov006_0212eb0c[*(u8*)(raw + 0x4eb6 + off)];
        return;
    }

    (*(u16*)(raw + 0x4eb0 + off))++;
    if (*(u16*)(raw + 0x4eb0 + off) >= 4) {
        *(u16*)(raw + 0x4eb0 + off) = 0;
        (*(u8*)(raw + 0x4eb6 + off))++;
        if (*(u8*)(raw + 0x4eb6 + off) >= 6) {
            *(u8*)(raw + 0x4eb6 + off) = 0;
        }
        *(u8*)(raw + 0x4eb5 + off) = data_ov006_0212eb14[*(u8*)(raw + 0x4eb6 + off)];
    }

    *(s32*)(raw + 0x4ea0 + off) = *(s32*)(raw + 0x4ea0 + off) + *(s32*)(raw + 0x4ea8 + off);
    y = *(s32*)(raw + 0x4ea0 + off) >> 0xc;
    if (y <= data_ov006_0212eb50[i + 2]) {
        if (*(s32*)(raw + 0x4ea8 + off) <= -0x800) {
            *(s32*)(raw + 0x4ea8 + off) = *(s32*)(raw + 0x4ea8 + off) + 0x80;
        }
    } else {
        *(s32*)(raw + 0x4ea8 + off) = *(s32*)(raw + 0x4ea8 + off) - 0x80;
    }

    if (y > data_ov006_0212eb50[i]) {
        return;
    }
    *(s32*)(raw + 0x4ea0 + off) = data_ov006_0212eb50[i] << 0xc;
    *(s32*)(raw + 0x4ea8 + off) = 0x1000;
    (*(u8*)(raw + 0x4eb4 + off))++;
    *(u16*)(raw + 0x4eb0 + off) = 0;
    *(u8*)(raw + 0x4eb6 + off) = 0;
    *(u8*)(raw + 0x4eb5 + off) = data_ov006_0212eb0c[0];
}

#pragma push
#pragma opt_common_subs off
// @symbol func_ov006_020fc718
/* Resets one row of the ov006 table (rows are 0x1c bytes apart) after
   rolling a random kind from data_ov006_0212eb2c.  The row base `raw + off`
   is spelled twice on purpose: the ROM computes it once for the +0x4000
   stores and again, right after the kind reload and before the predicated
   if-else, for the tail stores.  With common-subexpression elimination on,
   mwccarm folds the second into the first and sinks the add below the
   if-else, which recolours the whole tail. */
void func_ov006_020fc718(char *raw, int n)
{
    int rnd = RandomIntInternal(&data_0209d4b8);
    int off = n * 0x1c;
    unsigned int rand = ((unsigned)rnd >> 16) & 0x7fff;
    unsigned int idx = (rand << 1) >> 0xf;
    int kind = data_ov006_0212eb2c[idx];
    unsigned char *kinds = (unsigned char*)(raw + 0x4eb3);
    char *q;
    kinds[off] = (unsigned char)kind;
    q = raw + off + 0x4000;
    *(unsigned char*)(q + 0xeb4) = 0;
    q = raw + off;
    if (kinds[off] == 2) {
        *(unsigned char*)(q + 0x4000 + 0xeb9) = 1;
        *(int*)(q + 0x4000 + 0xea8) = 0x1000;
    } else {
        *(unsigned char*)(q + 0x4000 + 0xeb9) = 0;
        *(int*)(q + 0x4000 + 0xea8) = -0x1000;
    }
    *(short*)(raw + off + 0x4e00 + 0xb0) = 0;
    *(unsigned char*)(raw + off + 0x4000 + 0xeb6) = 0;
    *(unsigned char*)(raw + off + 0x4000 + 0xeb8) = 1;
}
#pragma pop

// @symbol func_ov006_020fc7d0
/* Walks the 8-byte {code pointer, adjustment} records at 0x02142624; an odd
   adjustment selects the virtual spelling through the receiver's own vtable. */
void func_ov006_020fc7d0(char* raw){
  int i=0;
  char* row=raw;
  do{
    if(*(unsigned char*)(row+0x4000+0xeb7)!=0){
      int idx=*(unsigned char*)(row+0x4000+0xeb3);
      Ent7d0* rec=&data_ov006_02142624[idx];
      int adj=rec->b;
      char* obj=raw+(adj>>1);
      int fn;
      if(adj&1){
        fn=*(int*)(*(int*)obj + rec->a);
      } else {
        fn=rec->a;
      }
      ((void(*)(void*,int))fn)(obj,i);
    }
    i++;
    row+=0x1c;
  }while(i<2);
}

// @symbol func_ov006_020fc844
void func_ov006_020fc844(unsigned char* row){
  int i;
  for(i=0;i<2;i++,row+=0x1c){
    *(unsigned char*)(row+0x4eb7)=1;
    *(unsigned char*)(row+0x4eb8)=0;
    *(unsigned char*)(row+0x4eb3)=0;
    *(unsigned char*)(row+0x4eb4)=0;
    *(unsigned char*)(row+0x4eb5)=0;
    *(short*)(row+0x4eb0)=0;
    *(int*)(row+0x4ea0)=data_ov006_0212eb24[i]<<12;
    *(int*)(row+0x4ea4)=0x68000;
    *(int*)(row+0x4ea8)=0;
    *(int*)(row+0x4eac)=0;
    *(unsigned char*)(row+0x4eb9)=0;
    *(unsigned char*)(row+0x4eba)=0;
  }
}

#pragma push
#pragma opt_prelinearize off
static inline int FX_Mul(int v1, int v2)
{
    s64 t = (s64)v1 * v2;
    return (int)((t + 0x800) >> 12);
}

// @symbol func_ov006_020fc8c0
/* Draws the 30 balls. A ball whose byte at +0x2d is set is drawn with the
 * sprite the byte at +0x33 picks, at its position plus the offset at
 * +0x10/+0x14, through a 2x2 rotate-and-scale matrix: the u16 at +0x24 is the
 * angle (>> 4 indexes the shared sine/cosine table) and the 20.12 value at
 * +0x20 is the scale. The products go through FX_Mul; spelled out in the body
 * they put the scale in the first smull's Rm instead of the table value. With
 * opt_prelinearize on, every callee-saved register in the loop rotates by one
 * place. */
void func_ov006_020fc8c0(char *scene)
{
    int i;
    char *ball = scene;
    for (i = 0; i < 30; i++, ball += 0x38) {
        char *p = ball + 0x4000;
        if (*(unsigned char *)(p + 0x68d) != 0) {
            int idx = ((int)*(unsigned short *)(ball + 0x4684) >> 4) << 1;
            int scale = *(int *)(p + 0x680);
            int sprite = *(unsigned char *)(p + 0x693);
            Matrix2x2 m;
            int cosv = FX_Mul(data_02082214[idx + 1], scale);
            int sinv = FX_Mul(data_02082214[idx], scale);
            int x = (*(int *)(p + 0x660) + *(int *)(p + 0x670)) >> 12;
            int y = (*(int *)(p + 0x664) + *(int *)(p + 0x674)) >> 12;
            m._00 = cosv;
            m._01 = sinv;
            m._10 = -sinv;
            m._11 = cosv;
            func_ov004_020b023c((void *)data_ov006_02136cd4[sprite], x, y, -1, &m);
        }
    }
}
#pragma pop

// @symbol func_ov006_020fc9b0
void func_ov006_020fc9b0(char *base, int i)
{
    int x, y, cnt;

    if (*(unsigned char *)(base + 0x4000 + i * 0x38 + 0x68f) == 5) return;
    x = *(int *)(base + 0x4000 + i * 0x38 + 0x660) >> 12;
    y = *(int *)(base + 0x4000 + i * 0x38 + 0x664) >> 12;
    cnt = 0;
    if (y >= 0xd0) cnt++;
    if (x >= 0x120 || x <= -0x20) cnt++;
    if (cnt != 0) {
        *(unsigned char *)(base + 0x4000 + i * 0x38 + 0x68c) = 0;
        *(unsigned char *)(base + 0x4000 + i * 0x38 + 0x68d) = 0;
    }
}

#pragma push
#pragma opt_common_subs off
// @symbol func_ov006_020fca1c
void func_ov006_020fca1c(char *c, int idx)
{
    int i;
    char *p;
    int dx;
    int dy;
    int n;

    if (*(u8 *)(c + idx * 0x38 + 0x468f) == 5)
        return;

    p = c;
    for (i = 0; i < 4; i++, p += 0xc) {
        if (*(u8 *)(p + 0x5bd4) == 0)
            continue;

        dx = (*(int *)(p + 0x5bcc) - *(int *)(c + idx * 0x38 + 0x4660)) >> 12;
        dy = (*(int *)(p + 0x5bd0) - *(int *)(c + idx * 0x38 + 0x4664)) >> 12;
        if (dx < -0x10) continue;
        if (dx > 0x18) continue;
        if (dy < -0x10) continue;
        if (dy > 0x18) continue;

        n = idx * 0x38;
        *(u8 *)(c + n + 0x468c) = 0;
        *(u8 *)(c + n + 0x468d) = 0;
        func_ov006_020fb8fc(c,
                            *(int *)(c + i * 0xc + 0x5bcc),
                            *(int *)(c + i * 0xc + 0x5bd0),
                            2, 0, i + 1);
        func_ov006_020fc1b4(c, 0);
        func_02012718(0x18c, *(int *)(c + n + 0x4000 + 0x660));
        return;
    }
}
#pragma pop

#pragma push
#pragma opt_common_subs off
// @symbol func_ov006_020fcb4c
/* Re-aims entity `i` at a randomly chosen live player. Bails out if no
 * player slot is live, or if the entity has sunk below y=0. Takes the length of the entity's current velocity vector, scales
 * it to 9/10, picks a random live player slot, and rewrites the velocity as
 * that speed pointed along atan2 towards the chosen player, then fires a sound
 * whose pan comes from the entity's x. Slot state at +0x4000+0x68f/0x694.
 *
 * The two velocity-component base offsets must be introduced in the order
 * below: the constant pool emits 0x4668 before 0x466c, and the declaration
 * order is what fixes it. */
void func_ov006_020fcb4c(char *base, int i)
{
    int idx; int n; int factor; int dist; int *pY; int *pVx; int *pVy; char *p; char *angBase; s16 angle;
    int count=0,j; char *q;
    for(j=0,q=base;j<4;j++,q+=0xc){ if(*(u8*)(q+0x5000+0xbd4)!=0){count++;break;} }
    if(!count) return;
    n = i*0x38;
    { char *yBase=base+0x4664; int y=*(int*)(yBase+n); pY=(int*)(yBase+n); if((y>>12)<0) return; }
    {
        char *vyBase = base + 0x4668;
        char *vxBase = base + 0x466c;
        int a = *(int*)(vyBase + n);
        s64 distSq = (s64)a * a;
        int b = *(int*)(vxBase + n);
        pVx = (int*)(vyBase + n);
        distSq += (s64)b * b;
        pVy = (int*)(vxBase + n);
        dist = (int)cstd::sqrt((u64)distSq);
    }
    factor = dist * 9 / 10;
    idx = ((u32)RandomIntInternal(&data_0209d4b8)>>16 & 0x7fff)*4>>15;
    for(;;){
        int t = idx * 0xc;
        p = base + t;
        p = p + 0x5000;
        if(*(u8*)(p+0xbd4)!=0) break;
        idx = ((u32)RandomIntInternal(&data_0209d4b8)>>16 & 0x7fff)*4>>15;
    }
    {
        char *xBase = base + 0x4660;
        int playerY = *(int*)(p + 0xbd0);
        int yval = *pY;
        int playerX = *(int*)(p + 0xbcc);
        int entX = *(int*)(xBase + n);
        angBase = base + 0x4686;
        angle = _ZN4cstd5atan2E5Fix12IiES1_((playerY-yval)>>12, (playerX-entX)>>12);
        *(u16*)(angBase + n) = (u16)angle;
        u16 angU = *(u16*)(angBase + n);
        int angIdx = (angU >> 4) << 1;
        {
            s16 cosv = data_02082214[angIdx + 1];
            *pVx = (int)(((s64)cosv * factor + 0x800) >> 12);
        }
        {
            u16 angU2 = *(u16*)(angBase + n);
            s16 sinv = data_02082214[(angU2 >> 4) << 1];
            *pVy = (int)(((s64)sinv * factor + 0x800) >> 12);
        }
        int fieldByte = *(u8*)(base + n + 0x4000 + 0x696);
        int div40 = (fieldByte * 0x180) / 40;
        int e8ret = func_020126e8(*(int*)(xBase + n));
        func_020126ac(0x186, 6, 0, div40 - 0x80, e8ret);
        *(u8*)(base + n + 0x4000 + 0x68f) = 4;
        *(u8*)(base + n + 0x4000 + 0x694) = (u8)idx;
    }
}
#pragma pop

// @symbol func_ov006_020fcd8c
void func_ov006_020fcd8c(char *thiz, int idx) {
    unsigned char *flag = (unsigned char*)(thiz + 0x4695) + idx * 0x38;
    char *e;
    if (*flag != 0) return;
    e = thiz + idx * 0x38;
    if ((*(int*)(e + 0x4664) >> 0xc) < -0xf2) return;
    *flag = *flag + 1;
    func_02012718(0x184, *(int*)(e + 0x4660));
}

// @symbol func_ov006_020fce04
void func_ov006_020fce04(char *c, int i)
{
    int k = i * 0x38;
    unsigned short *cnt = (unsigned short *)(c + 0x4688 + k);
    if (*cnt != 0) {
        *cnt = *cnt - 1;
        if (*cnt != 0)
            return;
        func_ov006_020fb8fc(c,
                            *(int *)(c + 0x4660 + k),
                            *(int *)(c + k + 0x4000 + 0x664),
                            2,
                            data_ov006_0213d954[0],
                            0);
        func_02012718(0x18b, *(int *)(c + 0x4660 + k));
        return;
    }
    *(unsigned char *)((c + k) + 0x4000 + 0x68c) = 0;
    *(unsigned char *)((c + k) + 0x4000 + 0x68d) = 0;
}

// @symbol func_ov006_020fcec4
void func_ov006_020fcec4(char *c, int i)
{
    int n = i * 0x38;
    int x;
    int y;
    int f;
    short sv;

    *(int *)(c + 0x4660 + n) += *(int *)(c + 0x4668 + n);
    *(int *)(c + 0x4664 + n) += *(int *)(c + 0x466c + n);
    *(unsigned short *)(c + 0x4684 + n) += 0x800;
    if (*(int *)(c + 0x4678 + n) <= 0x30000) {
        *(int *)(c + 0x4678 + n) += 0x800;
    }
    sv = data_02082214[(*(unsigned short *)(c + 0x4684 + n) >> 4) * 2 + 1];
    *(int *)(c + 0x4670 + n) = (int)(((long long)sv * *(int *)(c + 0x4678 + n) + 0x800) >> 12);
    sv = data_02082214[(*(unsigned short *)(c + 0x4684 + n) >> 4) * 2];
    *(int *)(c + 0x4674 + n) = (int)(((long long)sv * *(int *)(c + 0x4678 + n) + 0x800) >> 12);

    x = (*(int *)(c + 0x4660 + n) + *(int *)(c + 0x4670 + n)) >> 12;
    y = (*(int *)(c + 0x4664 + n) + *(int *)(c + 0x4674 + n)) >> 12;
    *(int *)(c + 0x467c + n) = Sound_PlayIfNotActive(*(int *)(c + 0x467c + n), 2, 0x187, 0);
    f = 0;
    if (x >= 0x130 || x <= -0x30) {
        f++;
    }
    if (y >= 0xf0 || y <= -0x110) {
        f++;
    }
    if (f != 0) {
        *(unsigned char *)(c + 0x468c + n) = 0;
        *(unsigned char *)(c + 0x468d + n) = 0;
    }
}

// @symbol func_ov006_020fd088
void func_ov006_020fd088(char *self, int idx)
{
  char *new_var;
  *((int *) ((self + 0x4660) + (idx * 0x38))) = (*((int *) ((self + 0x4660) + (idx * 0x38)))) + (*((int *) ((self + (idx * 0x38)) + 0x4668)));
  new_var = self;
  *((int *) ((new_var + 0x4664) + (idx * 0x38))) = (*((int *) ((new_var + 0x4664) + (idx * 0x38)))) + (*((int *) ((self + (idx * 0x38)) + 0x466c)));
  {
    int raw = *((u16 *) ((new_var + ((idx * 0x38) & 0xFFFFFFFF)) + 0x4686));
    u16 ip = (u16) (raw - 0x4000);
    short tgt = (short) ip;
    short *cur = (short *) ((self + 0x4684) + (idx * 0x38));
    if ((*cur) > tgt)
    {
      *cur = (*cur) - 0x100;
      if ((*cur) <= ((short) ip))
      {
        *cur = ip & 0xFFFFFFFFFFFFFFFF;
      }
    }
    else
      if ((*cur) < ((short) ip))
    {
      *cur = (*cur) + 0x100;
      raw = ip & 0xFFFFFFFFFFFFFFFF;
      if ((*cur) >= ((short) ip))
      {
        *cur = raw;
      }
    }
  }
  func_ov006_020fdaf0(new_var, idx);
  func_ov006_020fca1c(self, idx);
  func_ov006_020fc9b0(self, idx);
}

// @symbol func_ov006_020fd17c
void func_ov006_020fd17c(char *c, int i)
{
    unsigned short t;
    int lim;

    if (*(unsigned short *)(c + i * 0x38 + 0x4688) != 0)
    {
        (*(unsigned short *)(c + 0x4688 + i * 0x38))--;
        if (*(unsigned short *)(c + 0x4688 + i * 0x38) == 0)
        {
            func_02012718(0x185, *(int *)(c + i * 0x38 + 0x4660));
        }
    }

    *(int *)(c + 0x4660 + i * 0x38) += *(int *)(c + 0x4668 + i * 0x38);
    *(int *)(c + 0x4664 + i * 0x38) += *(int *)(c + 0x466c + i * 0x38);

    t = *(unsigned short *)(c + 0x5c28);
    if (t > 12)
    {
        lim = 0xd80 + ((t - 12) << 7);
    }
    else
    {
        lim = 0x600 + t * 0xa0;
    }

    if (*(int *)(c + 0x466c + i * 0x38) <= lim)
    {
        *(int *)(c + 0x466c + i * 0x38) += t * 4 + 8;
    }

    func_ov006_020fdaf0(c, i);
    func_ov006_020fca1c(c, i);
    func_ov006_020fc9b0(c, i);

    *(unsigned short *)(c + 0x4686 + i * 0x38) =
        _ZN4cstd5atan2E5Fix12IiES1_(*(int *)(c + 0x466c + i * 0x38), *(int *)(c + 0x4668 + i * 0x38));
    *(unsigned short *)((unsigned int)c + i * 0x38 + 0x4684) = *(unsigned short *)(c + 0x4686 + i * 0x38) - 0x4000;

    func_ov006_020fcb4c(c, i);
}

// @symbol func_ov006_020fd2d8
void func_ov006_020fd2d8(char *o, int i)
{
    int idx;
    func_ov006_020fcd8c(o, i);
    idx = i * 0x38;
    if (*(unsigned char *)(o + 0x4690 + idx) == 0)
    {
        *(unsigned char *)(o + 0x4690 + idx) += 1;
        if (*(unsigned short *)(o + 0x5c28) > 0xc)
        {
            *(int *)(ATI(o, idx) + 0x466c) =
                ((((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 3) >> 0xf) << 7) + 0xb80
                + ((*(unsigned short *)(o + 0x5c28) - 0xc) << 7);
        }
        else
        {
            *(int *)(ATU(o, idx) + 0x466c) =
                *(unsigned short *)(o + 0x5c28) * 0xa0
                + (((((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 3) >> 0xf) << 7) + 0x400);
        }
        if ((((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 1) >> 0xf) != 0)
        {
            *(int *)(o + idx + 0x4668) = (*(unsigned short *)(o + 0x5c28) << 7) + 0x600;
            *(unsigned char *)(o + idx + 0x4691) = 0;
        }
        else
        {
            *(int *)(o + idx + 0x4668) = -((*(unsigned short *)(o + 0x5c28) << 7) + 0x600);
            *(unsigned char *)(o + idx + 0x4691) = 0;
        }
        *(unsigned short *)((ATI(o, 0) + idx) + 0x4688) =
            ((((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 5) >> 0xf) << 3) + 0x20;
    }
    else
    {
        int v12;
        int *vel;
        *(int *)(o + 0x4664 + idx) += *(int *)(o + idx + 0x466c);
        vel = (int *)(o + 0x4668 + idx);
        *(int *)(o + 0x4660 + idx) += *(int *)(o + 0x4668 + idx);
        v12 = *(int *)(o + 0x4660 + idx) >> 12;
        if (v12 > 0xf0)
        {
            u8 d;
            *vel = -*vel;
            d = *(unsigned char *)(o + 0x4691 + idx);
            if (d != 0)
            {
                if (d == 1)
                    *(unsigned char *)(o + 0x4691 + idx) = 2;
                else
                    *(unsigned char *)(o + 0x4691 + idx) = 1;
            }
            *(int *)(o + 0x4660 + idx) = 0xf0000;
        }
        if (v12 < 0x10)
        {
            u8 d;
            *(int *)(o + 0x4668 + idx) = -*(int *)(o + 0x4668 + idx);
            d = *(unsigned char *)(o + 0x4691 + idx);
            if (d != 0)
            {
                if (d == 1)
                    *(unsigned char *)(o + 0x4691 + idx) = 2;
                else
                    *(unsigned char *)(o + 0x4691 + idx) = 1;
            }
            *(int *)(o + 0x4660 + idx) = 0x10000;
        }
        func_ov006_020fdaf0(o, i);
        func_ov006_020fca1c(o, i);
        func_ov006_020fc9b0(o, i);
        {
            unsigned short t = *(unsigned short *)(o + 0x4688 + idx);
            if (t != 0)
            {
                *(unsigned short *)(o + 0x4688 + idx) = t - 1;
                if (*vel > 0)
                {
                    *(unsigned short *)(o + 0x4684 + idx) -= 0x80;
                    if (*(short *)(o + 0x4684 + idx) <= -0x1800)
                        *(unsigned short *)(o + 0x4684 + idx) = 0xe800;
                }
                else if (*vel < 0)
                {
                    *(unsigned short *)(o + 0x4684 + idx) += 0x80;
                    if (*(short *)(o + 0x4684 + idx) >= 0x1800)
                        *(short *)(o + 0x4684 + idx) = 0x1800;
                }
                if (*(short *)(o + 0x4688 + idx) < 0)
                    *(short *)(o + 0x4688 + idx) = 0;
                return;
            }
            {
                u8 st = *(unsigned char *)(o + 0x4691 + idx);
                if (st == 0)
                {
                    int v = *vel;
                    if (v > 0)
                    {
                        *vel = v - 0x20;
                        *(unsigned short *)(o + 0x4684 + idx) += 0x80;
                        if (*(short *)(o + 0x4684 + idx) >= 0)
                            *(short *)(o + 0x4684 + idx) = 0;
                        if (*vel <= 0)
                        {
                            *(unsigned char *)(o + 0x4691 + idx) =
                                (((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 1) >> 0xf) + 1;
                            *(unsigned short *)(o + 0x4688 + idx) =
                                ((((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 4) >> 0xf) << 2);
                            *(short *)(o + 0x4684 + idx) = 0;
                        }
                    }
                    else if (v < 0)
                    {
                        *vel = v + 0x20;
                        *(unsigned short *)(o + 0x4684 + idx) -= 0x80;
                        if (*(short *)(o + 0x4684 + idx) < 0)
                            *(short *)(o + 0x4684 + idx) = 0;
                        if (*vel >= 0)
                        {
                            *(unsigned char *)(o + 0x4691 + idx) =
                                (((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 1) >> 0xf) + 1;
                            *(unsigned short *)(o + 0x4688 + idx) =
                                ((((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 4) >> 0xf) << 2);
                            *(short *)(o + 0x4684 + idx) = 0;
                        }
                    }
                }
                else if (st == 1)
                {
                    *vel += 0x20;
                    *(unsigned short *)(o + 0x4684 + idx) -= 0x80;
                    if (*(short *)(o + 0x4684 + idx) <= -0x1800)
                        *(unsigned short *)(o + 0x4684 + idx) = 0xe800;
                    if (*vel >= (*(unsigned short *)(o + 0x5c28) << 7) + 0x600)
                    {
                        *(unsigned char *)(o + 0x4691 + idx) = 0;
                        *vel = (*(unsigned short *)(o + 0x5c28) << 7) + 0x600;
                        *(unsigned short *)(o + 0x4688 + idx) =
                            ((((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 5) >> 0xf) << 3) + 0x10;
                    }
                }
                else
                {
                    *vel -= 0x20;
                    *(unsigned short *)(o + 0x4684 + idx) += 0x80;
                    if (*(short *)(o + 0x4684 + idx) >= 0x1800)
                        *(short *)(o + 0x4684 + idx) = 0x1800;
                    if (*vel <= -((*(unsigned short *)(o + 0x5c28) << 7) + 0x600))
                    {
                        *(unsigned char *)(o + 0x4691 + idx) = 0;
                        *vel = -((*(unsigned short *)(o + 0x5c28) << 7) + 0x600);
                        *(unsigned short *)(o + 0x4688 + idx) =
                            ((((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 5) >> 0xf) << 3) + 0x10;
                    }
                }
            }
        }
    }
    func_ov006_020fcb4c(o, i);
}

// @symbol func_ov006_020fd894
void func_ov006_020fd894(char *o, int i)
{
    if (*(unsigned char *)(o + 0x4690 + i * 0x38) == 0)
    {
        *(unsigned char *)(o + 0x4690 + i * 0x38) += 1;
        if (*(unsigned short *)(o + 0x5c28) > 0xc)
        {
            *(int *)(o + i * 0x38 + 0x466c) =
                ((((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 3) >> 0xf) << 7) + 0xb80
                + ((*(unsigned short *)(o + 0x5c28) - 0xc) << 7);
        }
        else
        {
            *(int *)(o + i * 0x38 + 0x466c) =
                *(unsigned short *)(o + 0x5c28) * 0xa0
                + (((((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 3) >> 0xf) << 7) + 0x400);
        }
    }
    else
    {
        *(int *)(o + 0x4664 + i * 0x38) += *(int *)(o + i * 0x38 + 0x466c);
    }
    func_ov006_020fcd8c(o, i);
    func_ov006_020fdaf0(o, i);
    func_ov006_020fca1c(o, i);
    func_ov006_020fc9b0(o, i);
    func_ov006_020fcb4c(o, i);
}

// @symbol func_ov006_020fd9cc
void func_ov006_020fd9cc(char *base, int idx)
{
    int off = idx * 0x38;
    unsigned short *timer = (unsigned short *)(base + 0x4688 + off);
    unsigned int r;
    char *o;
    if (*timer != 0) {
        *timer = *timer - 1;
        return;
    }
    r = RandomIntInternal(&data_0209d4b8);
    o = base + off;
    *(unsigned char *)(o + 0x468f) = ((((r >> 16) & 0x7fff) << 1) >> 0xf) + 1;
    *(unsigned char *)(o + 0x4690) = 0;
    *(unsigned char *)(o + 0x468d) = 1;
    *(unsigned char *)(o + 0x4693) = 0;
    *(unsigned short *)(o + 0x4684) = 0;
    *(int *)(o + 0x4678) = 0;
    *(int *)(o + 0x4670) = 0;
    *(int *)(o + 0x4674) = 0;
    *(int *)(o + 0x4680) = 0x1000;
    *(unsigned char *)(o + 0x4695) = 0;
}

// @symbol func_ov006_020fda7c
void func_ov006_020fda7c(char* c){
  int i=0;
  char* r5=c;
  do{
    if(*(unsigned char*)(r5+0x468c)!=0){
      int idx=*(unsigned char*)(r5+0x468f);
      Ent7d0* e=&data_ov006_02142694[idx];
      int adj=e->b;
      char* obj=c+(adj>>1);
      int fn;
      if(adj&1){
        fn=*(int*)(*(int*)obj + e->a);
      } else {
        fn=e->a;
      }
      ((void(*)(void*,int))fn)(obj,i);
    }
    i++;
    r5+=0x38;
  }while(i<30);
}

/* Finds the first live entry within 0x18 of
 * the caller's entry and launch it.
 *
 * Walks the 0x30 records of stride 0x38 that begin at self+0x4000. A record is
 * a candidate when its flag byte at +0x4f0c is non-zero and its state byte at
 * +0x4f0d is at least 2. For each candidate the planar distance from record
 * `i`'s position (self + i*0x38 + 0x4660 for x, +0x4664 for z) is taken in
 * whole units (>> 12) and compared against 0x18 through cstd::sqrt of the
 * 64-bit sum of squares. The first record inside that radius wins: the scan
 * writes its heading into self+0x4684+i*0x38, derives a sine/cosine pair from
 * data_02082214 at 0x2000 scale plus a zero velocity pair into the record at
 * self+i*0x38+0x4000+0x668..0x674, plays the sound whose id comes from
 * data_ov006_0212eb94 indexed by the hit record's counter (clamped at 5),
 * spawns the effect through func_ov006_020fb8fc, bumps that counter, and
 * returns. With no hit the loop runs out and the function returns having
 * written nothing.
 *
 * Two spellings are load-bearing for the register colouring in the guard
 * block, and only TOGETHER -- each one alone scores worse than either does
 * apart (16 and 11 against 7):
 *
 *   1. The four record fields in the guard block are reached off `walk` with
 *      the +0x4000 already folded into the constant (0x4f0c, 0x4f0d, 0x4ed8,
 *      0x4edc). Introducing a `char *w = walk + 0x4000` local and using
 *      w+0xf0c... is the same address arithmetic but creates a named web that
 *      takes r2 away from the loaded x.
 *   2. The z difference is computed BEFORE the x difference. mwccarm schedules
 *      the z term first either way, but the source order decides which of the
 *      two competing values reaches the allocator first, and with it whether
 *      the record base lands in r2 and the loaded x in r3 (the cartridge) or
 *      the other way round.
 */


// @symbol func_ov006_020fdaf0
void func_ov006_020fdaf0(char *base, int i)
{
    int n = i * 0x38;
    char *o = base;
    int j = 0;
    char *walk = o;
    int *xp = (int *)((o + n) + 0x4660);
    int *zp = (int *)((o + n) + 0x4664);
    do {
        if (*(u8 *)(walk + 0x4f0c) == 0)
            goto next;
        if (*(u8 *)(walk + 0x4f0d) < 2)
            goto next;
        {
            int dy = (*zp - *(int *)(walk + 0x4edc)) >> 12;
            int dx = (*xp - *(int *)(walk + 0x4ed8)) >> 12;
            s64 distsq = (s64)dx * dx + (s64)dy * dy;
            if (cstd::sqrt((unsigned long long)distsq) > 0x18)
                goto next;
        }
        {
            int m = j * 0x38;
            int five = 5;
            char *ov = (char *)o;
            unsigned char *f0f = (unsigned char *)(o + 0x4f0f);
            char *bn = ov + n;
            char *bm = ov + m;
            char *ip = bn; ip += 0x4000;
            u16 ang;
            s16 s;
            s16 c;
            int vel;
            int cnt;
            int cc;
            char *p84;
            char *p78;
            char *p60;
            unsigned char *cntp;
            int round = 0x800;

            *(u8 *)(ip + 0x68f) = (u8)five;
            {
                char *r = bm + 0x4f00;
                ang = *(u16 *)(r + 8);
                r = o + 0x4684;
                p84 = r;
            }
            *(u16 *)(p84 + n) = ang;
            ang = *(u16 *)(p84 + n);

            s = data_02082214[((ang >> 4) << 1) + 1];
            {
                unsigned char *lr = f0f;
                p78 = o + 0x4678;
                *(int *)(ip + 0x668) = (int)((((s64)s * 0x2000) + round) >> 12);
                ang = *(u16 *)(p84 + n);
                c = data_02082214[(ang >> 4) << 1];
                *(int *)(ip + 0x66c) = (int)((((s64)c * 0x2000) + round) >> 12);
                *(int *)(p78 + n) = 0;
                ang = *(u16 *)(p84 + n);
                vel = *(int *)(p78 + n);
                s = data_02082214[((ang >> 4) << 1) + 1];
                *(int *)(ip + 0x670) = (int)((((s64)s * (s64)vel) + round) >> 12);
                ang = *(u16 *)(p84 + n);
                vel = *(int *)(p78 + n);
                c = data_02082214[(ang >> 4) << 1];
                *(int *)(ip + 0x674) = (int)((((s64)c * (s64)vel) + round) >> 12);
                cnt = *(u8 *)(lr + m);
                cc = cnt;
                if (cc >= 5)
                    cc = five;
                p60 = o + 0x4660;
                func_02012718(data_ov006_0212eb94[cc], *(int *)(p60 + n));
                {
                    char *q = (char *)o + n;
                    func_ov006_020fb8fc(o,
                        *(int *)(p60 + n),
                        *(int *)(q + 0x4664),
                        2,
                        (int)data_ov006_0213d954[cc],
                        0);
                }
                cntp = f0f + m;
                *cntp = (u8)(*cntp + 1);
            }
            return;
        }
    next:
        j += 1;
        walk += 0x38;
    } while (j < 0x30);
}

// @symbol func_ov006_020fdd40
void func_ov006_020fdd40(dScMgPachinko_c *self)
{
    int i;
    int ones;
    int tens;

    if (self->unk_5c24 != 0) {
        self->unk_5c24--;
        if ((s16)self->unk_5c24 < 0) self->unk_5c24 = 0;
        return;
    }
    self->unk_5c24 = 0x50;
    for (i = 0; i < 0x1e; i++) {
        int ra;
        int rb;
        if (self->mBall[i].active != 0) continue;
        self->mBall[i].active = 1;
        ra = RandomIntInternal(&data_0209d4b8);
        rb = RandomIntInternal(&data_0209d4b8);
        self->mBall[i].x = (data_ov006_0212eb80[(((u32)rb >> 16) & 0x7fff) * 5 >> 15] + ((((u32)ra >> 16) & 0x7fff) * 4 >> 15) * 8) << 12;
        self->mBall[i].y = -0x100000;
        self->mBall[i].unk08 = 0;
        self->mBall[i].unk0c = 0;
        self->mBall[i].state = 0;
        self->mBall[i].unk30 = 0;
        ra = RandomIntInternal(&data_0209d4b8);
        self->mBall[i].timer = ((((u32)ra >> 16) & 0x7fff) * 8 >> 15) << 4;
        self->mBall[i].unk24 = 0;
        self->mBall[i].unk1c = 0;
        self->mBall[i].unk36 = self->unk_5c28;
        self->unk_5c26++;
        if (self->unk_5c28 == 0) break;
        {
            int r = (((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 16 >> 15;
            int lvl = self->unk_5c28 >> 1;
            if (lvl >= 4) lvl = 4;
            if (data_ov006_0213d974[r + lvl * 4] == 0) break;
        }
        if ((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 2 >> 15) {
            self->mBall[i].x = 0x100000;
            self->mBall[i].angle = 0xc000 - (((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 0xc >> 15) + 1 << 9);
        } else {
            self->mBall[i].x = 0;
            self->mBall[i].angle = (((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 0xc >> 15) + 1 << 9) + 0xc000;
        }
        self->mBall[i].unk18 = 0;
        self->mBall[i].unk10 = 0;
        self->mBall[i].unk14 = 0;
        self->mBall[i].unk24 = self->mBall[i].angle - 0x4000;
        self->mBall[i].y = -0x60000;
        self->mBall[i].unk08 = FMUL(data_02082214[(self->mBall[i].angle >> 4) * 2 + 1], 0xe80);
        self->mBall[i].unk0c = FMUL(data_02082214[(self->mBall[i].angle >> 4) * 2], 0xe80);
        self->mBall[i].state = 3;
        self->mBall[i].unk2d = 1;
        self->mBall[i].unk33 = 0;
        self->mBall[i].unk20 = 0x1000;
        self->mBall[i].timer = 0x10;
        break;
    }
    ones = self->unk_5c26;
    for (tens = 0; ones >= 0xa; tens++) ones -= 0xa;
    if (tens != 0 && ones == 0) self->unk_5c28++;
    if (self->unk_5c28 > 0x28) self->unk_5c28 = 0x27;
    self->unk_5c24 -= self->unk_5c28 << 2;
    if ((s16)self->unk_5c24 <= 0x20) self->unk_5c24 = 0x20;
}

// @symbol func_ov006_020fe1a8
void func_ov006_020fe1a8(char *p)
{
    int i;
    for (i = 0; i < 0x30; i++) {
        *(unsigned char *)(p + 0x4f0c) = 0;
        *(unsigned char *)(p + 0x4f0e) = 0;
        p += 0x38;
    }
}

// @symbol func_ov006_020fe1d0
void func_ov006_020fe1d0(char *thiz) {
    int i;
    for (i = 0; i < 0x30; i++, thiz += 0x38) {
        if (*(unsigned char*)(thiz + 0x4f0e) == 0) continue;
        func_ov004_020aff38((OamAttr *)data_ov006_02137614,
                            *(int*)(thiz + 0x4ed8) >> 0xc,
                            *(int*)(thiz + 0x4edc) >> 0xc,
                            -1, -1, 0x1000, 0);
    }
}

// @symbol func_ov006_020fe248
void func_ov006_020fe248(char* c){
  int i=0;
  char* r5=c;
  do{
    if(*(unsigned char*)(r5+0x4f0c)!=0){
      int idx=*(unsigned char*)(r5+0x4f0d);
      Ent7d0* e=&data_ov006_02142644[idx];
      int adj=e->b;
      char* obj=c+(adj>>1);
      int fn;
      if(adj&1){
        fn=*(int*)(*(int*)obj + e->a);
      } else {
        fn=e->a;
      }
      ((void(*)(void*,int))fn)(obj,i);
    }
    i++;
    r5+=0x38;
  }while(i<48);
}

// @symbol func_ov006_020fe2bc
void func_ov006_020fe2bc(char *c)
{
    if (*(unsigned char *)(c + 0x5000 + 0xc32) == 0)
        *(unsigned char *)(c + 0x5c32) += 1;
}

// @symbol func_ov006_020fe2e4
void func_ov006_020fe2e4(char *self, int i)
{
    unsigned int idx = gActivePlayerSlot;
    int flag = 0;
    char *p;
    int dx, dy;

    if (gTouchHeld[idx][0] != 0) {
        flag = gTouchEdge[idx][0] != 0;
    }
    if (flag != 0) {
        p = self + i * 0x38;
        dx = ((*(s32 *)(p + 0x4ed8)) >> 12) - gTouchX[idx][0];
        dy = ((*(s32 *)(p + 0x4edc)) >> 12) - gTouchY[idx][0];
        *(unsigned char *)(p + 0x4f0d) = 1;
        *(s32 *)(p + 0x4ee8) = dx << 12;
        *(s32 *)(p + 0x4eec) = dy << 12;
        *(unsigned char *)(p + 0x4f0f) = 0;
    }
    func_ov006_020fbd38(self);
}

#pragma push
#pragma opt_propagation off
// @symbol func_ov006_020fe394
/* One ball's stylus frame. While the stylus is down the ball follows the touch
 * point through its grab offset, clamped into the 8..0xf8 x 8..0xb8 box, the
 * offset is re-derived, and the drag-speed sound is re-armed at 0x17b or 0x17c
 * depending on whether the distance from the launcher grew by more than ten.
 * On release the launch fires: the pull-back vector becomes an angle at 0x4f08
 * and a speed at 0x4ef8 (the square root scaled by 1.953, clamped to 0xa000),
 * that speed drives the sine table into the velocity pair at 0x4ee0/0x4ee4, and
 * the ball is re-seated at 0x80000, 0x28000 if the pull was too short.
 *
 * Four spellings only move registers, and each was measured:
 * - dy is taken before dx in both distance blocks;
 * - the second sine read goes through a named `const s16 *` (non-const changes
 *   the size; doing the same to the first read is worse), which keeps the two
 *   reads of data_02082214 from sharing one table base;
 * - the last y clamp stores through `py`, assigned between the two x clamps;
 * - the first velocity word's table index is a block-scope local, not inline
 *   and not at function scope.
 * `opt_propagation off` was measured against deleting it outright.
 */
void func_ov006_020fe394(dScMgPachinko_c *self, int i)
{
    char *c = (char *)self;
    s32 cx;
    s32 *py;
    s32 cy;
    u32 idx = gActivePlayerSlot;

    if (gTouchHeld[idx][0] != 0) {
        int off = i * 0x38;
        s32 vy, vx;

        cx = gTouchX[idx][0];
        cy = gTouchY[idx][0];
        *(s32 *)(c + 0x4ed8 + off) = (cx << 12) + *(s32 *)(c + 0x4ee8 + off);
        *(s32 *)(c + 0x4edc + off) = (cy << 12) + *(s32 *)(c + 0x4eec + off);

        vy = *(s32 *)(c + 0x4edc + off) >> 12;
        vx = *(s32 *)(c + 0x4ed8 + off) >> 12;
        if (vx >= 0xf8)
            *(s32 *)(c + 0x4ed8 + off) = 0xf8000;
        py = (s32 *)(c + 0x4edc + off);
        if (vx <= 8)
            *(s32 *)(c + 0x4ed8 + off) = 0x8000;
        if (vy >= 0xb8)
            *py = 0xb8000;
        if (vy <= 8)
            *py = 0x8000;

        {
            s32 nx = (*(s32 *)(c + 0x4ed8 + off) >> 12) - gTouchX[gActivePlayerSlot][0];
            s32 ny = (*(s32 *)(c + 0x4edc + off) >> 12) - gTouchY[gActivePlayerSlot][0];

            *(s32 *)(c + 0x4ee8 + off) = nx << 12;
            *(s32 *)(c + 0x4eec + off) = ny << 12;
        }

        {
            s32 dy = 0x80 - (*(s32 *)(c + 0x4ed8 + off) >> 12);
            s32 dx = 0x20 - (*(s32 *)(c + 0x4edc + off) >> 12);
            s32 dist = cstd::sqrt((u64)(s64)(dy * dy + dx * dx));
            s32 prev = *(s32 *)(c + 0x4f04 + off);

            *(s32 *)(c + 0x4f04 + off) = dist;
            if (dist > prev) {
                if (dist > prev + 10)
                    *(s32 *)(c + 0x4f00 + off) = Sound_PlayIfNotActive(
                        *(s32 *)(c + 0x4f00 + off), 2, 0x17b, 0);
                else
                    *(s32 *)(c + 0x4f00 + off) = Sound_PlayIfNotActive(
                        *(s32 *)(c + 0x4f00 + off), 2, 0x17c, 0);
            }
        }
    } else {
        int off = i * 0x38;
        s32 dy, dx, dist;

        *(u8 *)(c + 0x4f0d + off) = 2;
        dy = 0x80 - (*(s32 *)(c + 0x4ed8 + off) >> 12);
        dx = 0x20 - (*(s32 *)(c + 0x4edc + off) >> 12);
        dist = cstd::sqrt((u64)(s64)(dy * dy + dx * dx));
        if (dist >= 0x10) {
            {
                s16 ang = _ZN4cstd5atan2E5Fix12IiES1_(dx, dy);
                char *p = c + off;

                *(s16 *)(p + 0x4f08) = ang;
            }
            *(s32 *)(c + 0x4ef8 + off) =
                cstd::sqrt((u64)(s64)(dy * dy + dx * dx)) << 8;
            *(s32 *)(c + 0x4ef8 + off) += *(s32 *)(c + 0x4ef8 + off) >> 2;
            *(s32 *)(c + 0x4ef8 + off) += *(s32 *)(c + 0x4ef8 + off) >> 2;
            *(s32 *)(c + 0x4ef8 + off) += *(s32 *)(c + 0x4ef8 + off) >> 2;
            if (*(s32 *)(c + 0x4ef8 + off) >= 0xa000)
                *(s32 *)(c + 0x4ef8 + off) = 0xa000;
            self->unk_5c1c = (u16)(*(s32 *)(c + 0x4ef8 + off) >> 10);
            if (self->unk_5c1c == 0)
                self->unk_5c1c = 1;
            *(u8 *)(c + 0x5c31) = 0;
            {
                int ai = (*(u16 *)(c + 0x4f08 + off) >> 4) * 2 + 1;
                *(s32 *)(c + 0x4ee0 + off) = (s32)(((s64)data_02082214[ai] *
                    *(s32 *)(c + 0x4ef8 + off) + 0x800) >> 12);
            }
            {
                const s16 *sp = &data_02082214[(*(u16 *)(c + 0x4f08 + off) >> 4) * 2];
                *(s32 *)(c + 0x4ee4 + off) = (s32)(((s64)*sp * *(s32 *)(c + 0x4ef8 + off) + 0x800) >> 12);
            }
            *(s16 *)(c + 0x4f0a + off) = 0;
            if (dist >= 0x40)
                Sound::PlayBank2_2D(0x17e);
            else
                Sound::PlayBank2_2D(0x17d);
        } else {
            *(s32 *)(c + 0x4ed8 + off) = 0x80000;
            *(s32 *)(c + 0x4edc + off) = 0x28000;
            *(u8 *)(c + 0x4f0d + off) = 0;
        }
    }
    func_ov006_020fbd38(self);
}
#pragma pop

// @symbol func_ov006_020fe750
void func_ov006_020fe750(char* c, int idx)
{
    int n = idx * 0x38;
    int old;
    int e, b;

    *(int*)(c + 0x4ed8 + n) = *(int*)(c + 0x4ed8 + n) + *(int*)(c + 0x4ee0 + n);
    *(int*)(c + 0x4edc + n) = *(int*)(c + 0x4edc + n) + *(int*)(c + 0x4ee4 + n);
    *(int*)(c + 0x4efc + n) = *(int*)(c + 0x4efc + n) + 0x10;
    old = *(int*)(c + 0x4ee4 + n);
    *(int*)(c + 0x4ee4 + n) = old + *(int*)(c + 0x4efc + n);
    if (*(int*)(c + 0x4ee4 + n) >= 0x8000)
        *(int*)(c + 0x4ee4 + n) = 0x8000;

    *(s16*)(c + 0x4f08 + n) = _ZN4cstd5atan2E5Fix12IiES1_(
        *(int*)(c + 0x4ee4 + n) >> 12,
        *(int*)(c + 0x4ee0 + n) >> 12);

    if (old < 0) {
        if (*(int*)(c + 0x4ee4 + n) >= 0)
            *(s16*)(c + 0x4f0a + n) = 0x40;
    }

    if (*(u16*)(c + 0x4f0a + n) != 0) {
        *(volatile u16*)(c + 0x4f0a + n) = *(volatile u16*)(c + 0x4f0a + n) - 1;
        if (*(s16*)(c + 0x4f0a + n) <= 0)
            *(u8*)(c + 0x4f0d + n) = 4;
    }

    b = *(int*)(c + 0x4ed8 + n) >> 12;
    e = *(int*)(c + 0x4edc + n) >> 12;
    if (e <= -0x120)
        *(u8*)(c + 0x4f0d + n) = 4;
    if (e >= 0xd0)
        *(u8*)(c + 0x4f0d + n) = 4;
    if (b >= 0x140 || b <= -0x40)
        *(u8*)(c + 0x4f0d + n) = 4;

    func_ov006_020fbbe8(c);
    if (*(u8*)(c + 0x5c2f) != 0)
        func_ov006_020fbd38(c);
}

// @symbol func_ov006_020fe90c
void func_ov006_020fe90c(char* base, int i)
{
    int n = i * 0x38;
    s16 t;
    u16 v;
    *(int*)(base + 0x4ef8 + n) = *(int*)(base + 0x4ef8 + n) + *(int*)(base + 0x4efc + n);
    *(int*)(base + 0x4efc + n) = *(int*)(base + 0x4efc + n) + 0x10;
    *(int*)(base + 0x4ed8 + n) = *(int*)(base + 0x4ed8 + n)
        + (int)(((s64)data_02082214[((*(u16*)(base + 0x4f08 + n) >> 4) << 1) + 1] * 0x800 + 0x800) >> 0xc);
    t = data_02082214[(*(u16*)(base + 0x4f08 + n) >> 4) << 1];
    *(int*)(base + 0x4edc + n) = *(int*)(base + 0x4edc + n)
        + (int)(((s64)t * *(int*)(base + 0x4ef8 + n) + 0x800) >> 0xc);
    v = *(u16*)(base + 0x4f0a + n);
    if (v != 0) {
        *(s16*)(base + 0x4f0a + n) = v - 1;
        if (*(s16*)(base + 0x4f0a + n) < 0)
            *(s16*)(base + 0x4f0a + n) = 0;
    } else {
        *(unsigned char*)(base + 0x4f0d + n) = 4;
    }
}

// @symbol func_ov006_020fea54
void func_ov006_020fea54(char *p, int idx) {
    *(p + idx * 0x38 + 0x4000 + 0xf0c) = 0;
    *(p + idx * 0x38 + 0x4000 + 0xf0e) = 0;
}

// @symbol func_ov006_020fea70
void func_ov006_020fea70(char *o)
{
    int i;
    char *p;

    if (*(unsigned short *)(o + 0x5c1c) == 0)
        return;
    *(unsigned short *)(o + 0x5c1c) -= 1;
    if (*(short *)(o + 0x5c1c) > 0)
        return;
    *(unsigned short *)(o + 0x5c1c) = 0;

    p = o;
    for (i = 0; i < 0x30; i++)
    {
        if (*(unsigned char *)(p + 0x4f0c) == 0)
        {
            /* Two-step base: mla into temp then add #0x4000 into r3 (not ip). */
            p = o + i * 0x38;
            p = p + 0x4000;
            *(unsigned char *)(p + 0xf0c) = 1;
            *(unsigned char *)(p + 0xf0e) = 1;
            *(unsigned char *)(p + 0xf0d) = 0;
            *(int *)(p + 0xefc) = 0;
            *(int *)(p + 0xed8) = 0x80000;
            *(int *)(p + 0xedc) = 0x28000;
            *(int *)(p + 0xee8) = 0;
            *(int *)(p + 0xeec) = 0;
            *(unsigned char *)(p + 0xf0f) = 0;
            *(int *)(p + 0xee0) = 0;
            *(int *)(p + 0xee4) = 0;
            *(unsigned short *)(o + i * 0x38 + 0x4f0a) = 0;
            *(int *)(p + 0xf00) = 0;
            *(int *)(p + 0xf04) = 4;
            *(unsigned char *)(o + 0x5c2f) = (unsigned char)(i + 1);
            if (*(unsigned short *)(o + 0x5c20) != 0)
            {
                Sound::PlayBank2_2D(0x18d);
                return;
            }
            *(unsigned short *)(o + 0x5c20) += 1;
            return;
        }
        p += 0x38;
    }
}

// @symbol func_ov006_020feba8
void func_ov006_020feba8(char *self)
{
    int i, j, k, l, m, n;
    char *p, *q, *r, *s, *t, *u;

    *(int *)(self + 0x5c10) = 0;

    p = self;
    for (i = 0; i < 30; i++) {
        *(int *)(p + 0x4cf0) = 0;
        *(int *)(p + 0x4cf4) = 0;
        *(short *)(p + 0x4cf8) = 0;
        *(short *)(p + 0x4cfa) = 0;
        p += 0xc;
    }

    q = self;
    for (j = 0; j < 3; j++) {
        *(char *)(q + 0x4e6c) = 0;
        *(char *)(q + 0x4e6d) = 0;
        q += 0x18;
    }

    r = self;
    for (k = 0; k < 2; k++) {
        *(int *)(r + 0x4ea0) = 0;
        *(int *)(r + 0x4ea4) = 0;
        *(int *)(r + 0x4ea8) = 0;
        *(int *)(r + 0x4eac) = 0;
        *(short *)(r + 0x4eb0) = 0;
        *(char *)(r + 0x4eb5) = 0;
        *(char *)(r + 0x4eb3) = 0;
        *(char *)(r + 0x4eb2) = 0;
        *(char *)(r + 0x4eb7) = 0;
        *(char *)(r + 0x4eb8) = 0;
        r += 0x1c;
    }

    s = self;
    for (l = 0; l < 0x30; l++) {
        *(int *)(s + 0x4ed8) = 0;
        *(int *)(s + 0x4edc) = 0;
        *(int *)(s + 0x4ef0) = 0;
        *(int *)(s + 0x4ef4) = 0;
        *(int *)(s + 0x4ef8) = 0;
        *(int *)(s + 0x4efc) = 0;
        *(short *)(s + 0x4f08) = 0;
        *(short *)(s + 0x4f0a) = 0;
        *(char *)(s + 0x4f0c) = 0;
        *(char *)(s + 0x4f0d) = 0;
        *(char *)(s + 0x4f0e) = 0;
        s += 0x38;
    }

    t = self;
    for (m = 0; m < 30; m++) {
        *(char *)(t + 0x468c) = 0;
        *(char *)(t + 0x468d) = 0;
        *(char *)(t + 0x468f) = 0;
        t += 0x38;
    }

    u = self;
    for (n = 0; n < 30; n++) {
        *(char *)(u + 0x5964) = 0;
        *(char *)(u + 0x5966) = 0;
        u += 0x14;
    }

    *(char *)(self + 0x5bc6) = 0;
    *(char *)(self + 0x5bc8) = 0;
    *(char *)(self + 0x5bc7) = 0;
    *(short *)(self + 0x5c18) = 0;
    *(short *)(self + 0x5c22) = 0;
    *(short *)(self + 0x5c1e) = 0;
    *(char *)(self + 0x5c30) = 0;
    *(char *)(self + 0x5c31) = 0;
    *(char *)(self + 0x5c32) = 0;
    *(short *)(self + 0x5c24) = 0;
    *(char *)(self + 0x5c34) = 0;
    *(short *)(self + 0x5c26) = 0;
    *(short *)(self + 0x5c28) = 0;
    *(short *)(self + 0x5c2a) = 0;
    *(short *)(self + 0x5c20) = 0;

    func_ov004_020adb1c(0);
    func_ov006_020faeec(self);
    func_ov006_020fadfc(self);
    func_ov006_020fad90(self);
}

}  /* extern "C" */

// @symbol _ZN15dScMgPachinko_c13OnYoshiTryEatEi
/* Vtable slot 18, an override of
   dScMgBase_c::OnYoshiTryEat(int). The signature must repeat the base
   declaration exactly, or mwcc appends a slot instead of overriding. */
void dScMgPachinko_c::OnYoshiTryEat(int n)
{
    /* The score increment reads unk_0bc through a const view of `this`.
       Without it mwcc CSEs the +0xbc field address into its own register
       (add r2,r4,#0xbc / ldr [r2] / str [r2]) and the function grows a
       word; the cartridge re-issues ldr r1,[r4,#0xbc] / str r1,[r4,#0xbc].
       Same lever as dScMgPachinko2_c::OnYoshiTryEat. */
    const dScMgPachinko_c *ro = this;

    unk_5c10 = 0;
    if (n == 9) {
        unk_0bc = ro->unk_0bc + 1;
        if (unk_0bc > 0x270e) unk_0bc = 0x270e;
    } else {
        unk_0bc = 0;
        if (unk_0bc > 0x270e) unk_0bc = 0x270e;
    }
    func_ov006_020fadfc((char *)this);
    func_ov006_020fad90((char *)this);
}

// @symbol _ZN15dScMgPachinko_c6RenderEv
/* Vtable slot 9. */
s32 dScMgPachinko_c::Render()
{
    func_ov006_020fba48(this);
    func_ov006_020fba28(this);
    func_ov006_020fb74c(this);
    func_ov006_020fba64((char *)this);
    func_ov006_020fe1d0((char *)this);
    func_ov006_020faf14((char *)this);
    func_ov006_020fc8c0((char *)this);
    func_ov006_020fae20((char *)this);
    func_ov006_020fc144((char *)this);
    func_ov006_020fa7b8((char *)this);
    return 1;
}

// @symbol _ZN15dScMgPachinko_c8BehaviorEv
/* Vtable slot 6. */
s32 dScMgPachinko_c::Behavior()
{
    char *c = (char *)this;

    switch (*(s32 *)(c + 0x5c10)) {
    case 0:
        FreeGfxSlotsById(0x1d);
        func_ov006_020feba8(c);
        func_ov006_020fc844((u8 *)c);
        func_ov006_020fae90((u8 *)c);
        *(s32 *)(c + 0x5c10) = 1;
        *(u16 *)(c + 0x5c1c) = 0x10;
        *(u16 *)(c + 0x5c24) = 0x60;
        break;
    case 1:
        if (*(u8 *)(c + 0xc4) == 0) {
            *(u8 *)(c + 0xc3) = 1;
            *(u8 *)(c + 0xc4) = 1;
            *(u16 *)(c + 0xc0) = 0;
        }
        if (*(u16 *)(c + 0x5c2a) != 0) {
            func_ov006_020fb7e0(c);
            (*(u16 *)(c + 0x5c2a))--;
        } else {
            func_ov006_020fdd40(this);
            func_ov006_020fe2bc(c);
            func_ov006_020fea70(c);
            func_ov006_020fe248(c);
            func_ov006_020fda7c(c);
            func_ov006_020fb670(c);
            func_ov006_020fb60c((C2 *)c);
            func_ov006_020fc7d0(c);
            func_ov006_020fbad4(c);
            func_ov006_020fb97c(c);
            func_ov006_020fb7e0(c);
        }
        break;
    case 2:
        if (*(u16 *)(c + 0x5c18) != 0) {
            (*(u16 *)(c + 0x5c18))--;
            if (*(s16 *)(c + 0x5c18) <= 0) {
                func_ov004_020b0a54(0x10);
                *(u8 *)(c + 0xc3) = 0;
            }
        }
        func_ov006_020fb7e0(c);
        func_ov006_020fda7c(c);
        func_ov006_020fb60c((C2 *)c);
        func_ov006_020fc7d0(c);
        func_ov006_020fbad4(c);
        break;
    }
    func_ov006_020fad34((C *)c);
    return 1;
}

// @symbol _ZN15dScMgPachinko_c13InitResourcesEv
/* Vtable slot 0. */
s32 dScMgPachinko_c::InitResources()
{
    char *c = (char *)this;
    char *b;
    char *dst;
    volatile u16 fillScr;
    volatile u16 fillZero;
    volatile u16 fillIdx;
    int objChar;
    int objPltt;
    int f;
    int n;
    int y;
    int x;

    data_0209d45c |= 8;
    *(volatile u16 *)0x400000e = (*(volatile u16 *)0x400000e & ~3) | 1;
    *(volatile u16 *)0x400000e &= ~0x40;
    *(volatile u32 *)0x400001c = 0;
    *(volatile u16 *)0x400000e = (*(volatile u16 *)0x400000e & 0x43) | 0x1210;

    f = LoadFile(0x5f);
    DecompressLZ16((void *)f, (void *)func_02054d88());
    Deallocate((void *)f);

    f = LoadFile(0x60);
    _ZN2GX10LoadBGPlttEPKvjj((const void *)f, 0x60, 0x1a0);
    Deallocate((void *)f);

    f = LoadFile(0x61);
    func_02056314((void *)f, 0, 0x800);
    Deallocate((void *)f);

    *(volatile u16 *)0x400000c = (*(volatile u16 *)0x400000c & ~3) | 1;
    *(volatile u16 *)0x400000c &= ~0x40;
    *(volatile u16 *)0x400000c = (*(volatile u16 *)0x400000c & 0x43) | 0x5418;

    f = LoadFile(0x46);
    DecompressLZ16((void *)f, G2::GetBG2CharPtr());
    Deallocate((void *)f);

    b = (char *)G2::GetBG2ScrPtr();
    fillScr = 0x3073;
    MultiStore16(fillScr, b, 0x1000);

    f = LoadFile(6);
    func_020563d4((const void *)f, 0, 0x800);
    Deallocate((void *)f);

    objChar = LoadFile(0xe0);
    objPltt = LoadFile(0xe1);
    DecompressLZ16((void *)objChar, (void *)0x6400000);
    _ZN2GX11LoadOBJPlttEPKvjj((const void *)objPltt, 0, 0x100);

    data_0209d454 |= 0xd;
    *(volatile u16 *)0x400100c &= ~3;
    *(volatile u16 *)0x400100c &= ~0x40;
    *(volatile u32 *)0x4001018 = 0;
    *(volatile u16 *)0x400100c = (*(volatile u16 *)0x400100c & 0x43) | 0x418;

    func_ov004_020af2f8(c, 0, 2, 0);

    b = (char *)G2S::GetBG2CharPtr();
    fillZero = 0;
    MultiStore16(fillZero, b, 0x6000);

    n = 0;
    for (y = 0; y < 0x18; y++) {
        for (x = 0; x < 0x20; x++) {
            dst = (char *)((u16 *)G2S::GetBG2ScrPtr() + x + y * 0x20);
            fillIdx = n;
            MultiStore16(fillIdx, dst, 2);
            n++;
        }
    }

    *(volatile u16 *)0x400100e = (*(volatile u16 *)0x400100e & ~3) | 2;
    *(volatile u16 *)0x400100e &= ~0x40;
    *(volatile u32 *)0x400101c = 0;
    *(volatile u16 *)0x400100e = (*(volatile u16 *)0x400100e & 0x43) | 0x208;

    f = LoadFile(0x5c);
    DecompressLZ16((void *)f, (void *)G2S::GetBG3CharPtr());
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

    DecompressLZ16((void *)objChar, (void *)0x6600000);
    _ZN3GXS11LoadOBJPlttEPKvjj((const void *)objPltt, 0, 0x100);
    Deallocate((void *)objChar);
    Deallocate((void *)objPltt);

    func_ov006_020feba8(c);
    func_ov006_020fc844((u8 *)c);
    func_ov006_020fae90((u8 *)c);
    func_ov004_020b04d0(0x20);

    *(int *)(c + 0xa4) = 1;
    *(u16 *)(c + 0x5c1c) = 0x10;
    *(u16 *)(c + 0x5c24) = 0x60;
    *(int *)(c + 0x5c10) = 1;
    *(int *)(c + 0xa4) = 1;
    return 1;
}
