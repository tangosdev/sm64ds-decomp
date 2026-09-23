//cpp
/* One-player curling scene: functions 0 to 26 of the class
 * (.text 0x020e0638..0x020e20bc). Function 27 is not matched yet, so the
 * rest of the class lives in its own files. The destructor is out of line
 * and comes first, which makes this file the key function's home.
 *
 * Functions run in ROM order under `#pragma defer_codegen off`; do not
 * reorder.
 *
 * Blocked: dScMgCurling_c.h types only the stones (mStone) and a few
 * fields; the falling bits at 0x478c, the score popups at 0x473c and the
 * dragged stone at 0x4eb0 are padding there, so the helpers take the scene
 * as raw bytes. The helpers are unnamed in symbols.txt.
 */

#pragma defer_codegen off

#include "types.h"
#include "dScMgCurling_c.h"

/* `C` is the receiver of the pointer-to-member state tables. It must stay
 * incomplete: mwccarm picks the pointer-to-member layout from whether the
 * class is complete, and completing it changes all four dispatchers. */
struct C;
typedef void (C::*PMF)(int);
struct Entry { PMF pmf; };

/* The five score popups at 0x473c, as func_ov006_020e1554 reads them. The
 * other functions reach the same bytes by offset, which is what the ROM
 * shows. p0 covers the u16 countdown at +0xa and the active flag at +0xc. */
typedef struct {
    int x;
    int y;
    unsigned short points;
    unsigned char p0[3];
    unsigned char shown;
    unsigned char p1[2];
} ScorePopup;

typedef struct {
    unsigned char _pad[0x473c];
    ScorePopup popups[5];
    unsigned char _pad2[0x4ee8 - 0x473c - 5*16];
    unsigned char unk_4ee8;
} ScoreView;

/* data_020a0dea and data_020a0deb are four-byte touch records. The drag
 * handler reads them through a `u8 *` cast; a plain `u8 []` costs eighteen
 * words there. */
struct B4 { unsigned char v; unsigned char pad[3]; };

/* One of the 0x32 falling bits at 0x478c (0x24 bytes each): position,
 * velocity, a wait and a timer, and state bytes. */
#define BIT_X(b,i)       (*(int*)  ((char*)(b) + 0x478c + (i)*0x24))
#define BIT_Y(b,i)       (*(int*)  ((char*)(b) + 0x4790 + (i)*0x24))
#define BIT_VX(b,i)      (*(int*)  ((char*)(b) + 0x4794 + (i)*0x24))
#define BIT_VY(b,i)      (*(int*)  ((char*)(b) + 0x4798 + (i)*0x24))
#define BIT_WAIT(b,i)    (*(unsigned short*)((char*)(b) + 0x47a0 + (i)*0x24))
#define BIT_WAIT_S(b,i)  (*(short*)((char*)(b) + 0x47a0 + (i)*0x24))
#define BIT_TIMER(b,i)   (*(unsigned short*)((char*)(b) + 0x47a2 + (i)*0x24))
#define BIT_TIMER_S(b,i) (*(short*)((char*)(b) + 0x47a2 + (i)*0x24))
#define BIT_STATE(b,i)   (*(unsigned char*)((char*)(b) + 0x47aa + (i)*0x24))
/* Launder: forces an address through an integer so it is not shared. */
#define M(p) ((int *)(int)(p))

/* Everything this file calls or reads. Class members cannot go inside an
 * `extern "C"` block, so the C-linkage names are collected here in one. */
extern "C" {

extern int  func_ov004_020adbc0(void);
extern void func_ov004_020af948(int a, int b, int c, int d);
extern int  func_ov004_020afdd0(int a, int b, int c, int d, int e);
extern void func_ov004_020b2220(int a1, int a2, int a3, int a4, int a5, int a6, int a7);
extern void func_ov004_020b2444(int a1, int a2, int num, int a4, int a5, int sel, int idx);
extern void func_02012718(int a, int b);
extern void DrawOamSprite(int a, int b, int c, int d);
extern void RenderOamBothScreens(void* a0, int a1, int a2, int a3, int a4, void* a5);
extern int  RandomIntInternal(int *seed);
extern int  _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);

extern int  data_0209d4b8;
extern s16  data_02082214[];
extern u8   data_020a0de8[];
extern u8   data_020a0de9[];
extern struct B4 data_020a0dea[];
extern struct B4 data_020a0deb[];
extern u8   data_020a0e40[];
extern unsigned char data_ov006_0212e450[];
extern unsigned char data_ov006_0212e454[];
extern u8   data_ov006_0212e458[];
extern int  data_ov006_0212e460[];
extern int  data_ov006_0212e468[];
extern int  data_ov006_0213a5e0[];
extern int  data_ov006_0213c264;
extern int  data_ov006_0213c2ac;
extern int  data_ov006_0213c2e4[];

/* The four state tables, filled at startup by the ov006 static init. */
extern PMF   data_ov006_021418c0[];
extern PMF   data_ov006_021418d8[];
extern Entry data_ov006_021418f0[];
extern PMF   data_ov006_02141930[];

