//cpp
/* Bob-omb Squad. The player aims a Bob-omb with the stylus and fires it
 * up the board; a shot that reaches a target knocks the balls loose.
 * 45 functions (.text 0x020fa75c..0x020fc8c0).
 *
 * This is the lowest and largest stretch of the class's run: three unmatched
 * functions (0x020fc8c0, 0x020fcb4c and 0x020fdaf0) split the rest into
 * pieces that keep their own files. The destructor is declared first in the
 * header, so this TU owns the key function and emits the vtable and RTTI;
 * the ROM's own copies live elsewhere and these are discarded at link time.
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

/* func_ov006_020fc7d0's {code pointer, adjustment} records. */
struct Ent7d0 { int a; int b; };

/* func_ov006_020fb4e0's stride-only view of the same 0x1c-byte rows. */
struct E { char pad[0x1c]; };

/* Launder macros: they make the compiler materialise a base address on its
   own instead of folding it into a neighbouring add, as the ROM does. */
#define AT(p, off) ((void*)(int)((char*)(p) + (off)))
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

namespace G2S {
    unsigned GetBG2CharPtr();
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
extern s8   func_ov004_020adc1c(s8 levelID);
extern void func_ov004_020b19f0(s8 courseID);
extern void func_ov004_020adb1c(int self);
extern int  func_020126e8(int a);
extern int  func_02012468(int a, int b, int c, int d, int e, int f, int g, short h);
extern void SetBg2Offset(int a, int b);
extern void MultiStore16(unsigned short val, char *dst, int nbytes);
extern int  RandomIntInternal(int *seed);
extern int  _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(int, void*, int, int, int, int, int, void*, int, int);

/* --- ROM data this TU reads --- */
extern int   data_0209d4b8;
extern unsigned char data_0209d45c;
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

/* The two pointer-to-member tables the helpers dispatch through, and the
   record table func_ov006_020fc7d0 walks. Static initialisers fill them
   from {code pointer, adjustment} records in ov006 .data. */
extern Entry  data_ov006_02142604[];
extern Ent7d0 data_ov006_02142624[];
extern PMF2   data_ov006_0214266c[];

/* A helper of this class that lives outside this TU. */
extern void func_ov006_020fe1a8(char *p);

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
extern void func_ov006_020fba28(void);
extern void func_ov006_020fba48(s8 levelID);
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
void func_ov006_020fba28(void){
  func_ov004_020b1a5c(func_ov004_020adbc0(), 6);
}

// @symbol func_ov006_020fba48
/* Converts a sublevel ID to a course ID, then opens the cannon for it. */
void func_ov006_020fba48(s8 levelID)
{
    s8 courseID = func_ov004_020adc1c(levelID);
    func_ov004_020b19f0(courseID);
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

}  /* extern "C" */