/* Defined below. */
extern void func_ov006_020e0694(char *c);
extern void func_ov006_020e071c(char *c, int i);
extern void func_ov006_020e07b0(char *o, int i);
extern void func_ov006_020e0884(char *c, int i);
extern void func_ov006_020e091c(char *base, int i);
extern void func_ov006_020e0a24(char *base, int idx);
extern void func_ov006_020e0b64(char *base, int index);
extern void func_ov006_020e0ca0(char *o, int i);
extern void func_ov006_020e0d84(char *c, int i);
extern void func_ov006_020e0e18(char *base, int idx);
extern void func_ov006_020e0edc(char *c, int idx);
extern void func_ov006_020e0ff0(void *base, int idx);
extern void func_ov006_020e1100(char *c, int idx);
extern void func_ov006_020e1214(char *base, int idx);
extern void func_ov006_020e1264(char *c, int idx);
extern void func_ov006_020e12d0(char *o);
extern void func_ov006_020e13a4(char *c);
extern void func_ov006_020e1554(ScoreView *o);
extern void func_ov006_020e1608(char *self);
extern void func_ov006_020e1680(char *o);
extern void func_ov006_020e17f8(char *self);
extern void func_ov006_020e1854(void *arg);
extern void func_ov006_020e1b54(char *c);
extern void func_ov006_020e1c68(char *a0);
extern void func_ov006_020e1dc8(dScMgCurling_c *self, int idx);

}  /* extern "C" */

namespace cstd { int sqrt(u64 value); }
namespace Sound { u32 PlayBank2_2D(u32 id); }

// @symbol _ZN14dScMgCurling_cD1Ev
// @symbol _ZN14dScMgCurling_cD0Ev
dScMgCurling_c::~dScMgCurling_c()
{
}
// @symbol func_ov006_020e0694
/* Draws the falling bits. The functions after this one are the bits'
 * states: each field is 0x24 * index past its base offset. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020e0694(char *bit)
{
    int i;
    for (i = 0; i < 0x32; i++) {
        if (*(unsigned char *)(bit + 0x47ac)) {
            int x = *(int *)(bit + 0x478c) >> 0xc;
            int y = *(int *)(bit + 0x4790) >> 0xc;
            func_ov004_020af948(data_ov006_0213a5e0[*(unsigned char *)(bit + 0x47ad)], x, y, 0);
            DrawOamSprite(data_ov006_0213a5e0[*(unsigned char *)(bit + 0x47ae)], x, y, 0);
        }
        bit += 0x24;
    }
}
}

// @symbol func_ov006_020e071c
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020e071c(char *raw, int index)
{
    int off = index * 0x24;
    if (*(unsigned short *)(raw + 0x47a4 + off) != 0) {
        short *p = (short *)(raw + 0x47a4 + off);
        *p = (short)(*(unsigned short *)p - 1);
        if (*p < 0)
            *p = 0;
    } else if (*(int *)(raw + 0x4798 + off) > 0x100) {
        int *q = (int *)(raw + 0x4798 + off);
        *q = *q - 0x10;
        if ((short)*q < 0x100)
            *q = 0x100;
    } else {
        *(unsigned char *)(raw + 0x4000 + off + 0x7ab) = 0;
    }
}
}

// @symbol func_ov006_020e07b0
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020e07b0(char *raw, int index)
{
    int off = index * 0x24;
    if (*(int *)(raw + 0x479c + off) > *(int *)(raw + 0x4798 + off)) {
        *(int *)(raw + 0x4798 + off) += 0x10;
        if (*(int *)(raw + 0x479c + off) > *(int *)(raw + 0x4798 + off))
            *(int *)(raw + 0x4798 + off) = *(int *)(raw + 0x479c + off);
    }
    if (*(unsigned short *)(raw + 0x47a4 + off) != 0) {
        *(unsigned short *)(raw + 0x47a4 + off) = *(unsigned short *)(raw + 0x47a4 + off) - 1;
        if (*(short *)(raw + 0x47a4 + off) < 0) *(short *)(raw + 0x47a4 + off) = 0;
    } else {
        *(unsigned char *)(raw + off + 0x47ab) = 2;
        *(short *)(raw + 0x47a4 + off) = (short)(unsigned char)((((0x20 * (((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff)) >> 0xf)) + 0x20);
    }
}
}

// @symbol func_ov006_020e0884
extern "C" {
void func_ov006_020e0884(char* raw, int index) {
  int off = index * 0x24;
  unsigned int roll;
  *(int*)(raw + 0x4798 + off) = 0;
  roll = ((unsigned)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
  *(int*)(raw + 0x479c + off) = (((roll << 4) >> 15) << 4) + 0x300;
  *(unsigned char*)(raw + 0x47ab + off) = 1;
  roll = ((unsigned)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
  roll = ((roll << 5) >> 15) + 0x20;
  *(short*)(raw + 0x47a4 + off) = (unsigned char)roll;
}
}

// @symbol func_ov006_020e091c
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020e091c(char *raw, int index)
{
    int off = index * 0x24;
    char *vxs = raw + 0x4794;
    char *xs = raw + 0x478c;
    char *ys = raw + 0x4790;
    *(int*)(xs + off) = *(int*)(xs + off) + *(int*)(vxs + off);
    *(int*)(ys + off) = *(int*)(ys + off) + *(int*)(raw + off + 0x4798);
    if (*(u16*)(raw + off + 0x47a0) != 0) {
        char *timers = raw + 0x47a2;
        *(u16*)(timers + off) = *(u16*)(timers + off) - 1;
        if (*(s16*)(timers + off) < 0) *(s16*)(timers + off) = 0;
        return;
    }
    if (*(int*)(vxs + off) > 0) {
        *(int*)(vxs + off) = *(int*)(vxs + off) - 8;
        if ((s16)*(int*)(vxs + off) < 0) *(int*)(vxs + off) = 0;
        return;
    }
    if (*(int*)(vxs + off) < 0) {
        *(int*)(vxs + off) = *(int*)(vxs + off) + 8;
        if (*(int*)(vxs + off) > 0) *(int*)(vxs + off) = 0;
        return;
    }
    *(u8*)(raw + off + 0x47aa) = 0;
}
}

// @symbol func_ov006_020e0a24
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020e0a24(char *raw, int index)
{
    unsigned short timer;

    BIT_X(raw, index) = BIT_X(raw, index) + BIT_VX(raw, index);
    BIT_Y(raw, index) = BIT_Y(raw, index) + BIT_VY(raw, index);

    if (BIT_WAIT(raw, index) != 0) {
        BIT_WAIT(raw, index) = BIT_WAIT(raw, index) - 1;
        if (BIT_WAIT_S(raw, index) < 0)
            BIT_WAIT(raw, index) = 0;
        return;
    }

    if (BIT_VX(raw, index) > -0x300) {
        BIT_VX(raw, index) -= 8;
        if (BIT_VX(raw, index) <= -0x300)
            BIT_VX(raw, index) = 0x300;
    }

    timer = BIT_TIMER(raw, index);
    if (timer != 0) {
        BIT_TIMER(raw, index) = timer - 1;
        if (BIT_TIMER_S(raw, index) < 0)
            BIT_TIMER(raw, index) = 0;
        return;
    }

    BIT_STATE(raw, index) = 3;
    BIT_TIMER(raw, index) = (unsigned char)(((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 5 >> 0xf) + 0x20);
}
}

// @symbol func_ov006_020e0b64
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020e0b64(char *raw, int index)
{
    int off = index * 0x24;
    unsigned short timer;

    *(int *)(raw + 0x478c + off) += *(int *)(raw + 0x4794 + off);
    *(int *)(raw + 0x4790 + off) += *(int *)(raw + 0x4798 + off);

    timer = *(unsigned short *)(raw + 0x47a0 + off);
    if (timer != 0) {
        *(short *)(raw + 0x47a0 + off) = timer - 1;
        if (*(short *)(raw + 0x47a0 + off) < 0)
            *(short *)(raw + 0x47a0 + off) = 0;
        return;
    }

    if (*(int *)(raw + 0x4794 + off) < 0x300) {
        *(int *)(raw + 0x4794 + off) += 8;
        if (*(int *)(raw + 0x4794 + off) >= 0x300)
            *(int *)(raw + 0x4794 + off) = 0x300;
    }

    timer = *(unsigned short *)(raw + 0x47a2 + off);
    if (timer != 0) {
        *(short *)(raw + 0x47a2 + off) = timer - 1;
        if (*(short *)(raw + 0x47a2 + off) < 0)
            *(short *)(raw + 0x47a2 + off) = 0;
        return;
    }

    *(char *)(raw + 0x47aa + off) = 3;
    *(short *)(raw + 0x47a2 + off) = (((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 5) >> 0xf) + 0x20 & 0xff;
}
}

// @symbol func_ov006_020e0ca0
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020e0ca0(char *raw, int index)
{
    int off = index * 0x24;
    if (*(unsigned short *)(raw + 0x47a0 + off) != 0) {
        *(unsigned short *)(raw + 0x47a0 + off) = *(unsigned short *)(raw + 0x47a0 + off) - 1;
        if (*(short *)(raw + 0x47a0 + off) < 0) *(short *)(raw + 0x47a0 + off) = 0;
        return;
    }
    *(int *)(raw + 0x4794 + off) = 0;
    *(unsigned char *)(raw + 0x47aa + off) = data_ov006_0212e450[(((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 1 >> 15];
    *(unsigned short *)(raw + 0x47a0 + off) = (short)(unsigned char)((0x10 * (((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff)) >> 0xf);
    *(unsigned short *)(raw + 0x47a2 + off) = (short)(unsigned char)(((0x40 * (((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff)) >> 0xf) + 0x60);
}
}

// @symbol func_ov006_020e0d84
extern "C" void func_ov006_020e0d84(char *raw, int index)
{
    C *self = (C *)raw;
    int off = index * 0x24;
    unsigned char state = *(unsigned char *)(raw + off + 0x47aa);
    (self->*data_ov006_02141930[state])(index);
    unsigned char state2 = *(unsigned char *)((char *)self + off + 0x47ab);
    (self->*data_ov006_021418d8[state2])(index);
}

// @symbol func_ov006_020e0e18
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020e0e18(char *raw, int index) {
    int off = index * 0x24;
    int *x = (int *)(raw + 0x478c + off);
    int *vx = (int *)(raw + 0x4794 + off);
    int *y = (int *)(raw + 0x4790 + off);
    *x += *vx;
    *y += *(int *)(raw + off + 0x4798);
    if (*vx > 0) {
        *vx -= 0x20;
        if ((int)(short)*vx < 0) *vx = 0;
    } else if (*vx < 0) {
        *vx += 0x20;
        if (*vx > 0) *vx = 0;
    } else {
        *(unsigned char *)(raw + off + 0x47aa) = 0;
    }
}
}

// @symbol func_ov006_020e0edc
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020e0edc(char *raw, int index)
{
    char *bits = raw;
    int off = index * 0x24;

    *(int *)(bits + 0x478c + off) += *(int *)(bits + 0x4794 + off);
    *(int *)(bits + 0x4790 + off) +=
        *(int *)((char *)bits + off + 0x4798);

    u16 *wait = (u16 *)(bits + 0x47a0 + off);
    if (*wait != 0) {
        *wait = *wait - 1;
        s16 left = *(s16 *)wait;
        if (left < 0) {
            *wait = 0;
        }
        return;
    }

    int *vx = (int *)(bits + 0x4794 + off);
    if (*vx > -0x400) {
        *vx -= 0x20;
        if (*vx <= -0x400) {
            *vx = 0x400;
        }
    }

    u16 *timer = (u16 *)(bits + 0x47a2 + off);
    if (*timer != 0) {
        *timer = *timer - 1;
        s16 left = *(s16 *)timer;
        if (left < 0) {
            *timer = 0;
        }
        return;
    }

    *(unsigned char *)(bits + off + 0x47aa) = 3;
}
}

#pragma push
#pragma inline_depth(0)
// @symbol func_ov006_020e0ff0
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020e0ff0(void* raw, int index) {
    char* bits = (char*)raw;
    int off = index * 0x24;

    *(int*)(bits + 0x478c + off) += *(int*)(bits + 0x4794 + off);
    *(int*)(bits + 0x4790 + off) += *(int*)((char*)raw + off + 0x4798);

    unsigned short* wait = (unsigned short*)(bits + 0x47a0 + off);
    if (*wait != 0) {
        *wait = *wait - 1;
        short left = *(short*)wait;
        if (left < 0) {
            *wait = 0;
        }
        return;
    }

    int* vx = (int*)(bits + 0x4794 + off);
    if (*vx < 0x400) {
        *vx = *vx + 0x20;
        if (*vx >= 0x400) {
            *vx = 0x400;
        }
    }

    unsigned short* timer = (unsigned short*)(bits + 0x47a2 + off);
    if (*timer != 0) {
        *timer = *timer - 1;
        short left = *(short*)timer;
        if (left < 0) {
            *timer = 0;
        }
        return;
    }

    *(char*)(bits + off + 0x47aa) = 3;
}
#pragma pop
}

// @symbol func_ov006_020e1100
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020e1100(char *raw, int idx)
{
    int off = idx * 0x24;
    unsigned short wait;
    short left;
    unsigned int roll;
    wait = *(unsigned short *)(raw + 0x47a0 + off);
    if (wait != 0) {
        left = (short)(wait - 1);
        *(short *)(raw + 0x47a0 + off) = left;
        if (*(short *)(raw + 0x47a0 + off) < 0) *(short *)(raw + 0x47a0 + off) = 0;
        return;
    }
    *(int *)(raw + 0x4794 + off) = 0;
    roll = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
    *(int *)(raw + 0x4798 + off) = (int)(((roll << 5) >> 15) << 4) + 0x600;
    roll = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
    *(unsigned char *)(raw + 0x47aa + off) = data_ov006_0212e454[(roll << 1) >> 15];
    roll = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
    *(short *)(raw + 0x47a0 + off) = (unsigned char)((roll << 4) >> 15);
    roll = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
    *(short *)(raw + 0x47a2 + off) = (unsigned char)(((roll * 0x30) >> 15) + 0x30);
}
}

// @symbol func_ov006_020e1214
extern "C" void func_ov006_020e1214(char *raw, int idx)
{
    unsigned char state = BIT_STATE(raw, idx);
    (((C*)raw)->*data_ov006_021418f0[state].pmf)(idx);
}

#pragma push
#pragma opt_propagation off
// @symbol func_ov006_020e1264
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020e1264(char *raw, int idx) {
    unsigned roll = (unsigned)RandomIntInternal(&data_0209d4b8);
    int k = 0;
    unsigned pick = ((roll >> 16) & 0x7fff) << 3 >> 0xf;
    if (pick == 5) k = 1;
    char *bit = raw + idx * 0x24;
    *(unsigned char *)(bit + 0x47a9) = data_ov006_0212e460[k];
    *(unsigned char *)(bit + 0x47aa) = 0;
}
}
#pragma pop

// @symbol func_ov006_020e12d0
extern "C" void func_ov006_020e12d0(char *raw)
{
    int i;
    char *bit = raw;
    for (i = 0; i < 0x32; i++) {
        if (*(unsigned char *)(bit + 0x47a8) != 0) {
            unsigned char kind = *(unsigned char *)(bit + 0x47a9);
            (((C *)raw)->*data_ov006_021418c0[kind])(i);
            if ((*(int *)(bit + 0x4790) >> 0xc) >= 0xc8) {
                *(int *)(bit + 0x478c) = (((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 5 >> 0xf << 0xf;
                *(int *)(bit + 0x4790) = -0x8000;
                *(unsigned char *)(bit + 0x47aa) = 0;
                *(unsigned char *)(bit + 0x47a9) = 0;
                *(unsigned char *)(bit + 0x47ab) = 0;
            }
        }
        bit += 0x24;
    }
}

// @symbol func_ov006_020e13a4
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020e13a4(char *raw)
{
    int i;
    char *bit;
    unsigned int roll;
    unsigned int v;
    int q;
    unsigned int m;

    i = 0;
    bit = raw;
    for (; i < 0x32; i++)
    {
        *(int *)(bit + 0x478c) = 0;
        *(int *)(bit + 0x4790) = 0;
        *(int *)(bit + 0x4794) = 0;
        *(int *)(bit + 0x4798) = 0;
        *(short *)(bit + 0x47a0) = 0;
        *(short *)(bit + 0x47a2) = 0;
        *(short *)(bit + 0x47a4) = 0;
        *(char *)(bit + 0x47a8) = 0;
        *(char *)(bit + 0x47a9) = 0;
        *(char *)(bit + 0x47aa) = 0;
        *(char *)(bit + 0x47ab) = 0;
        *(char *)(bit + 0x47ac) = 0;
        *(char *)(bit + 0x47ad) = 0;
        *(char *)(bit + 0x47ae) = 1;
        bit += 0x24;
    }

    i = 0;
    bit = raw;
    for (; i < 0x32; i++)
    {
        roll = (unsigned int)RandomIntInternal(&data_0209d4b8);
        m = ((roll >> 16) & 0x7fff) << 5;
        *(int *)(bit + 0x478c) = (int)((m >> 0xf)) << 0xf;
        *(int *)(bit + 0x4790) = -0x8000;
        *(char *)(bit + 0x47a8) = 1;
        *(char *)(bit + 0x47ac) = 1;
        *(char *)(bit + 0x47a9) = 0;
        *(char *)(bit + 0x47aa) = 0;

        roll = (unsigned int)RandomIntInternal(&data_0209d4b8);
        *(char *)(bit + 0x47ad) = (char)(((roll >> 16) & 0x7fff) * 5 >> 0xf);

        roll = (unsigned int)RandomIntInternal(&data_0209d4b8);
        v = *(unsigned char *)(bit + 0x47ad) + ((((roll >> 16) & 0x7fff) << 2) >> 0xf) + 1;
        v = v & 0xff;
        if (v >= 5)
            v = (v - 5) & 0xff;
        *(char *)(bit + 0x47ae) = (char)v;

        roll = (unsigned int)RandomIntInternal(&data_0209d4b8);
        *(short *)(bit + 0x47a0) = (short)(((i & 7) << 6) + (((roll >> 16) & 0x7fff) * 0x30 >> 0xf));

        roll = (unsigned int)RandomIntInternal(&data_0209d4b8);
        m = ((roll >> 16) & 0x7fff) << 5;
        *(int *)(bit + 0x478c) = (int)((m >> 0xf)) << 0xf;

        roll = (unsigned int)RandomIntInternal(&data_0209d4b8);
        q = (((roll >> 16) & 0x7fff) * 0x1a) >> 0xf;
        *(int *)(bit + 0x4790) = (((q << 3) - 8)) << 0xc;
        *(short *)(bit + 0x47a0) = 0;
        bit += 0x24;
    }
}
}

// @symbol func_ov006_020e1554
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020e1554(ScoreView *view)
{
    int i;
    for (i = 0; i < 5; i++) {
        if (view->popups[i].shown != 0) {
            func_ov004_020b2444(view->popups[i].x >> 12, view->popups[i].y >> 12, view->popups[i].points, -1, -1, 0, 0);
        }
    }
    if (view->unk_4ee8 != 0) {
        int r = func_ov004_020adbc0();
        func_ov004_020b2220(0x80, 0x60, r, 1, 0, 0x800, 0);
    }
}
}

#pragma push
// @symbol func_ov006_020e1608
extern "C" {  /* .c-derived member: C linkage for the whole block */
#pragma opt_strength_reduction off
void func_ov006_020e1608(char *raw) {
    int i;
    for (i = 0; i < 5; i++) {
        char *popup = raw + (i << 4);
        if (*(unsigned char*)(popup + 0x4748) == 0) continue;
        if (*(unsigned short*)(popup + 0x4746) == 0) continue;
        {
            volatile unsigned short *timer = (volatile unsigned short*)((unsigned int)popup + 0x4746);
            *timer = *timer - 1;
            if (*timer != 0) continue;
        }
        *(unsigned char*)(popup + 0x4749) = 1;
        Sound::PlayBank2_2D(0x1bc);
    }
}
}
#pragma pop

#pragma push
#pragma opt_strength_reduction off
// @symbol func_ov006_020e1680
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020e1680(char *raw)
{
    int slot = 0;
    int i, j;
    char *stone;
    u8 *delay;

    for (j = 0; j < 5; j++) {
        if (*(u8 *)(raw + j * 16 + 0x4748) == 0) {
            slot = j;
            break;
        }
    }

    delay = &data_ov006_0212e458[slot];
    stone = raw;
    for (i = 0; i < 5; i++, stone += 0x2c) {
        int dx, dz, dist, top;

        if (*(u8 *)(stone + 0x4689) == 0)
            continue;
        dx = *(int *)(stone + 0x4660) - *(int *)(raw + 0x4e94);
        dz = *(int *)(stone + 0x4664) - *(int *)(raw + 0x4e98);
        {
            int ax = dx >> 12;
            int az = dz >> 12;
            dist = cstd::sqrt((u64)(s64)(ax * ax + az * az));
        }
        *(u8 *)(raw + slot * 16 + 0x4748) = 1;
        *(int *)(raw + slot * 16 + 0x473c) = *(int *)(stone + 0x4660);
        {
        int *popupY = M(raw + slot * 16 + 0x4740);
        *popupY = *(int *)(stone + 0x4664) + 0x1000;
        *(short *)(raw + slot * 16 + 0x4746) = *delay;
        if (dist <= 8)
            *(short *)(raw + slot * 16 + 0x4744) = 1000;
        else if (dist <= 0x18)
            *(short *)(raw + slot * 16 + 0x4744) = 500;
        else if (dist <= 0x28)
            *(short *)(raw + slot * 16 + 0x4744) = 300;
        else if (dist <= 0x38)
            *(short *)(raw + slot * 16 + 0x4744) = 100;
        else
            *(short *)(raw + slot * 16 + 0x4744) = 0;
        top = *popupY >> 12;
        if (top >= -32 && top <= 8)
            *popupY = -0x28000;
        }
        delay++;
        slot++;
    }
}
}
#pragma pop

// @symbol func_ov006_020e17f8
/* Draws a sprite at the aimed stone's position (0x4eb0) once the stylus
 * handler has set 0x4ee5. */
extern "C" void func_ov006_020e17f8(char *raw)
{
  if(*(unsigned char*)(raw+0x4ee5)==0) return;
  int x=*(int*)(raw+0x4eb0);
  int y=*(int*)(raw+0x4eb4);
  func_ov004_020afdd0((int)data_ov006_0213c2e4,(x>>12)-0x20,(y>>12)-8,-1,0);
}

#pragma push
#pragma opt_common_subs off
// @symbol func_ov006_020e1854
/* Stylus handler for the stone being aimed. While the stylus is down, the
 * touch point plus the grab offset becomes the new position, clamped to the
 * 0x20000..0xe0000 by 0x94000..0xb8000 box; a move under two units is undone.
 * Otherwise it updates the swing tracking (0x4eea), turns the move into an
 * angle at 0x4ede clamped to the lower half turn and averaged with the last
 * one, and folds the move length into the speed at 0x4ec8. With the stylus
 * up, two flags are reset.
 *
 * Shapes that must stay: the touch records are read through a `u8 *` cast;
 * dx is computed before dy with no temporaries; the tail's index `j` is
 * wider than a byte; and the pragma keeps the 0x4eb0 and 0x4eb4 re-reads
 * after the clamps. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020e1854(void *arg)
{
    u8 *raw = (u8 *)arg;
    u8 idx;
    int dx, oldx, oldy;
    int diff;
    u16 ang;

    idx = data_020a0e40[0];
    if (data_020a0de8[idx * 4] != 0) {
        int dy2, dy;

        int i4 = idx * 4;
        u8 *pa = (u8 *)data_020a0dea;
        u8 *pb = (u8 *)data_020a0deb;
        u8 bx = pa[i4];
        u8 by = pb[i4];

        oldx = *(int *)(raw + 0x4eb0);
        oldy = *(int *)(raw + 0x4eb4);
        *(int *)(raw + 0x4eb0) = (bx << 12) + *(int *)(raw + 0x4ec0);
        *(int *)(raw + 0x4eb4) = *(int *)(raw + 0x4ec4) + (by << 12);

        if (*(int *)(raw + 0x4eb4) <= 0x94000)
            *(int *)(raw + 0x4eb4) = 0x94000;
        if (*(int *)(raw + 0x4eb0) <= 0x20000)
            *(int *)(raw + 0x4eb0) = 0x20000;
        if (*(int *)(raw + 0x4eb0) >= 0xe0000)
            *(int *)(raw + 0x4eb0) = 0xe0000;
        if (*(int *)(raw + 0x4eb4) >= 0xb8000)
            *(int *)(raw + 0x4eb4) = 0xb8000;


        dx = (*(int *)(raw + 0x4eb0) - oldx) >> 12;
        dy = (*(int *)(raw + 0x4eb4) - oldy) >> 12;
        dy2 = dy * dy;

        if (cstd::sqrt((s64)(dx * dx + dy2)) <= 1) {
            *(int *)(raw + 0x4eb0) = oldx;
            *(int *)(raw + 0x4eb4) = oldy;
            return;
        }

        diff = (*(int *)(raw + 0x4eb4) - *(int *)(raw + 0x4ebc)) >> 12;
        if (*(u8 *)(raw + 0x4eea) == 0) {
            func_02012718(0x1d6, *(int *)(raw + 0x4eb0));
            *(u8 *)(raw + 0x4eea) = 2;
            *(int *)(raw + 0x4ed4) = (*(int *)(raw + 0x4eb4) - *(int *)(raw + 0x4ebc)) >> 12;
            *(int *)(raw + 0x4ebc) = *(int *)(raw + 0x4eb4);
        } else if (*(u8 *)(raw + 0x4eea) == 1) {
            if (*(int *)(raw + 0x4ed4) * diff > 0) {
                if (diff < 0)
                    diff = -diff;
                if (diff >= 0xa)
                    *(u8 *)(raw + 0x4eea) = 0;
            } else {
                *(int *)(raw + 0x4ed4) = diff;
                *(int *)(raw + 0x4ebc) = *(int *)(raw + 0x4eb4);
            }
        } else {
            if (*(int *)(raw + 0x4ed4) * diff < 0)
                *(u8 *)(raw + 0x4eea) = 1;
            *(int *)(raw + 0x4ed4) = (*(int *)(raw + 0x4eb4) - *(int *)(raw + 0x4ebc)) >> 12;
            *(int *)(raw + 0x4ebc) = *(int *)(raw + 0x4eb4);
        }

        ang = *(u16 *)(raw + 0x4ede);
        *(u16 *)(raw + 0x4ede) = _ZN4cstd5atan2E5Fix12IiES1_(dy, dx >> 1);
        {
            u16 a = *(u16 *)(raw + 0x4ede);
            if (a <= 0x8000) {
                if (a >= 0x4000) {
                    *(u16 *)(raw + 0x4ede) = 0x8000;
                    goto ang_done;
                }
            }
            if (a <= 0x4000)
                *(u16 *)(raw + 0x4ede) = 0;
        }
    ang_done:;

        {
            int mag;
            *(u16 *)(raw + 0x4ede) = (u16)((*(u16 *)(raw + 0x4ede) + ang) >> 1);
            mag = cstd::sqrt((s64)((dx >> 1) * (dx >> 1) + dy2)) * 9;
            mag = (mag << 12) >> 4;
            if (mag >= 0xc000)
                mag = 0xc000;
            if (mag > *(int *)(raw + 0x4ec8))
                *(int *)(raw + 0x4ec8) = mag;
            {
                int cur = *(int *)(raw + 0x4ec8);
                if (cur > mag) {
                    *(int *)(raw + 0x4ec8) = *(int *)(raw + 0x4ec8) - ((cur - mag) >> 1);
                }
            }
        }


        {
            int px = *(int *)(raw + 0x4eb0);
            int py = *(int *)(raw + 0x4eb4);
            int j = data_020a0e40[0];
            u8 jx = ((u8 *)data_020a0dea)[j * 4];
            int ax = (px >> 12) - jx;
            u8 jy = ((u8 *)data_020a0deb)[j * 4];
            int ay = (py >> 12) - jy;
            *(int *)(raw + 0x4ec0) = ax << 12;
            *(int *)(raw + 0x4ec4) = ay << 12;
        }

        return;
    }

    *(u8 *)(raw + 0x4ee4) = 0;
    *(u8 *)(raw + 0x4ee5) = 1;
}
}
#pragma pop

// @symbol func_ov006_020e1b54
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020e1b54(char *raw)
{
  int idx;
  int touching = 0;
  int x;
  int y;
  idx = data_020a0e40[0];
  if (data_020a0de8[idx * (4 & 0xFFFFFFFF)] != 0)
  {
    if (data_020a0de9[idx * 4] != 0)
    {
      touching = 1;
    }
  }
  if (touching == 0)
  {
    return;
  }
  x = ((*((int *) (raw + 0x4eb0))) >> 0xc) - ((u8 *)data_020a0dea)[idx * 4];
  y = ((*((int *) (raw + 0x4eb4))) >> 0xc) - ((u8 *)data_020a0deb)[idx * 4];
  *((int *) (raw + 0x4ec0)) = x << 0xc;
  *((int *) (raw + 0x4ec4)) = y << 0xc;
  *((u8 *) (raw + 0x4ee4)) = 1;
  *((u16 *) (raw + 0x4ede)) = 0xc000;
  if ((*((u8 *) (raw + 0x4ee9))) == 0)
  {
    func_02012718(0x1d2, *((int *) (raw + 0x4eb0)));
    *((u8 *) (raw + 0x4ee9)) = 6;
  }
  *((int *) (raw + 0x4ecc)) = 0;
  *((int *) (raw + 0x4ed0)) = 0;
  *((int *) (raw + 0x4eb8)) = (*((int *) (raw + 0x4eb0))) + (*((int *) (raw + 0x4ec0)));
  *((int *) (raw + 0x4ebc)) = (*((int *) (raw + 0x4eb4))) + (*((int *) (raw + 0x4ec4)));
  *((int *) (raw + 0x4ed4)) = 0xff;
  *((u8 *) (raw + 0x4eea)) = 0;
}
}

// @symbol func_ov006_020e1c68
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020e1c68(char* raw) {
    int x, y;
    int i;
    char* stone;
    int remaining;
    int j;
    stone = raw;
    for (i = 0; i < 5; i++) {
        if (*(unsigned char*)(stone + 0x4689) == 0) goto next;
        if (*(unsigned char*)(stone + 0x468a) == 0) goto next;
        x = *(int*)(stone + 0x4660) >> 12;
        y = *(int*)(stone + 0x4664) >> 12;
        RenderOamBothScreens(&data_ov006_0213c264, x, y, -1, 1, 0);
        RenderOamBothScreens(&data_ov006_0213c2ac, x, y + 8, -1, 2, 0);
    next:
        stone += 0x2c;
    }
    remaining = 5 - *(unsigned char*)(raw + 0x4ee6);
    if (remaining < 0) remaining = 0;
    j = 0;
    if (remaining > 0) {
        for (; j < remaining; j++) {
            int slotX = data_ov006_0212e468[j];
            RenderOamBothScreens(&data_ov006_0213c264, slotX, 0xb0, -1, 1, 0);
            RenderOamBothScreens(&data_ov006_0213c2ac, slotX, 0xb8, -1, 2, 0);
        }
    }
}
}

// @symbol func_ov006_020e1dc8
/* Stone separation. Stone idx has just moved: the first other active stone
 * within 24 units is pushed out to 26 units along the line between them,
 * then that stone gets the same check once against the rest (with a bump
 * sound). Only one push each way per call.
 *
 * The rotated offsets must be named (vx, vy), and the inner scan needs its
 * own dx, dy, dist and ang; sharing the outer ones changes the registers. */
extern "C" void func_ov006_020e1dc8(dScMgCurling_c *self, int idx)
{
    int i;
    int j;
    int dx;
    int dy;
    int dist;
    u16 ang;
    int k;

    for (i = 0; i < 5; i++) {
        if (self->mStone[i].active == 0) continue;
        if (idx == i) continue;
        dx = (self->mStone[i].x - self->mStone[idx].x) >> 12;
        dy = (self->mStone[i].y - self->mStone[idx].y) >> 12;
        dist = cstd::sqrt((u64)(dx * dx + dy * dy));
        ang = _ZN4cstd5atan2E5Fix12IiES1_(dy, dx);
        if (dist > 0x18) continue;
        {
            int cs;
            int sn;
            int vx;
            int vy;

            k = (ang >> 4) * 2;
            cs = data_02082214[k + 1];
            vx = (int)(((long long)cs * 0x1a + 0x800) >> 12);
            self->mStone[i].x = self->mStone[idx].x + (vx << 12);
            sn = data_02082214[k];
            vy = (int)(((long long)sn * 0x1a + 0x800) >> 12);
            self->mStone[i].y = self->mStone[idx].y + (vy << 12);
            for (j = 0; j < 5; j++) {
                int dx2;
                int dy2;
                int dist2;
                u16 ang2;

                if (self->mStone[j].active == 0) continue;
                if (i == j) continue;
                dx2 = (self->mStone[j].x - self->mStone[i].x) >> 12;
                dy2 = (self->mStone[j].y - self->mStone[i].y) >> 12;
                dist2 = cstd::sqrt((u64)(dx2 * dx2 + dy2 * dy2));
                ang2 = _ZN4cstd5atan2E5Fix12IiES1_(dy2, dx2);
                if (dist2 > 0x18) continue;
                {
                    int cs2;
                    int sn2;
                    int vx2;
                    int vy2;

                    k = (ang2 >> 4) * 2;
                    cs2 = data_02082214[k + 1];
                    vx2 = (int)(((long long)cs2 * 0x1a + 0x800) >> 12);
                    self->mStone[j].x = self->mStone[i].x + (vx2 << 12);
                    sn2 = data_02082214[k];
                    vy2 = (int)(((long long)sn2 * 0x1a + 0x800) >> 12);
                    self->mStone[j].y = self->mStone[i].y + (vy2 << 12);
                    func_02012718(0xe8, self->mStone[idx].x);
                    return;
                }
            }
            return;
        }
    }
}

