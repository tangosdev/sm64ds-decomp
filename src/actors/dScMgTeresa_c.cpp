//cpp
/* Hide and Boo Seek. The player watches the Boo cross the room, the lights
 * go out, and they rub the Touch Screen to uncover every hiding Boo. The
 * first 15 rounds give three seconds; after that the clock is two. This
 * file is all 81 functions of dScMgTeresa_c's ov006 unit (.text
 * 0x0211cbd0..0x021207a8): the big Boo's flight, the countdown and the
 * reveal sprites, the 16 hiding Boos (spawn, flight, wall bounce, fade and
 * the touch test against the rubbed-out BG0 tiles), the scene states, and
 * the class's own overrides. Below it is dScMgSound_c; above it is
 * dScMgTeresa_c_classInit in src/d_s_mg_teresa.cpp, which stays its own
 * file.
 *
 * Nothing in the ROM names the helpers, so the members keep their
 * func_ov006_ names. InitResources, Behavior, Render and OnYoshiTryEat were
 * named from their vtable slots; Virtual50 and Virtual88 are placeholders
 * (see dScMgBase_c.h).
 *
 * Functions run in ROM order, lowest address first, because of
 * `#pragma defer_codegen off` below. Do not reorder, and do not drop that
 * pragma. It makes the out-of-line destructor come out D1, D0 as in the
 * ROM (the unused D2 trails it and is deadstripped), and it is what lets
 * the seven push and pop brackets bind. Each bracket was measured: without
 * it, its function stops matching. The destructor is out of line, so it is
 * the key function and this file emits the vtable and RTTI for the whole
 * base chain.
 *
 * The four PMF state tables dispatch on this class: the big Boo's flight
 * at data_ov006_02142f18 (9 states by Boo::state), the reveal pair at
 * data_ov006_02142e88 (5 states by SlotElem::state), the 16-Boo rows at
 * data_ov006_02142ed8 (8 states), and the scene table data_ov006_02142eb0
 * (5 no-argument states by unk_4be8). __sinit_ov006_02132f68 fills them
 * from the .data descriptor records at data_ov006_0213f8xx.
 *
 * OAM::Render takes Fix12<int> by value, so that call stays mangled.
 *
 * comment leftovers:
 *  - dScMgTeresa_c.h leaves 0x4660..0x4be8 as padding, so the 16
 *    0x24-byte Boo rows at 0x4660, the 0x20-stride pair at 0x4bac, the HUD
 *    sprites at 0x4960, and the bytes at 0x4c1b and 0x4c1f are still reached
 *    through the local views below or raw offsets on `this`. Naming them in
 *    the header is what would unblock typed access -- but each function's
 *    base choice (fresh cast per access vs one saved pointer) is what
 *    mwccarm's addressing actually follows, so the conversion has to be
 *    measured one function at a time.
 */

#pragma defer_codegen off

#include "types.h"
#include "OamAttr.h"
#include "dScMgTeresa_c.h"
#include "decl_common.h"
#include "G2x.h"
#include "PlayerInput.h"

/* Local views of object ranges the header does not type yet. The layouts
 * disagree offset for offset (the second 0x20-stride element at 0x4bcc
 * overlaps the big Boo record), so they are kept apart. */

/* One Boo, 0x1c bytes, at 0x4bcc. The flight functions are called with an
 * index and step one record. x/y are the sprite's screen position (20.12).
 * yVel is added into y and then pulled down until y sits at 192. frameTimer
 * counts up and the sprite frame steps when it trips. shown is the draw
 * flag: the render returns immediately when it is clear. moveRight selects
 * the +x flight and the flipped sprite row. done sends the Boo out instead
 * of turning it around. */
struct Boo {
    int x;            /* 0x00 */
    int y;            /* 0x04 */
    int xVel;         /* 0x08 */
    int yVel;         /* 0x0c */
    u16 frameTimer;   /* 0x10 */
    u16 wait;         /* 0x12 */
    u8 active;        /* 0x14 state machine runs only while set */
    u8 state;         /* 0x15 which flight function runs */
    u8 shown;         /* 0x16 */
    u8 frame;         /* 0x17 sprite; moveRight adds 4 */
    u8 moveRight;     /* 0x18 */
    u8 done;          /* 0x19 */
    u8 pad1a[2];
};
struct BooWalk {
    char pad[0x4bcc];
    struct Boo boo[1];
};
typedef char Boo_size_must_be_0x1c[sizeof(struct Boo) == 0x1c ? 1 : -1];
/* Each access is a fresh cast. A saved Boo* makes mwcc address every field
 * from 0x4bcc, and that is not the code in the ROM. */
#define BOO(p) ((struct Boo *)((p) + 0x4bcc))
/* func_ov006_0211d018's wait and func_ov006_0211d4e8's y/yVel are a raw
 * base plus the field. Taking `&BOO(p)->member` shares one base and does
 * not match. */
#define BOO_FIELD(p, member) ((p) + (0x4bcc + (int)&((struct Boo *)0)->member))


/* func_ov006_0211fd44's view of the ov004 score block at data_ov004_020beb68
   (dScMgBase_c.h declares it void *): the current score at +0xb4 and the best
   at +0xb8. */
struct ScoreView { unsigned char pad[0xb4]; int b4; int b8; };

/* func_ov006_0211e0c8's `struct C`: a 0x24-stride row walk, no fields named. */
struct Row { char pad[0x24]; };
struct RowArray { struct Row rows[1]; };

/* The 0x20-stride element pair at +0x4bac; element 0 also drives the
   data_ov006_02142e88 PMF table via its `state`/`active` bytes. */
typedef struct SlotElem {
    int word0;
    short pad04;
    unsigned short cnt;
    unsigned short h08;
    short pad0a;
    unsigned char b0c;
    unsigned char pad0d;
    unsigned char state;   /* +0x0e -- PMF index into data_ov006_02142e88 */
    unsigned char pad0f;
    unsigned char active;  /* +0x10 -- dispatch runs only while set */
    unsigned char b11;
    unsigned char pad12[0xe];
} SlotElem;
typedef struct S {
    char pad[0x4bac];
    SlotElem arr[2];
} S;

/* func_ov006_0211e118's view of the 0x10-stride dMeter_c sprite array at +0x4960. */
struct HudElem {
    int x;
    int y;
    int unk8;
    unsigned char idx;
    unsigned char unkd;
    unsigned char flag;
    unsigned char unkf;
};
struct HudArray {
    char pad[0x4960];
    struct HudElem arr[16];
};

/* func_ov006_0211e220's 0x10-stride step over the same array. */
struct E29 { unsigned char b[0x10]; };

/* func_ov006_0211e5cc's two windows onto the same object. */
typedef struct {
    char _pad0[0x4677];
    u8 active;   /* +0x4677 */
    u8 stage;    /* +0x4678 */
} View;
typedef struct {
    char _pad0[0x4c20];
    u8 latch;    /* +0x4c20 */
} Work;

/* Everything this file calls or reads that decl_common.h does not declare. */
extern "C" {

extern void  RenderOamMainScreen(void*, int, int, int, int);
extern void *LoadFile(int handle);
extern void  DecompressLZ16(const void *src, void *dst);
extern int   RandomIntInternal(int *seed);
extern void  FreeGfxSlotsById(int arg);
extern int   GetGameLanguage(void);
extern void  DrawOamSprite(int, int, int, int);
extern void  Hud_RenderSprite(void *fn, int a, int b, int c, int d);
extern void  func_ov004_020b2220(int, int, int, int, int, int, int);
extern void  func_ov004_020b1e34(void *, int, int, int);
extern int   func_ov004_020ae5c4(void *a, int b, int c, int d, int e, int f, int g);
extern int   func_ov004_020adbe0(void);
extern int   func_020126e8(int a);
extern int   func_02012468(int a, int b, int c, int d, int e, int f, int g, short h);
extern void  MultiStore16(u16 val, void *dst, int nbytes);
/* func_ov006_0211ebdc and 0211f224 pass `0x10 - alpha` as computed: the
   cartridge does not narrow it to 16 bits at either call, which G2x.h's
   unsigned short parameters would. So those two call the mangled name. */
extern void  _ZN3G2x13SetBlendAlphaEPVttttj(volatile unsigned short *reg, int a, int b,
                                            int c, int d);
/* G2's own name is taken: decl_common.h declares `int G2[]`. */
extern void *_ZN2G212GetBG2ScrPtrEv(void);
extern void *_ZN2G213GetBG2CharPtrEv(void);
extern void  _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(int sub, struct OamAttr *data,
                                                         s32 x, s32 y, s32 palette,
                                                         s32 priority, s32 scaleX,
                                                         s32 scaleY, s32 rotation,
                                                         s32 mode);

extern unsigned char  data_0209d454;
extern unsigned char  data_0209d45c;
extern int            data_ov006_021350fc[];
extern unsigned char  data_ov006_0212efc4[];
extern unsigned short data_ov006_0212efc8[];
extern unsigned char  data_ov006_0212efcc[];
extern unsigned char  data_ov006_0212efd4[];
extern unsigned short data_ov006_0212efdc[];
extern void          *data_ov006_02133a5c;
extern void          *data_ov006_02134210[];
extern int            data_ov006_02135fc8[];
extern int            data_ov006_02137cd8[];
extern void          *data_ov006_0213a5f4;
extern struct OamAttr *data_ov006_0213a628[];
extern int           *data_ov006_0213f9d0[];
extern void          *data_ov006_0213a964[];
extern int            data_ov006_0212efec[];
extern int            data_ov006_0212f014[];
extern int            data_ov006_0212f050[];
extern int            data_ov006_0212f08c[];
extern int            data_ov006_0213f9e4[];
extern short          data_02082214[];
extern int            data_ov004_020beb6c;

/* The four pointer-to-member state tables. Pmf handlers take the Boo/slot
   index; the scene table in data_ov006_02142eb0 takes none. */
typedef void (dScMgTeresa_c::*Pmf)(int);
typedef void (dScMgTeresa_c::*Pmf0)();
struct PmfEntry { Pmf0 pmf; };

extern Pmf data_ov006_02142f18[];   /* big Boo flight states, by Boo::state */
extern Pmf data_ov006_02142e88[];   /* reveal-pair states, by SlotElem::state */
extern Pmf data_ov006_02142ed8[];   /* per-Boo states, one call per live row */
extern PmfEntry data_ov006_02142eb0[]; /* scene states, by unk_4be8 */

}  /* extern "C" */

namespace Sound { void PlayBank2_2D(unsigned int id); }
namespace G2S {
char *GetBG0CharPtr();
void *GetBG0ScrPtr();
void *GetBG2CharPtr();
}
namespace GX {
void LoadBGPltt(const void *src, u32 offset, u32 size);
void LoadOBJPltt(const void *src, u32 offset, u32 size);
}
namespace GXS {
void LoadBGPltt(const void *src, u32 offset, u32 size);
void LoadOBJPltt(const void *src, u32 offset, u32 size);
}


// @symbol _ZN13dScMgTeresa_cD1Ev
// @symbol _ZN13dScMgTeresa_cD0Ev
/* One out-of-line definition; with codegen not deferred it emits D1, then
 * D0, as in the ROM. */
dScMgTeresa_c::~dScMgTeresa_c()
{
}


// @symbol _ZN13dScMgTeresa_c19func_ov006_0211cc2cEv
void dScMgTeresa_c::func_ov006_0211cc2c()
{
    unsigned char *raw = (unsigned char *)this;
    if (raw[0x4c1f]) {
        BOO(raw)->frame = 3;
    } else {
        BOO(raw)->frame = 4;
    }
    BOO(raw)->x = 0x80000;
    BOO(raw)->y = 0x100000;
    BOO(raw)->yVel = -0x1800;
    BOO(raw)->frameTimer = 0;
    BOO(raw)->moveRight = 0;
    BOO(raw)->state = 8;
    BOO(raw)->shown = 1;
    raw[0x4c1b] = 0;
}


// @symbol _ZN13dScMgTeresa_c19func_ov006_0211cc90Ev
void dScMgTeresa_c::func_ov006_0211cc90()
{
    unsigned char *base = (unsigned char *)this;
    base += 0x4000;
    if (base[0xbe0] != 0) {
        base[0xbe5] = 1;
    }
}


// @symbol _ZN13dScMgTeresa_c19func_ov006_0211cca8Ev
/* decl_common.h declares this one with a `void *` parameter. */
void dScMgTeresa_c::func_ov006_0211cca8()
{
    void *arg = this;
  unsigned char *raw = (unsigned char *)arg;
  int idx,v,p,q;
  if(BOO(raw)->shown==0) return;
  idx=BOO(raw)->frame;
  p=BOO(raw)->x>>12;
  q=BOO(raw)->y>>12;
  if(BOO(raw)->moveRight!=0) idx+=4;
  if(BOO(raw)->state==8) v=data_ov006_02135fc8[idx];
  else v=data_ov006_021350fc[idx];
  RenderOamMainScreen((void*)v,p,q,-1,-1);
}


#define FLAG  (*(u8*)((char*)raw + 0x4c1f))
// @symbol _ZN13dScMgTeresa_c19func_ov006_0211cd24Ei
void dScMgTeresa_c::func_ov006_0211cd24(int idx)
{
    void *raw = this;
    struct BooWalk *w = (struct BooWalk *)raw;

    if ((w->boo[idx].y >> 12) == 0xc0) {
        w->boo[idx].frameTimer++;
        if (FLAG != 0) {
            if (w->boo[idx].frame >= 3) return;
            if (w->boo[idx].frameTimer < 4) return;
            w->boo[idx].frameTimer = 0;
            w->boo[idx].frame++;
        } else {
            if (w->boo[idx].frame >= 6) return;
            if (w->boo[idx].frameTimer < 8) return;
            w->boo[idx].frameTimer = 0;
            w->boo[idx].frame++;
        }
    } else {
        w->boo[idx].shown = 1;
        w->boo[idx].y += w->boo[idx].yVel;
        w->boo[idx].yVel -= 0x200;
        if ((w->boo[idx].y >> 12) > 0xc0) return;
        w->boo[idx].y = 0xc0000;
        w->boo[idx].yVel = 0;
        w->boo[idx].frameTimer = 0;
        if (FLAG != 0) w->boo[idx].frame = 0;
    }
}
#undef FLAG


// @symbol _ZN13dScMgTeresa_c19func_ov006_0211ce90Ev
void dScMgTeresa_c::func_ov006_0211ce90()
{
}


// @symbol _ZN13dScMgTeresa_c19func_ov006_0211ce94Ei
void dScMgTeresa_c::func_ov006_0211ce94(int index)
{
    struct BooWalk *base = (struct BooWalk *)this;
    void *data;
    base->boo[index].xVel = 0;
    base->boo[index].yVel = 0;
    base->boo[index].shown = 0;
    data = LoadFile(0x103);
    DecompressLZ16(data, (void *)0x6400000);
    base->boo[index].state = 7;
    Deallocate(data);
}


// @symbol _ZN13dScMgTeresa_c19func_ov006_0211cef4Ei
void dScMgTeresa_c::func_ov006_0211cef4(int i)
{
    char *c = (char *)this;
    struct BooWalk *w = (struct BooWalk *)c;
    unsigned short *p16 = &w->boo[i].frameTimer;
    int *sum;
    int *add;
    int v;
    *p16 = *p16 + 1;
    if (*p16 >= 8) {
        unsigned char *p8;
        int z = 0;
        *p16 = z;
        p8 = &w->boo[i].frame;
        *p8 = *p8 + 1;
        if (*p8 >= 4) *p8 = z;
    }
    add = &w->boo[i].xVel;
    sum = &w->boo[i].x;
    *sum = *sum + *add;
    v = *sum >> 12;
    if (w->boo[i].moveRight != 0) {
        if (v >= 0x120) {
            w->boo[i].state = 6;
            return;
        }
        if (*add <= 0x3000) *add = *add + 0x200;
    } else {
        if (v <= -0x20) {
            w->boo[i].state = 6;
            return;
        }
        if (*add >= -0x3000) *add = *add - 0x200;
    }
}


// @symbol _ZN13dScMgTeresa_c19func_ov006_0211d018Ei
void dScMgTeresa_c::func_ov006_0211d018(int idx)
{
    char *raw = (char *)this;
  struct BooWalk *w = (struct BooWalk *)raw;
  int off = idx * 0x1c;
  if (w->boo[idx].done != 0)
  {
    w->boo[idx].state = 6;
    return;
  }
  unsigned short *wait = (unsigned short *)(BOO_FIELD(raw, wait) + off);
  if (*wait != 0)
  {
    *wait = *wait - 1;
    return;
  }
  w->boo[idx].shown = 1;
  if ((w->boo[idx].x >> 0xc) < 0)
  {
    w->boo[idx].moveRight = 1;
    w->boo[idx].x = -0x80000;
    w->boo[idx].xVel = 0x4800;
    w->boo[idx].state = 3;
    return;
  }
  w->boo[idx].moveRight = 0;
  w->boo[idx].x = 0x180000;
  w->boo[idx].xVel = -0x4800;
  w->boo[idx].state = 2;
}


// @symbol _ZN13dScMgTeresa_c19func_ov006_0211d0f8Ei
void dScMgTeresa_c::func_ov006_0211d0f8(int i)
{
    char *raw = (char *)this;
    struct BooWalk *w = (struct BooWalk *)raw;
    int v;
    w->boo[i].frameTimer++;
    if (w->boo[i].frameTimer >= 4) {
        w->boo[i].frameTimer = 0;
        w->boo[i].frame++;
        if (w->boo[i].frame >= 4)
            w->boo[i].frame = 0;
    }
    w->boo[i].x += w->boo[i].xVel;
    v = w->boo[i].x >> 0xc;
    if (v >= 0xc0) {
        if (w->boo[i].xVel >= 0x800)
            w->boo[i].xVel -= 0x80;
    } else {
        if (w->boo[i].xVel <= 0x7000)
            w->boo[i].xVel += 0x400;
    }
    if (v >= 0x120) {
        w->boo[i].xVel = 0;
        w->boo[i].x = 0x120000;
        w->boo[i].state = 4;
        w->boo[i].wait = 8;
        w->boo[i].shown = 0;
        if (w->boo[i].done != 0)
            w->boo[i].state = 6;
    }
}


// @symbol _ZN13dScMgTeresa_c19func_ov006_0211d224Ei
void dScMgTeresa_c::func_ov006_0211d224(int i)
{
    char *raw = (char *)this;
    struct BooWalk *w = (struct BooWalk *)raw;
    int t;

    w->boo[i].frameTimer++;
    if (w->boo[i].frameTimer >= 4) {
        w->boo[i].frameTimer = 0;
        w->boo[i].frame++;
        if (w->boo[i].frame >= 4) {
            w->boo[i].frame = 0;
        }
    }

    w->boo[i].x = w->boo[i].x + w->boo[i].xVel;
    t = w->boo[i].x >> 0xc;
    if (t <= 0x40) {
        if (w->boo[i].xVel <= -0x800) {
            w->boo[i].xVel = w->boo[i].xVel + 0x80;
        }
    } else {
        if (w->boo[i].xVel >= -0x7000) {
            w->boo[i].xVel = w->boo[i].xVel - 0x400;
        }
    }
    if (t > -0x20) {
        return;
    }
    w->boo[i].x = -0x20000;
    w->boo[i].xVel = 0;
    w->boo[i].state = 4;
    w->boo[i].wait = 8;
    w->boo[i].shown = 0;
    if (w->boo[i].done != 0) {
        w->boo[i].state = 6;
    }
}


// @symbol _ZN13dScMgTeresa_c19func_ov006_0211d368Ei
void dScMgTeresa_c::func_ov006_0211d368(int i)
{
    char *raw = (char *)this;
    struct BooWalk *w = (struct BooWalk *)raw;
    unsigned char state = w->boo[i].frame;
    if ((unsigned char)(state - 8) < 2)
    {
        w->boo[i].frameTimer++;
        if (w->boo[i].frameTimer >= data_ov006_0212efc8[(unsigned char)(state - 8)])
        {
            w->boo[i].frameTimer = 0;
            w->boo[i].frame++;
        }
    }
    if (w->boo[i].wait != 0)
    {
        w->boo[i].wait--;
        return;
    }
    data_0209d45c &= ~4;
    SetBg3Offset(0, 0x100);
    {
    int rnd = RandomIntInternal(&data_0209d4b8);
    w->boo[i].state =
        data_ov006_0212efc4[((unsigned int)(((unsigned int)rnd >> 16) & 0x7fff) * 2) >> 15];
    }
    w->boo[i].frameTimer = 0;
    w->boo[i].frame = 0;
    if (w->boo[i].state == 2)
    {
        w->boo[i].moveRight = 0;
        w->boo[i].xVel = -0x4800;
        w->boo[i].x = 0x180000;
    }
    else
    {
        w->boo[i].moveRight = 1;
        w->boo[i].xVel = 0x4800;
        w->boo[i].x = -0x80000;
    }
    if (w->boo[i].done != 0)
    {
        w->boo[i].state = 5;
    }
}


// @symbol _ZN13dScMgTeresa_c19func_ov006_0211d4e8Ei
void dScMgTeresa_c::func_ov006_0211d4e8(int i)
{
    char *raw = (char *)this;
    int off = i * 0x1c;
    struct BooWalk *w = (struct BooWalk *)raw;
    int *y = (int *)BOO_FIELD(raw, y);
    int *yVel = (int *)BOO_FIELD(raw, yVel);
    w->boo[i].shown = 1;
    *(int *)((char *)y + off) = *(int *)((char *)y + off) + *(int *)((char *)yVel + off);
    *(int *)((char *)yVel + off) = *(int *)((char *)yVel + off) - 0x100;
    if (*(int *)((char *)y + off) >> 0xc > 0xc0)
        return;
    *(int *)((char *)y + off) = 0xc0000;
    *(int *)((char *)yVel + off) = 0;
    w->boo[i].state = 1;
    w->boo[i].wait = 0x30;
    func_ov004_020b0cac(0xc, 0x80, 0x40, 1, -1, 0xd);
}


// @symbol _ZN13dScMgTeresa_c19func_ov006_0211d5a8Ev
void dScMgTeresa_c::func_ov006_0211d5a8()
{
    unsigned char *raw = (unsigned char *)this;
    if (BOO(raw)->active == 0) return;
    (this->*data_ov006_02142f18[BOO(raw)->state])(0);
}


// @symbol _ZN13dScMgTeresa_c19func_ov006_0211d608Ev
void dScMgTeresa_c::func_ov006_0211d608()
{
    char *raw = (char *)this;
    BOO(raw)->active = 1;
    BOO(raw)->x = 0x80000;
    BOO(raw)->y = 0x100000;
    BOO(raw)->xVel = 0;
    BOO(raw)->yVel = -0x1800;
    BOO(raw)->frameTimer = 0;
    BOO(raw)->wait = 0;
    BOO(raw)->state = 0;
    BOO(raw)->frame = 8;
    BOO(raw)->moveRight = 0;
    BOO(raw)->done = 0;
    G2x::SetBlendAlpha((volatile unsigned short *)0x4000050, 0, 0xc, 0xc, 0x10);
}


// @symbol _ZN13dScMgTeresa_c19func_ov006_0211d688Ev
void dScMgTeresa_c::func_ov006_0211d688()
{
    char *raw = (char *)this;
    BOO(raw)->active = 0;
    BOO(raw)->shown = 0;
}


// @symbol _ZN13dScMgTeresa_c19func_ov006_0211d69cEv
void dScMgTeresa_c::func_ov006_0211d69c()
{
    char *raw = (char *)this;
    if (*(unsigned char *)(raw + 0x4c24) >= 8)
    {
        return;
    }
    (*(unsigned short *)(raw + 0x4c18))++;
    if (*(unsigned short *)(raw + 0x4c18) >= data_ov006_0212efdc[*(unsigned char *)(raw + 0x4c24)])
    {
        (*(unsigned char *)(raw + 0x4c24))++;
        *(unsigned short *)(raw + 0x4c18) = 0;
        if (*(unsigned char *)(raw + 0x4c24) & 1)
        {
            SetBg3Offset(0, 0);
        }
        else
        {
            SetBg3Offset(0, 0x100);
        }
    }
    if (*(unsigned char *)(raw + 0x4c24) >= 8)
    {
        SetBg3Offset(0, 0);
    }
}


// @symbol _ZN13dScMgTeresa_c19func_ov006_0211d75cEv
/* decl_common.h declares this one with a `void *` parameter. */
void dScMgTeresa_c::func_ov006_0211d75c()
{
    void *arg = this;
  char* raw = (char*)arg;
  if(*(unsigned char*)(raw + 0x4bc9)==0) return;
  RenderOamMainScreen(data_ov006_0213a5f4,
                      *(int*)(raw + 0x4bc0)>>0xc,
                      *(int*)(raw + 0x4bc4)>>0xc,
                      -1, -1);
}


// @symbol _ZN13dScMgTeresa_c19func_ov006_0211d7b0Ev
/* Empty. func_ov006_021200a8 calls it with the object. */
void dScMgTeresa_c::func_ov006_0211d7b0()
{
}


// @symbol _ZN13dScMgTeresa_c19func_ov006_0211d7b4Ev
void dScMgTeresa_c::func_ov006_0211d7b4()
{
    char *raw = (char *)this;
    *(int *)(raw + 0x4bc0) = 327680;
    *(int *)(raw + 0x4bc4) = 262144;
    *(char *)(raw + 0x4bc8) = 1;
    *(char *)(raw + 0x4bc9) = 1;
}


// @symbol _ZN13dScMgTeresa_c19func_ov006_0211d7d8Ev
void dScMgTeresa_c::func_ov006_0211d7d8()
{
    char *raw = (char *)this;
    *(char *)(raw + 0x4bc8) = 0;
    *(char *)(raw + 0x4bc9) = 0;
}


// @symbol _ZN13dScMgTeresa_c19func_ov006_0211d7ecEv
/* decl_common.h declares this one with a `void *` parameter. */
void dScMgTeresa_c::func_ov006_0211d7ec()
{
    void *arg = this;
    char *raw = (char *)arg;
    if (*(unsigned char *)(raw + 0x4bb9) == 0) return;
    _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(
        0,
        data_ov006_0213a628[*(unsigned char *)(raw + 0x4bb8)],
        *(int *)(raw + 0x4ba0) >> 12,
        *(int *)(raw + 0x4ba4) >> 12,
        -1, 0,
        0x1000, 0x1000,
        *(unsigned short *)(raw + 0x4bb0), -1);
}


// @symbol _ZN13dScMgTeresa_c19func_ov006_0211d86cEi
void dScMgTeresa_c::func_ov006_0211d86c(int idx)
{
    char *raw = (char *)this;
    char *base = raw + (idx << 5);
    if (*(unsigned short*)(base + 0x4bb4) != 0) {
        *(unsigned short*)(raw + 0x4bb4 + (idx << 5)) =
            *(unsigned short*)(raw + 0x4bb4 + (idx << 5)) - 1;
        return;
    }
    *(unsigned char*)(base + 0x4bb9) = 0;
    *(unsigned char*)(base + 0x4bbc) = 0;
    func_ov006_0211d7d8();
    data_0209d45c |= 4;
    data_0209d454 |= 1;
    Sound::PlayBank2_2D(0x1f6);
    *(int*)(raw + 0x4be8) = 2;
    func_ov006_0211f51c();
    func_ov006_0211d608();
    Sound::PlayBank2_2D(0x1f5);
}


#pragma push
#pragma opt_common_subs off
#define AT(p,off) ((void*)(int)((char*)(p)+(off)))
// @symbol _ZN13dScMgTeresa_c19func_ov006_0211d924Ei
void dScMgTeresa_c::func_ov006_0211d924(int i)
{
    char *raw = (char *)this;
    u8 flag;

    flag = *(u8*)(raw + 0x4000 + (i << 5) + 0xbbe);

    if (flag == 0) {
        *(u16*)(raw + 0x4bb4 + (i << 5)) += 1;
        *(int*)(raw + 0x4ba4 + (i << 5)) += *(int*)(raw + 0x4000 + (i << 5) + 0xbac);
        *(int*)AT(raw, 0x4bc4) += *(int*)(raw + 0x4000 + (i << 5) + 0xbac);
        if (*(int*)(raw + 0x4000 + (i << 5) + 0xbac) >= 0x800)
            *(int*)(raw + 0x4bac + (i << 5)) -= 0x20;
        if (*(u16*)(raw + 0x4b00 + (i << 5) + 0xb4) >= 0x10) {
            *(u16*)(raw + 0x4b00 + (i << 5) + 0xb4) = 0;
            *(u8*)(raw + 0x4bbe + (i << 5)) += 1;
            *(int*)(raw + 0x4000 + (i << 5) + 0xbac) = 0;
        }
    } else if (flag == 1) {
        *(u16*)(raw + 0x4bb4 + (i << 5)) += 1;
        if (*(u16*)(raw + 0x4b00 + (i << 5) + 0xb4) == 0x10)
            *(int*)(raw + 0x4000 + (i << 5) + 0xbac) = -0x1000;
        *(int*)(raw + 0x4ba4 + (i << 5)) += *(int*)(raw + 0x4000 + (i << 5) + 0xbac);
        *(int*)AT(raw, 0x4bc4) += *(int*)(raw + 0x4000 + (i << 5) + 0xbac);
        if (*(int*)(raw + 0x4000 + (i << 5) + 0xbac) <= -0x800)
            *(int*)(raw + 0x4bac + (i << 5)) += 0x20;
        if (*(u16*)(raw + 0x4b00 + (i << 5) + 0xb4) >= 0x20) {
            *(u16*)(raw + 0x4b00 + (i << 5) + 0xb4) = 0x20;
            *(u8*)(raw + 0x4000 + (i << 5) + 0xbbe) = 0;
            *(u8*)(raw + 0x4000 + (i << 5) + 0xbba) = 4;
        }
    }
}
#undef AT
#pragma pop


// @symbol _ZN13dScMgTeresa_c19func_ov006_0211dad0Ei
void dScMgTeresa_c::func_ov006_0211dad0(int i)
{
    S *s = (S *)this;
    s->arr[i].cnt++;
    if (s->arr[i].cnt < 8) return;
    s->arr[i].cnt = 0;
    s->arr[i].b11++;
    if (s->arr[i].b11 >= 8) {
        s->arr[i].b11 = 0;
        s->arr[i].b0c = 0;
        s->arr[i].state = 3;
        s->arr[i].h08 = 0;
        s->arr[i].word0 = 0x1000;
        Sound::PlayBank2_2D(0x1f9);
    } else {
        s->arr[i].b0c = data_ov006_0212efd4[s->arr[i].b11];
    }
}


#pragma push
#pragma opt_common_subs off
#define A16(off) (*(unsigned short *)(raw + (off) + i * 32))
#define A8(off)  (*(unsigned char  *)(raw + (off) + i * 32))
#define A32(off) (*(int            *)(raw + (off) + i * 32))
#define B16(off) (*(unsigned short *)(raw + i * 32 + (off)))
#define B8(off)  (*(unsigned char  *)(raw + i * 32 + (off)))
#define B32(off) (*(int            *)(raw + i * 32 + (off)))
// @symbol _ZN13dScMgTeresa_c19func_ov006_0211db7cEi
void dScMgTeresa_c::func_ov006_0211db7c(int i)
{
    char *raw = (char *)this;
    A16(0x4bb2) += 1;
    if (B16(0x4bb2) >= 8) {
        B16(0x4bb2) = 0;
        A8(0x4bbd) += 1;
        if (B8(0x4bbd) >= 8)
            B8(0x4bbd) = 0;
        B8(0x4bb8) = data_ov006_0212efcc[B8(0x4bbd)];
    }
    if (B32(0x4ba0) >> 12 <= 0x80) {
        B32(0x4ba0) = 0x80000;
    } else {
        A32(0x4ba0) += B32(0x4ba8);
        if (B32(0x4ba8) <= -0x600)
            A32(0x4ba8) += 0x20;
    }
    if (B16(0x4bb4) != 0) {
        A16(0x4bb4) -= 1;
        return;
    }
    Sound::PlayBank2_2D(0x1f3);
    B8(0x4bba) = 2;
    B8(0x4bbd) = 0;
    B8(0x4bb8) = 0;
    B16(0x4bb2) = 0;
    B32(0x4ba8) = 0;
}
#undef A16
#undef A8
#undef A32
#undef B16
#undef B8
#undef B32
#pragma pop


// @symbol _ZN13dScMgTeresa_c19func_ov006_0211dce0Ei
void dScMgTeresa_c::func_ov006_0211dce0(int i)
{
    char *raw = (char *)this;
  char *p = raw + (i << 5);
  *((raw + (i << 5)) + 0x4bba) = 1;
  *((short *) ((raw + (i << 5)) + 0x4bb4)) = 0x40;
  *((int *) (p + 0x4ba8)) = -0xb00;
}


// @symbol _ZN13dScMgTeresa_c19func_ov006_0211dd0cEv
void dScMgTeresa_c::func_ov006_0211dd0c()
{
    S *s = (S *)this;
    if (s->arr[0].active == 0) return;
    (this->*data_ov006_02142e88[s->arr[0].state])(0);
}


// @symbol _ZN13dScMgTeresa_c19func_ov006_0211dd6cEv
void dScMgTeresa_c::func_ov006_0211dd6c()
{
    char *raw = (char *)this;
    *(int *)(raw + 0x4ba0) = 655360;
    *(int *)(raw + 0x4ba4) = 393216;
    *(char *)(raw + 0x4bbc) = 1;
    *(char *)(raw + 0x4bb9) = 1;
    *(char *)(raw + 0x4bba) = 0;
    *(char *)(raw + 0x4bb8) = 0;
    *(short *)(raw + 0x4bb0) = 0;
    *(short *)(raw + 0x4bb2) = 0;
    *(short *)(raw + 0x4bb4) = 0;
    *(char *)(raw + 0x4bbb) = 0;
    *(char *)(raw + 0x4bbd) = 0;
    *(char *)(raw + 0x4bbe) = 0;
}


// @symbol _ZN13dScMgTeresa_c19func_ov006_0211ddb8Ev
void dScMgTeresa_c::func_ov006_0211ddb8()
{
    char *raw = (char *)this;
    *(char *)(raw + 0x4bb9) = 0;
    *(char *)(raw + 0x4bbc) = 0;
}


// @symbol _ZN13dScMgTeresa_c19func_ov006_0211ddccEv
/* decl_common.h declares this one with a `void *` parameter, and
   func_ov004_020af948 as `(void *, int, int, void *)`. */
void dScMgTeresa_c::func_ov006_0211ddcc()
{
    void *c_ = this;
    char *c = (char *)c_;
    int i;
    for (i = 0; i < 0x10; i++) {
        if (*(unsigned char*)(c + 0x4a71)) {
            int b = (*(int*)(c + 0x4a64)) >> 12;
            int a = (*(int*)(c + 0x4a60)) >> 12;
            if (b <= 8) b = 8;
            if (b >= 0xb8) b = 0xb8;
            func_ov004_020af948((void*)data_ov006_02137cd8[*(unsigned char*)(c + 0x4a72) + 1], a, b, (void*)0);
        }
        c += 0x14;
    }
}


// @symbol _ZN13dScMgTeresa_c19func_ov006_0211de54Ev
void dScMgTeresa_c::func_ov006_0211de54()
{
    char *p = (char *)this;
    int i;
    for (i = 0; i < 0x10; i++) {
        *(unsigned char *)(p + 0x4a70) = 0;
        *(unsigned char *)(p + 0x4a71) = 0;
        p += 0x14;
    }
}


// @symbol _ZN13dScMgTeresa_c19func_ov006_0211de7cEv
int dScMgTeresa_c::func_ov006_0211de7c()
{
    char *c = (char *)this;
    int cnt = 0;
    int i;
    for (i = 0; i < 0x10; i++) {
        if (*(unsigned char*)(c + 0x4a70) != 0 && *(unsigned char*)(c + 0x4a73) != 2) {
            cnt++;
        }
        c += 0x14;
    }
    return cnt != 0;
}


// @symbol _ZN13dScMgTeresa_c19func_ov006_0211dec0Ev
void dScMgTeresa_c::func_ov006_0211dec0()
{
    void *arg = this;
    int i;
    char *p = (char *)arg;
    for (i = 0; i < 16; i++, p += 0x14) {
        unsigned char v;
        if (*(unsigned char *)(p + 0x4a70) == 0) {
            continue;
        }
        v = *(unsigned char *)(p + 0x4a73);
        if (v == 0) {
            if (*(int *)(p + 0x4a6c) != 0) {
                *(int *)(p + 0x4a6c) -= 1;
                if ((short)*(int *)(p + 0x4a6c) < 0) {
                    *(int *)(p + 0x4a6c) = 0;
                }
            } else {
                Sound::PlayBank2_2D(0x1bc);
                *(int *)(p + 0x4a68) = -0x400;
                *(int *)(p + 0x4a6c) = 0x10;
                *(unsigned char *)(p + 0x4a73) += 1;
                *(unsigned char *)(p + 0x4a71) = 1;
            }
        } else if (v == 1) {
            *(int *)(p + 0x4a64) += *(int *)(p + 0x4a68);
            *(int *)(p + 0x4a68) -= 0x80;
            if (*(int *)(p + 0x4a6c) != 0) {
                *(int *)(p + 0x4a6c) -= 1;
                if ((short)*(int *)(p + 0x4a6c) < 0) {
                    *(int *)(p + 0x4a6c) = 0;
                }
            } else {
                *(int *)(p + 0x4a6c) = 0x18;
                *(unsigned char *)(p + 0x4a73) += 1;
            }
        }
    }
}


// @symbol _ZN13dScMgTeresa_c19func_ov006_0211e020Ei
void dScMgTeresa_c::func_ov006_0211e020(int i)
{
    char *raw = (char *)this;
  int n;
  char *p = raw;
  for (n = 0; n < 0x10; n++)
  {
    if (((*((unsigned char *) (p + 0x4a70))) == 0) != 0)
    {
      char *d = (raw + (n * 0x14)) + 0x4000;
      char *s = raw + (i * 0x24);
      unsigned char *tbl = (unsigned char *) (raw + 0x4680);
      *((unsigned char *) (((raw + (n * 0x14)) + 0x4000) + 0xa70)) = 1;
      *((unsigned char *) (d + 0xa71)) = 0;
      *((int *) (((raw + (n * 0x14)) + 0x4000) + 0xa60)) = *((int *) ((raw + (i * 0x24)) + 0x4660));
      *((int *) (((raw + (n * 0x14)) + 0x4000) + 0xa64)) = (*((int *) (s + 0x4664))) - 0x10000;
      *((int *) (((raw + (n * 0x14)) + 0x4000) + 0xa6c)) = (tbl[i * 0x24] * 0x3c) + 0x10;
      *((unsigned char *) (((raw + (n * 0x14)) + 0x4000) + 0xa73)) = 0;
      *((unsigned char *) (((raw + (n * 0x14)) + 0x4000) + 0xa72)) = ((unsigned char *) (raw + 0x4680))[i * 0x24];
      return;
    }
    p += 0x14;
  }

}


// @symbol _ZN13dScMgTeresa_c19func_ov006_0211e0c8Ev
/* The call to func_ov006_0211e020 is cast because that function takes
   `char *`. */
void dScMgTeresa_c::func_ov006_0211e0c8()
{
    RowArray *c = (RowArray *)this;
    int i;
    struct Row* r = c->rows;
    for (i = 0; i < 0x10; i++) {
        if (((unsigned char*)r + 0x4000)[0x677] != 0 && ((unsigned char*)r + 0x4000)[0x678] == 5)
            func_ov006_0211e020(i);
        r++;
    }
}


// @symbol _ZN13dScMgTeresa_c19func_ov006_0211e118Ev
/* decl_common.h declares this one with a `void *` parameter. */
void dScMgTeresa_c::func_ov006_0211e118()
{
    void *a0_ = this;
    struct HudArray* a0 = (struct HudArray*)a0_;
    int i;
    for (i = 0; i < 16; i++) {
        if (a0->arr[i].flag) {
            Hud_RenderSprite(data_ov006_02134210[a0->arr[i].idx], a0->arr[i].x >> 12, a0->arr[i].y >> 12, -1, -1);
        }
    }
}


#define LI(i) ((int)((long long)(i)) * 0x10)
#define A(p) ((int)(p))
// @symbol _ZN13dScMgTeresa_c19func_ov006_0211e184Ev
void dScMgTeresa_c::func_ov006_0211e184()
{
    char *raw = (char *)this;
    int i;

    for (i = 0; i < 0x10; i++) {
        unsigned char *fp = (unsigned char *)A(raw + LI(i) + 0x496d);
        unsigned short *c16;
        unsigned char *c8;
        if (*fp == 0) continue;
        c16 = (unsigned short *)A(raw + LI(i) + 0x4968);
        *c16 += 1;
        if (*c16 < 4) continue;
        *c16 = 0;
        c8 = (unsigned char *)A(raw + LI(i) + 0x496c);
        *c8 += 1;
        if (*c8 >= 5) {
            *c8 = 0;
            *(unsigned char *)(raw + LI(i) + 0x496e) = 0;
            *fp = 0;
        }
    }
}
#undef LI
#undef A


#pragma push
#pragma opt_strength_reduction off
// @symbol _ZN13dScMgTeresa_c19func_ov006_0211e220Ei
void dScMgTeresa_c::func_ov006_0211e220(int param)
{
    unsigned char *raw = (unsigned char *)this;
  struct E29* a=(struct E29*)raw;
  int i;
  for(i=0;i<16;i++){
    unsigned char* s=(unsigned char*)&a[i];
    unsigned char* base = s + 0x4000;
    if(base[0x96d]==0){
      unsigned char* src = raw + param*0x24;
      src += 0x4000;
      int v1 = *(int*)(src + 0x660);
      unsigned char* base2 = s + 0x4900;
      *(int*)(base + 0x960) = v1;
      int v2 = *(int*)(src + 0x664);
      int zero = 0;
      *(int*)(base + 0x964) = v2;
      *(short*)(base2 + 0x68) = zero;
      base[0x96c] = zero;
      base[0x96d] = 1;
      base[0x96e] = 1;
      return;
    }
  }
}
#pragma pop


// @symbol _ZN13dScMgTeresa_c19func_ov006_0211e29cEv
/* decl_common.h declares this one with a `void *` parameter. */
void dScMgTeresa_c::func_ov006_0211e29c()
{
    void *arg = this;
  unsigned char* raw = (unsigned char*)arg;
  if(*(unsigned char*)(raw+0x4c1b)==0) return;
  func_ov004_020b2220(0x80,0x60,*(unsigned short*)(raw+0x4c14),1,0,0x800,0);
  DrawOamSprite(data_ov006_0213f9d0[GetGameLanguage()][3],0x80,0x48,0);
}


// @symbol _ZN13dScMgTeresa_c19func_ov006_0211e318Ev
void dScMgTeresa_c::func_ov006_0211e318()
{
    char *raw = (char *)this;
  if (*(unsigned char*)(raw + 0x4c1c) == 0) return;
  if (*(unsigned short*)(raw + 0x4c14) == 0) return;
  *(unsigned char*)(((int)raw + 0x4c1a)) = *(unsigned char*)(((int)raw + 0x4c1a)) + 1;
  if (*(unsigned char*)(raw + 0x4c1a) >= 0x3c){
    *(unsigned char*)(raw + 0x4c1a) = 0;
    *(unsigned short*)(((int)raw + 0x4c14)) = *(unsigned short*)(((int)raw + 0x4c14)) - 1;
    if (*(short*)(raw + 0x4c14) < 0) *(short*)(raw + 0x4c14) = 0;
    if (*(unsigned short*)(raw + 0x4c14) != 0)
      Sound::PlayBank2_2D(0xa7);
    else
      Sound::PlayBank2_2D(0xa6);
  }
  if (*(unsigned short*)(raw + 0x4c14) == 0){
    *(unsigned short*)(raw + 0x4c14) = 0;
    *(unsigned char*)(raw + 0x4c1a) = 0;
  }
}


// @symbol _ZN13dScMgTeresa_c19func_ov006_0211e3e0Ev
void dScMgTeresa_c::func_ov006_0211e3e0()
{
    char *raw = (char *)this;
    if (*(unsigned char *)(raw + 0x4c20) == 0) return;
    if (*(unsigned char *)(raw + 0x4c1c) != 0) return;
    if (*(unsigned int *)(raw + 0xbc) >= 0xf)
        *(unsigned short *)(raw + 0x4c14) = 2;
    else
        *(unsigned short *)(raw + 0x4c14) = 3;
    *(unsigned char *)(raw + 0x4c1a) = 0;
    *(unsigned char *)(raw + 0x4c1b) = 1;
    *(unsigned char *)(raw + 0x4c1c) = 1;
    Sound::PlayBank2_2D(0xa7);
}


// @symbol _ZN13dScMgTeresa_c19func_ov006_0211e460Ev
/* decl_common.h declares this one with a `void *` parameter. */
void dScMgTeresa_c::func_ov006_0211e460()
{
    void *c_ = this;
    char *c = (char *)c_;
    int i;
    for (i = 0; i < 0x10; i++) {
        if (*(unsigned char *)(c + 0x48aa) != 0) {
            int v0 = *(int *)(c + 0x48a0) >> 12;
            int v4 = *(int *)(c + 0x48a4) >> 12;
            if (v4 <= 8) v4 = 8;
            if (v4 >= 0xb8) v4 = 0xb8;
            Hud_RenderSprite(data_ov006_02133a5c, v0, v4, -1, -1);
        }
        c += 0xc;
    }
}


#define A(p) ((unsigned char *)(int)(p))
// @symbol _ZN13dScMgTeresa_c19func_ov006_0211e4e0Ev
void dScMgTeresa_c::func_ov006_0211e4e0()
{
    char *base = (char *)this;
    int i;

    for (i = 0; i < 0x10; i++) {
        if (*(unsigned char *)(base + 0x48a9) != 0) {
            if (*(unsigned char *)(base + 0x48a8) != 0) {
                unsigned char *p = A(base + 0x48a8);
                int v;
                *p -= 1;
                v = *(unsigned char *)(base + 0x48a8);
                if (v < 0)
                    *(unsigned char *)(base + 0x48a8) = 0;
            } else {
                *(unsigned char *)(base + 0x48a9) = 0;
                *(unsigned char *)(base + 0x48aa) = 0;
            }
        }
        base += 0xc;
    }
}
#undef A


// @symbol _ZN13dScMgTeresa_c19func_ov006_0211e55cEi
void dScMgTeresa_c::func_ov006_0211e55c(int idx)
{
    char *raw = (char *)this;
    int i;
    char* slot = raw;
    for (i = 0; i < 0x10; i++) {
        if (*(unsigned char*)(slot + 0x48a9) == 0) {
            *(unsigned char*)(raw + i * 0xc + 0x48a9) = 1;
            *(unsigned char*)(raw + i * 0xc + 0x48aa) = 1;
            *(int*)(raw + i * 0xc + 0x48a0) = *(int*)(raw + idx * 0x24 + 0x4660);
            *(int*)(raw + i * 0xc + 0x48a4) = *(int*)(raw + idx * 0x24 + 0x4664) - 0x18000;
            *(unsigned char*)(raw + i * 0xc + 0x48a8) = 0x60;
            return;
        }
        slot += 0xc;
    }
}


// @symbol _ZN13dScMgTeresa_c19func_ov006_0211e5ccEv
void dScMgTeresa_c::func_ov006_0211e5cc()
{
    char *raw = (char *)this;
    int found;
    int i;
    char* p;
    if (((Work*)raw)->latch != 0)
        return;
    found = 0;
    for (i = 0, p = raw; i < 0x10; i++, p += 0x24) {
        if (((View*)p)->active != 0) {
            if (((View*)p)->stage <= 2) {
                found++;
                break;
            }
        }
    }
    if (found != 0)
        return;
    FreeGfxSlotsById(0xc);
    (*(u8*)(raw + 0x4c20))++;
}


// @symbol _ZN13dScMgTeresa_c19func_ov006_0211e658Ev
void dScMgTeresa_c::func_ov006_0211e658()
{
    unsigned char *raw = (unsigned char *)this;
    if (*(unsigned short*)(raw + 0x4c14) == 0 && *(unsigned char*)(raw + 0x4c20) != 0) {
        *(unsigned char*)(raw + 0x4c1f) = 0;
        *(int*)(raw + 0x4be8) = 3;
        *(unsigned short*)(raw + 0x4c0c) = 0x60;
        func_ov006_0211cc90();
        *(unsigned char*)(raw + 0x4c27) = 1;
        return;
    }
    {
        int count = 0;
        int i = 0;
        unsigned char* p = raw;
        do {
            if (*(unsigned char*)(p + 0x4677) != 0) {
                if (*(unsigned char*)(p + 0x467c) == 0) {
                    count++;
                    break;
                }
            }
            i++;
            p += 0x24;
        } while (i < 0x10);
        if (count != 0)
            return;
        *(int*)(raw + 0x4be8) = 3;
        *(unsigned short*)(raw + 0x4c0c) = 0x60;
        func_ov006_0211cc90();
        *(unsigned char*)(raw + 0x4c1f) = 1;
    }
}


// @symbol _ZN13dScMgTeresa_c19func_ov006_0211e72cEv
/* Draws the 16 Boo sprites. Each 0x24-byte slot at +0x4660 holds a position
 * (x, y in 20.12), a translucency flag, a visible flag, an OAM priority and an
 * animation/frame pair that picks the sprite's attribute block from a table of
 * seven frames per animation. Visible slots go through OAM::Render at unit
 * scale, forced semi-transparent (mode 1) when the flag is set and in the
 * attribute's own mode (-1) otherwise. The mode is declared between the frame
 * index and the y read on purpose: that is what puts the -1 where the ROM
 * keeps it. decl_common.h declares this one with a `void *` parameter. */
void dScMgTeresa_c::func_ov006_0211e72c()
{
    void *arg = this;
    char *row = (char *)arg;
    int i;

    for (i = 0; i < 16; i++) {
        if (*(u8 *)(row + 0x4000 + 0x67a) != 0) {
            void **frames = data_ov006_0213a964;
            int translucent = *(u8 *)(row + 0x4000 + 0x676);
            int x = *(int *)(row + 0x4000 + 0x660) >> 12;
            int frame = *(u8 *)(row + 0x4000 + 0x67d);
            int anim = *(u8 *)(row + 0x4000 + 0x67e);
            int idx = anim * 7 + frame;
            int mode = -1;
            int y = *(int *)(row + 0x4000 + 0x664) >> 12;
            int priority = *(u8 *)(row + 0x4000 + 0x67b);

            if (translucent != 0)
                mode = 1;
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(
                1, (struct OamAttr *)frames[idx], x, y, -1, priority, 0x1000, 0x1000, 0, mode);
        }
        row += 0x24;
    }
}


// @symbol _ZN13dScMgTeresa_c19func_ov006_0211e7d8Ev
void dScMgTeresa_c::func_ov006_0211e7d8()
{
    char *self = (char *)this;
    int found = 0;
    int i = 0;
    char *p = self;
    for (; i < 0x10; i++) {
        if (*(unsigned char*)(p + 0x4677) != 0) {
            unsigned int t = (*(unsigned char*)(p + 0x4678) + 0xfd) & 0xff;
            if (t <= 1) {
                *(unsigned char*)(p + 0x4678) = 6;
                *(unsigned short*)(p + 0x466e) = 0;
                *(unsigned char*)(p + 0x4676) = 0;
                *(unsigned char*)(p + 0x4679) = 1;
                *(unsigned char*)(p + 0x467d) = 3;
                *(unsigned char*)(p + 0x4681) = 0;
                *(unsigned char*)(self + 0x4c22) = (unsigned char)i;
                *(unsigned short*)(p + 0x4674) = 1;
                found++;
            }
        }
        p += 0x24;
    }
    if (found != 0) {
        unsigned char idx = *(unsigned char*)(self + 0x4c22);
        *(unsigned char*)(self + idx * 0x24 + 0x4681) = 1;
    } else {
        unsigned char idx = *(unsigned char*)(self + 0x4c22);
        *(unsigned char*)(self + idx * 0x24 + 0x4681) = 0;
    }
}


#define A(a) (*(u8*)(a))
// @symbol _ZN13dScMgTeresa_c19func_ov006_0211e8a8Ei
void dScMgTeresa_c::func_ov006_0211e8a8(int idx)
{
    char *c = (char *)this;
    int x;
    int y;
    int count;
    int firstOk;
    int i;
    int* r;
    int word;
    int off;

    if (*(u8*)(c + 0x467c + idx * 0x24) != 0)
        return;

    count = 0;
    firstOk = 0;
    for (i = 0; i < 5; i++) {
        x = (*(int*)(c + idx * 0x24 + 0x4660) >> 12) - data_ov006_0212efec[i * 2];
        y = (*(int*)(c + idx * 0x24 + 0x4664) >> 12) - data_ov006_0212efec[i * 2 + 1];
        r = ((int (*)[8])G2S::GetBG0CharPtr())[(x >> 3) + ((y >> 3) << 5)];
        word = r[y & 7];
        if (((word >> ((x & 7) << 2)) & 0xf) == 0) {
            count++;
            if (i == 0)
                firstOk++;
        }
    }

    if (count < 2)
        return;
    if (firstOk == 0)
        return;

    *(u8*)(c + 0x467c + idx * 0x24) = 1;
    off = ((char (*)[0x24])0)[idx] - (char*)0;
    *(u8*)&((char (*)[0x24])c)[idx][0x467b] = 0;
    *(u8*)&((char (*)[0x24])c)[idx][0x4678] = 5;
    *(u8*)&((char (*)[0x24])c)[idx][0x4680] = *(u8*)(c + 0x4c21);
    *(s16*)&((char (*)[0x24])c)[idx][0x466e] = 0x40;
    A(c + 0x4c21)++;
    func_ov006_0211e55c(idx);
    func_02012718(0x1f0, *(int*)(c + off + 0x4660));
    if (*(u8*)(c + 0x4c26) == 0xff)
        *(u8*)(c + 0x4c26) = (u8)idx;
}
#undef A


#pragma push
#pragma opt_common_subs off
// @symbol _ZN13dScMgTeresa_c19func_ov006_0211ea70Ei
void dScMgTeresa_c::func_ov006_0211ea70(int idx)
{
    char *self = (char *)this;
    int gx, gy;
    int cnt;
    int y;
    int x;
    int *pA;
    int *pB;
    char *tile;
    int tidx;
    int *row;
    int word;
    int off;

    off = idx * 0x24;
    cnt = 0;
    pA = (int *)(self + off + 0x4660);
    pB = (int *)(self + off + 0x4664);
    for (y = 0; y < 8; y++) {
        for (x = 0; x < 8; x++) {
            gx = x + ((*pA >> 12) - 4);
            gy = y + ((*pB >> 12) - 4);
            tile = (char *)G2S::GetBG0CharPtr();
            tidx = (gx >> 3) + ((gy >> 3) << 5);
            row = (int *)(tile + (tidx << 5));
            /* pin row complete before nibble extract */
            row = (int *)((char *)row + (gy - gy));
            word = row[gy & 7];
            if (((word >> ((gx & 7) << 2)) & 0xf) == 0)
                cnt++;
        }
    }
    if (cnt == 0)
        return;
    if (*(u8 *)(self + off + 0x467f) == 0)
        return;
    *(u16 *)(self + 0x466c + idx * 0x24) += 0x8000;
}
#pragma pop


// @symbol _ZN13dScMgTeresa_c19func_ov006_0211eb90Ei
void dScMgTeresa_c::func_ov006_0211eb90(int i)
{
    char *c = (char *)this;
    if (*(unsigned char *)(c + i * 0x24 + 0x467f) == 0) {
        return;
    }
    func_ov006_0211f454(i);
    func_ov006_0211f34c(i);
}


// @symbol _ZN13dScMgTeresa_c19func_ov006_0211ebdcEi
void dScMgTeresa_c::func_ov006_0211ebdc(int i)
{
    char *c = (char *)this;
    u8 *s;
    int o = i * 0x24;
    u8 *fade;

    if (*(u16 *)(c + 0x4674 + o) != 0) {
        (*(u16 *)((c + 0x4674) + o))--;
        if (*(u16 *)((c + 0x4674) + o) == 0)
            Sound::PlayBank2_2D(0x1f1);
    }

    s = (u8 *)(c + 0x467d + o);
    if (*s != 6) {
        u16 *p70 = (u16 *)(c + 0x4670 + o);
        *p70 = *p70 + 1;
        if (*p70 >= 0xf) {
            *p70 = 0;
            (*s)++;
            if (*s >= 6)
                *s = 3;
        }
    }

    func_ov006_0211f454(i);
    func_ov006_0211f34c(i);

    {
        char *row = c + o;
        if (data_02082214[((*(u16 *)(row + 0x466c) >> 4) << 1) + 1] >= 0)
            *(u8 *)(row + 0x467e) = 1;
        else
            *(u8 *)(row + 0x467e) = 0;
    }

    if (*(u16 *)(((char *)c + o) + 0x4672) != 0) {
        u16 *p72 = (u16 *)((c + 0x4672) + o);
        *p72 = *p72 - 1;
        if (*p72 != 0)
            return;
        *(u16 *)((c + 0x466c) + o) += 0x8000;
        *s = 6;
        if (*(u8 *)(((char *)c + o) + 0x4681) != 0)
            Sound::PlayBank2_2D(0x1f4);
        return;
    }

    {
        u16 *p6e = (u16 *)(c + 0x466e + o);
        *p6e = *p6e + 1;
        if (*p6e < 4)
            return;
        fade = (u8 *)((c + 0x4676) + o);
        *fade = *fade + 1;
        *p6e = 0;
        if (i == *(u8 *)(c + 0x4c22)) {
            _ZN3G2x13SetBlendAlphaEPVttttj((volatile unsigned short *)0x4001050, 0, 4,
                0x10 - *fade, 0x10);
        }
        if (*fade >= 0x10) {
            *(u8 *)(c + 0x4677 + o) = 0;
            *(u8 *)(c + 0x467a + o) = 0;
        }
    }
}


// @symbol _ZN13dScMgTeresa_c19func_ov006_0211ee34Ei
void dScMgTeresa_c::func_ov006_0211ee34(int i)
{
    char *c = (char *)this;
    int k = i * 0x24;

    if (*(u16 *)(c + 0x4674 + k) != 0) {
        (*(u16 *)(c + 0x4674 + k))--;
        if (*(u16 *)(c + 0x4674 + k) == 0) {
            if (*(u8 *)(c + 0x4681 + k) != 0)
                Sound::PlayBank2_2D(0x1f1);
        }
    }

    (*(u16 *)(c + 0x4670 + k))++;
    if (*(u16 *)(c + 0x4670 + k) >= 0xf) {
        *(u16 *)(c + 0x4670 + k) = 0;
        (*(u8 *)(c + 0x467d + k))++;
        if (*(u8 *)(c + 0x467d + k) >= 6)
            *(u8 *)(c + 0x467d + k) = 3;
    }

    func_ov006_0211f454(i);
    func_ov006_0211f34c(i);

    if (data_02082214[2 * (*(u16 *)(c + 0x466c + k) >> 4) + 1] >= 0)
        *(u8 *)(c + 0x467e + k) = 1;
    else
        *(u8 *)(c + 0x467e + k) = 0;

    if (*(u8 *)(c + 0x4676 + k) >= 0x10)
        return;

    (*(u16 *)(c + 0x466e + k))++;
    if (*(u16 *)(c + 0x466e + k) < 4)
        return;

    (*(u8 *)(c + 0x4676 + k))++;
    *(u8 *)(c + 0x467b + k) = 0;
    *(u16 *)(c + 0x466e + k) = 0;

    if (i == *(u8 *)(c + 0x4c22)) {
        G2x::SetBlendAlpha((volatile unsigned short *)0x4001050, 0, 1,
            *(u8 *)(c + 0x4676 + k), 0x10);
    }

    if (*(u8 *)(c + 0x4676 + k) < 0x10)
        return;

    *(u16 *)(c + 0x4672 + k) = 0x80;
    *(u16 *)(c + 0x466e + k) = 0;
    *(u8 *)(c + 0x4676 + k) = 0;
    *(u8 *)(c + 0x4678 + k) = 7;
}


// @symbol _ZN13dScMgTeresa_c19func_ov006_0211f040Ei
void dScMgTeresa_c::func_ov006_0211f040(int idx)
{
    char *c = (char *)this;
    int off;
    if (*(int *)(c + 0x4000 + 0xbe8) != 4)
        return;
    off = idx * 0x24;
    if (*(unsigned short *)(c + 0x466e + off) != 0) {
        *(unsigned short *)(c + 0x466e + off) = *(unsigned short *)(c + 0x466e + off) - 1;
        return;
    }
    func_ov006_0211e220(idx);
    *(unsigned char *)(c + off + 0x4000 + 0x67a) = 0;
    *(unsigned char *)(c + off + 0x4000 + 0x677) = 0;
    if (*(unsigned char *)(c + 0x4000 + 0xc26) != idx)
        return;
    Sound::PlayBank2_2D(0x1c7);
}


// @symbol _ZN13dScMgTeresa_c19func_ov006_0211f0d0Ei
void dScMgTeresa_c::func_ov006_0211f0d0(int idx)
{
    unsigned char *base = (unsigned char *)this;
    int off = idx * 0x24;
    unsigned short* cnt = (unsigned short*)((unsigned char*)(base + 0x466e) + off);
    if (*cnt != 0) {
        *cnt = *cnt - 1;
        if (*(short*)cnt <= 0) {
            *cnt = 0;
            *(unsigned char*)((unsigned char*)(base + 0x4678) + off) = 3;
            return;
        }
    }
    func_ov006_0211f454(idx);
    func_ov006_0211f34c(idx);
    {
        int v = *(unsigned short*)((unsigned char*)(base + 0x466c) + off);
        short look = data_02082214[(v >> 4) * 2 + 1];
        if (look >= 0)
            *(unsigned char*)((unsigned char*)(base + 0x467e) + off) = 1;
        else
            *(unsigned char*)((unsigned char*)(base + 0x467e) + off) = 0;
    }
    func_ov006_0211e8a8(idx);
}


// @symbol _ZN13dScMgTeresa_c19func_ov006_0211f1a4Ei
void dScMgTeresa_c::func_ov006_0211f1a4(int i)
{
    char *c = (char *)this;
    char *e;
    int v;
    func_ov006_0211eb90(i);
    func_ov006_0211ea70(i);
    func_ov006_0211e8a8(i);
    e = c + i * 0x24;
    v = data_02082214[(*(unsigned short *)(e + 0x466c) >> 4) * 2 + 1];
    if (v >= 0)
        *(unsigned char *)(e + 0x467e) = 1;
    else
        *(unsigned char *)(e + 0x467e) = 0;
}


// @symbol _ZN13dScMgTeresa_c19func_ov006_0211f224Ei
void dScMgTeresa_c::func_ov006_0211f224(int i)
{
    char *c = (char *)this;
    int k;
    func_ov006_0211f454(i);
    func_ov006_0211f34c(i);
    k = i * 0x24;
    if (data_02082214[2 * (*(unsigned short *)(c + 0x466c + k) >> 4) + 1] >= 0)
        *(unsigned char *)(c + 0x467e + k) = 1;
    else
        *(unsigned char *)(c + 0x467e + k) = 0;
    (*(unsigned short *)(c + 0x466e + k))++;
    if (*(unsigned short *)(c + 0x466e + k) < 4)
        return;
    (*(unsigned char *)(c + 0x4676 + k))++;
    *(unsigned short *)(c + 0x466e + k) = 0;
    if (i == 0)
        _ZN3G2x13SetBlendAlphaEPVttttj((volatile unsigned short *)0x4001050, 0, 1,
            0x10 - *(unsigned char *)(c + 0x4676 + k), 0x10);
    if (*(unsigned char *)(c + 0x4676 + k) < 0x10)
        return;
    *(unsigned char *)(c + 0x4676 + k) = 0;
    *(unsigned char *)(c + 0x4678 + k) = 3;
    *(unsigned char *)(c + 0x467b + k) = 1;
}


// @symbol _ZN13dScMgTeresa_c19func_ov006_0211f34cEi
void dScMgTeresa_c::func_ov006_0211f34c(int i)
{
    char *o = (char *)this;
    int m = i * 0x24;
    int xt = *(int *)((char *)(((int)o + 0x4660)) + m) >> 12;
    int yt = *(int *)((char *)(((int)o + 0x4664)) + m) >> 12;
    if (xt + 8 > 0xfe) {
        u16 *pa = (u16 *)((char *)(((int)o + 0x466c)) + m);
        *pa = 0x8000 - *pa;
        *(int *)((char *)(((int)o + 0x4660)) + m) = 0xf6000;
    } else if (xt - 8 < 2) {
        u16 *pa = (u16 *)((char *)(((int)o + 0x466c)) + m);
        *pa = 0x8000 - *pa;
        *(int *)((char *)(((int)o + 0x4660)) + m) = 0xa000;
    }
    if (yt + 8 > 0xbe) {
        u16 *pa = (u16 *)((char *)(((int)o + 0x466c)) + m);
        *pa = -*pa;
        *(int *)((char *)(((int)o + 0x4664)) + m) = 0xb6000;
    } else if (yt - 8 < 2) {
        u16 *pa = (u16 *)((char *)(((int)o + 0x466c)) + m);
        *pa = -*pa;
        *(int *)((char *)(((int)o + 0x4664)) + m) = 0xa000;
    }
}


// @symbol _ZN13dScMgTeresa_c19func_ov006_0211f454Ei
void dScMgTeresa_c::func_ov006_0211f454(int i)
{
    char *c = (char *)this;
  int n = i * 0x24;
  char *pm = c + 0x4668;
  char *pa = c + 0x466c;
  int *new_var;
  short *tbl = data_02082214;
  int angle = *((unsigned short *) (pa + n));
  int s = tbl[((angle >> 4) * 2) + 1];
  int mult = *((int *) (pm + n));
  char *p0 = c + 0x4660;
  *((int *) (p0 + n)) = (*((int *) (p0 + n))) + ((int) (((((long long) (*(new_var = &s))) * mult) + 0x800) >> 12));
  {
    int angle2 = *((unsigned short *) (pa + n));
    int mult2 = *((int *) (pm + n));
    int s2 = tbl[(angle2 >> 4) * 2];
    char *p1 = c + 0x4664;
    *((int *) (p1 + n)) = (*((int *) (p1 + n))) + ((int) (((((long long) s2) * mult2) + 0x800) >> 12));
  }
}


// @symbol _ZN13dScMgTeresa_c19func_ov006_0211f51cEv
void dScMgTeresa_c::func_ov006_0211f51c()
{
    char *c = (char *)this;
    int i;
    for (i = 0; i < 0x10; i++) {
        if (*(unsigned char*)(c + 0x4677) != 0 && *(unsigned char*)(c + 0x4678) == 1) {
            *(unsigned char*)(c + 0x4678) = 2;
        }
        c += 0x24;
    }
}


// @symbol _ZN13dScMgTeresa_c19func_ov006_0211f554Ei
void dScMgTeresa_c::func_ov006_0211f554(int i)
{
    char *c = (char *)this;
    int off;
    char *e;
    int v;
    unsigned short h;
    int t;

    off = i;
    off *= 0x24;
    e = c;
    e += off;
    *(unsigned short *)(e + 0x466e) = 0;
    func_ov006_0211f454(i);
    func_ov006_0211f34c(i);
    e = c;
    e += off;
    h = *(unsigned short *)(e + 0x466c);
    t = (int)(h >> 4);
    t = t * 2 + 1;
    v = data_02082214[t];
    if (v >= 0)
        *(unsigned char *)(e + 0x467e) = 1;
    else
        *(unsigned char *)(e + 0x467e) = 0;
}


// @symbol _ZN13dScMgTeresa_c19func_ov006_0211f5d4Ei
void dScMgTeresa_c::func_ov006_0211f5d4(int idx)
{
    char *c = (char *)this;
    int off = idx * 0x24;
    char *e = c + off;
    unsigned char t;
    *(unsigned char *)(e + 0x4000 + 0x67a) = 1;
    *(short *)(e + 0x4600 + 0x6e) = 0x60;
    *(unsigned char *)(e + 0x4000 + 0x679) = 0;
    *(unsigned char *)(e + 0x4000 + 0x678) = 1;
    *(int *)(c + 0x4668 + off) = 0xc00;
    t = *(unsigned char *)(e + 0x4000 + 0x67f);
    if (t == 0)
        return;
    if (t == 8)
        *(int *)(c + 0x4668 + off) = *(int *)(c + 0x4668 + off) - 0x200;
    else
        *(int *)(c + 0x4668 + off) = *(int *)(c + 0x4668 + off) + ((t - 1) << 9);
}


// @symbol _ZN13dScMgTeresa_c19func_ov006_0211f664Ei
void dScMgTeresa_c::func_ov006_0211f664(int i)
{
    char *c = (char *)this;
  int idx = i * 0x24;
  if (*(unsigned char*)(c + 0x4678 + idx) >= 6) return;
  *(short*)(c + 0x4670 + idx) = *(unsigned short*)(c + 0x4670 + idx) + 1;
  if (*(unsigned short*)(c + 0x4670 + idx) < 0xf) return;
  *(short*)(c + 0x4670 + idx) = 0;
  *(unsigned char*)(c + 0x467d + idx) = *(unsigned char*)(c + 0x467d + idx) + 1;
  if (*(unsigned char*)(c + 0x467d + idx) >= 3)
    *(unsigned char*)(c + 0x467d + idx) = 0;
}


// @symbol _ZN13dScMgTeresa_c19func_ov006_0211f6fcEv
void dScMgTeresa_c::func_ov006_0211f6fc()
{
    int i;
    char *e = (char*)this;
    for (i = 0; i < 0x10; i++, e += 0x24) {
        if (*(unsigned char*)(e + 0x4677)) {
            (this->*data_ov006_02142ed8[*(unsigned char*)(e + 0x4678)])(i);
            func_ov006_0211f664(i);
        }
    }
}


// @symbol _ZN13dScMgTeresa_c19func_ov006_0211f77cEv
void dScMgTeresa_c::func_ov006_0211f77c()
{
    char *c = (char *)this;
    int sel;
    int count;
    unsigned short phase;
    int i;
    int valA;
    int flagA;
    int valB;
    int cval;
    int full;
    unsigned int r;

    sel = *(int *)(c + 0xbc);
    if (sel >= 0xf) {
        int k;
        r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
        k = (r << 2) >> 15;
        if (k + 0xa == *(unsigned char *)(c + 0x4c23)) {
            k += (((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 3) >> 15) + 1;
            if (k >= 4) k -= 4;
        }
        sel = k + 0xa;
    }
    *(unsigned char *)(c + 0x4c23) = sel;
    count = data_ov006_0212f014[sel];
    *(unsigned char *)(c + 0x4c25) = count;

    r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
    phase = ((r << 4) >> 15) << 12;
    r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
    valA = ((((r << 3) >> 15) << 3) + 0x60) << 12;
    r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
    valB = ((((r << 3) >> 15) << 3) + 0x60) << 12;

    i = 0;
    if (count <= 0) {
        return;
    }
    flagA = data_ov006_0212f08c[sel];
    cval = data_ov006_0212f050[sel];
    full = 0x10000;
    do {
        if (flagA == 0) {
            r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
            *(int *)(c + 0x4660) = ((((r * 0xf) >> 15) << 4) + 0x10) << 12;
            r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
            *(int *)(c + 0x4664) = ((((r * 0xb) >> 15) << 4) + 0x10) << 12;
            r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
            *(unsigned short *)(c + 0x466c) = ((r << 5) >> 15) << 11;
        } else {
            *(int *)(c + 0x4660) = valA;
            *(int *)(c + 0x4664) = valB;
            int q = full / count;
            *(unsigned short *)(c + 0x466c) = q * i + phase;
        }
        *(unsigned char *)(c + 0x467f) = cval;
        *(unsigned short *)(c + 0x466e) = 0;
        *(unsigned short *)(c + 0x4670) = 0;
        *(unsigned short *)(c + 0x4672) = 0;
        *(unsigned short *)(c + 0x4674) = 0;
        *(unsigned char *)(c + 0x4677) = 1;
        *(unsigned char *)(c + 0x4678) = 0;
        *(unsigned char *)(c + 0x4679) = 0;
        *(unsigned char *)(c + 0x467b) = 0;
        *(unsigned char *)(c + 0x467c) = 0;
        *(unsigned char *)(c + 0x467d) = 0;
        {
            int ang = *(unsigned short *)(c + 0x466c);
            if (data_02082214[(ang >> 4) * 2 + 1] >= 0) {
                *(unsigned char *)(c + 0x467e) = 1;
            } else {
                *(unsigned char *)(c + 0x467e) = 0;
            }
        }
        i++;
        c += 0x24;
    } while (i < count);
}


#pragma push
#pragma opt_common_subs off
// @symbol _ZN13dScMgTeresa_c19func_ov006_0211f9fcEv
void dScMgTeresa_c::func_ov006_0211f9fc()
{
    int self = (int)this;
    int close;

    if (((u8 *)self + 0x4000)[0xC20] == 0)
        return;
    if (((u8 *)self + 0x4000)[0xC27] != 0)
        return;

    close = 0;
    func_ov006_0211fb1c();

    if (((u8 *)self + 0x4000)[0xC1D] != 0) {
        /* Not Vector3: its C++ destructor changes this frame. */
        struct { Fix12i x, y, z; } d;
        int dist;
        d.x = *(int *)((char *)self + 0x4000 + 0xBEC) - *(int *)((char *)self + 0x4000 + 0xBF4);
        d.y = *(int *)((char *)self + 0x4000 + 0xBF0) - *(int *)((char *)self + 0x4000 + 0xBF8);
        dist = _ZN4cstd4sqrtEy((u64)((d.x) * (d.x) + (d.y) * (d.y)));
        if (dist <= 3)
            close = 1;
        if (close == 0) {
            int snd;
            int cur;
            func_ov004_020ae5c4(
                (void *)self,
                *(int *)((char *)self + 0x4000 + 0xBF4),
                *(int *)((char *)self + 0x4000 + 0xBF8),
                *(int *)((char *)self + 0x4000 + 0xBEC),
                *(int *)((char *)self + 0x4000 + 0xBF0),
                0,
                8);
            snd = func_020126e8(*(int *)((char *)self + 0x4000 + 0xBEC) << 12);
            cur = *(int *)((char *)self + 0x4000 + 0xC08);
            *(int *)((char *)self + 0x4000 + 0xC08) = func_02012468(
                cur,
                2,
                0x1F8,
                4,
                0,
                0,
                snd,
                0);
        }
    }
    if (close == 0) {
        *(int *)((char *)self + 0x4000 + 0xBF4) = *(int *)((char *)self + 0x4000 + 0xBEC);
        *(int *)((char *)self + 0x4000 + 0xBF8) = *(int *)((char *)self + 0x4000 + 0xBF0);
    }
}
#pragma pop


#define AT(p, off) ((void*)(int)((char*)(p) + (off)))
// @symbol _ZN13dScMgTeresa_c19func_ov006_0211fb1cEv
void dScMgTeresa_c::func_ov006_0211fb1c()
{
    char *c = (char *)this;
    int i = gActivePlayerSlot;
    if (gTouchHeld[(unsigned int)i * 4] != 0 && *(u8*)(c + 0x4c1d) == 0) {
        *(int*)(c + 0x4bec) = gTouchX[i * 4];
        *(int*)(c + 0x4bf0) = gTouchY[i * 4];
        *(int*)(c + 0x4bf4) = *(int*)(c + 0x4bec);
        *(int*)(c + 0x4bf8) = *(int*)(c + 0x4bf0);
        *(u8*)AT(c, 0x4c1d) += 1;
    }
    if (gTouchHeld[(unsigned int)gActivePlayerSlot * 4] == 0) {
        *(u8*)(c + 0x4c1d) = 0;
    }
    if (*(u8*)(c + 0x4c1d) == 0) return;
    *(int*)(c + 0x4bec) = gTouchX[(unsigned int)gActivePlayerSlot * 4];
    *(int*)(c + 0x4bf0) = gTouchY[(unsigned int)gActivePlayerSlot * 4];
}
#undef AT


#pragma push
#pragma opt_strength_reduction off
// @symbol _ZN13dScMgTeresa_c19func_ov006_0211fbf8Ev
void dScMgTeresa_c::func_ov006_0211fbf8()
{
    char *p = (char *)this;
  int v;
  int i;
  char *new_var;
  char *q;
  v = 0;
  q = p;
  i = 0;
  do
  {
    *((int *) (q + 0x4660)) = v;
    *((int *) (q + 0x4664)) = v;
    *((short *) (q + 0x466e)) = v;
    *((short *) (q + 0x4670)) = v;
    *((short *) (q + 0x4672)) = v;
    *((char *) (q + 0x4677)) = v;
    *((char *) (q + 0x4678)) = v;
    *((char *) (q + 0x4679)) = v;
    new_var = q;
    *((char *) (new_var + 0x467a)) = v;
    *((char *) (q + 0x467b)) = v;
    *((char *) (q + 0x467c)) = v;
    *((char *) (q + 0x467d)) = v;
    *((char *) (q + 0x467e)) = v;
    i++;
    *((char *) (q + 0x4676)) = v;
    q += 0x24;
  }
  while (i < 0x10);
  q = p;
  v = 0;
  i = 0;
  do
  {
    *((int *) (q + 0x48a0)) = i;
    *((int *) (q + 0x48a4)) = i;
    *((char *) (q + 0x48a8)) = i;
    *((char *) (q + 0x48a9)) = i;
    v++;
    *((char *) (q + 0x48aa)) = i;
    q += 0xc;
  }
  while (v < 0x10);
  v = 0;
  do
  {
    *((char *) ((p + (v << 4)) + 0x496d)) = i;
    *((char *) ((p + (v << 4)) + 0x496e)) = i;
    v++;
  }
  while (v < 0x10);
  *((int *) (p + 0x4bec)) = i;
  *((int *) (p + 0x4bf0)) = i;
  *((int *) (p + 0x4bf4)) = i;
  *((int *) (p + 0x4bf8)) = i;
  *((int *) (p + 0x4bfc)) = i;
  *((int *) (p + 0x4c00)) = i;
  *((short *) (p + 0x4c10)) = i;
  *((short *) (p + 0x4c0c)) = i;
  *((short *) (p + 0x4c0e)) = i;
  *((char *) (p + 0x4c1d)) = i;
  *((char *) (p + 0x4c1f)) = i;
  *((char *) (p + 0x4c20)) = i;
  *((short *) (p + 0x4c14)) = i;
  *((char *) (p + 0x4c1a)) = i;
  *((char *) (p + 0x4c1b)) = i;
  *((char *) (p + 0x4c1c)) = i;
  *((char *) (p + 0x4c21)) = i;
  func_ov006_0211ddb8();
  func_ov006_0211d7d8();
  *((short *) (p + 0x4c18)) = i;
  *((char *) (p + 0x4c24)) = i;
  func_ov006_0211d688();
  *((int *) (p + 0x4c08)) = i;
  *((unsigned char *) (p + 0x4c26)) = 0xff;
  *((char *) (p + 0x4c27)) = i;
}
#pragma pop


// @symbol _ZN13dScMgTeresa_c19func_ov006_0211fd44Ev
void dScMgTeresa_c::func_ov006_0211fd44()
{
    char *c = (char *)this;
    func_ov006_0211d5a8();
    func_ov006_0211f6fc();
    func_ov006_0211e184();
    if (*(unsigned short *)(c + 0x4c0c) == 0) return;
    *(unsigned short *)(((int)c + 0x4c0c)) -= 1;
    if (*(short *)(c + 0x4c0c) > 0) return;
    *(unsigned short *)(c + 0x4c0c) = 0;
    if (*(unsigned char *)(c + 0x4000 + 0xc1f) != 0) {
        struct ScoreView *p;
        func_ov004_020b0a54(0);
        p = (struct ScoreView *)data_ov004_020beb68;
        if (p != 0) {
            if (p->b4 < 0x270f) {
                *(int *)(((int)p + 0xb4)) += 1;
            }
            if (p->b4 > p->b8) {
                p->b8 = p->b4;
            }
        }
        func_ov004_020adb1c(data_ov004_020beb68 ? ((struct ScoreView *)data_ov004_020beb68)->b4 : 0);
    } else {
        if (func_ov004_020adbe0() != 0) {
            *(unsigned char *)(c + 0x4000 + 0xc1b) = 0;
        }
        func_ov004_020b0cac(8, 0x80, 0x60, 1, -1, 0xd);
        func_ov004_020b0a54(0x12);
    }
    *(unsigned char *)(c + 0xc3) = 0;
}


// @symbol _ZN13dScMgTeresa_c19func_ov006_0211fe78Ev
void dScMgTeresa_c::func_ov006_0211fe78()
{
    char *c = (char *)this;
    func_ov006_0211d5a8();
    func_ov006_0211f9fc();
    if (*(u16 *)(c + 0x4c0c) != 0) {
        {
            u16 *p = (u16 *)((int)c + 0x4c0c);
            *p = *p - 1;
        }
        if (*(s16 *)(c + 0x4c0c) > 0)
            return;
        if (*(u8 *)(c + 0x4c1f) != 0) {
            func_ov006_0211e0c8();
            *(s16 *)(c + 0x4c0e) = 0x60;
            return;
        }
        func_ov006_0211e0c8();
        func_ov006_0211e7d8();
        *(s16 *)(c + 0x4c0e) = 0x60;
        return;
    }
    func_ov006_0211dec0();
    func_ov006_0211f6fc();
    if (func_ov006_0211de7c() != 0)
        return;
    if (*(u16 *)(c + 0x4c0e) != 0) {
        func_ov006_0211d69c();
        {
            u16 *q = (u16 *)((int)c + 0x4c0e);
            *q = *q - 1;
        }
        if (*(s16 *)(c + 0x4c0e) < 0)
            *(s16 *)(c + 0x4c0e) = 0;
        return;
    }
    func_ov006_0211de54();
    *(int *)(c + 0x4be8) = 4;
    *(s16 *)(c + 0x4c0c) = 0x60;
    *(int *)0x4001000 = *(int *)0x4001000 & ~0xe000;
    data_0209d454 = data_0209d454 & ~1;
    Sound::PlayBank2_2D(0x1f7);
    func_ov006_0211cc2c();
    if (*(u8 *)(c + 0x4c1f) == 0)
        return;
    Sound::PlayBank2_2D(0x1f2);
}


// @symbol _ZN13dScMgTeresa_c19func_ov006_02120008Ev
void dScMgTeresa_c::func_ov006_02120008()
{
    char *c = (char *)this;
  unsigned short *g = (unsigned short *) (c + 0x4c00);
  int new_var;
  if (g[0xb] != 0)
  {
    new_var = ((int) c) + 0x4c16;
    *((unsigned short *) (new_var)) = (*((unsigned short *) (new_var))) - 1;
    if (((unsigned short *) (c + 0x4c00))[0xb] == 0)
    {
      FreeGfxSlotsById(0xd);
      if ((*((unsigned char *) (c + 0xc4))) == 0)
      {
        *((unsigned char *) (c + 0xc3)) = 1;
        *((unsigned char *) (c + 0xc4)) = 1;
        *((short *) (c + 0xc0)) = 0;
      }
    }
  }
  func_ov006_0211e318();
  func_ov006_0211f9fc();
  func_ov006_0211f6fc();
  func_ov006_0211e5cc();
  func_ov006_0211e3e0();
  func_ov006_0211d5a8();
  func_ov006_0211e658();
}


// @symbol _ZN13dScMgTeresa_c19func_ov006_021200a8Ev
void dScMgTeresa_c::func_ov006_021200a8()
{
    func_ov006_0211dd0c();
    func_ov006_0211d7b0();
    func_ov006_0211f6fc();
}


// @symbol _ZN13dScMgTeresa_c19func_ov006_021200ccEv
void dScMgTeresa_c::func_ov006_021200cc()
{
    char *p = (char *)this;
    *(int *)(p + 0x4be8) = 1;
}


#pragma push
#pragma opt_loop_invariants off
// @symbol _ZN13dScMgTeresa_c9Virtual88Eiiii
/* dScMgTeresa_c::Virtual88 - slot 34, a STIPPLED brush.

   Same square walk and the same 4bpp nibble arithmetic as dScMgBase_c's, with
   one difference that changes what it is for: each cell is painted only if
   `data_ov006_0213f9e4[row] & (1 << (col*4))` is set, so the table is a
   bitmapped shape and this stamps that shape rather than a solid block.

   It also never consults the layer index at +0x6c -- it always draws into sub
   BG0 -- which is why `this` arrives and is never read.  The comment in
   include/dScMgTeresa_c.h predicted both of these before the slot was
   declared. */
void dScMgTeresa_c::Virtual88(int cx, int cy, int val, int n)
{
    int half;
    int x0;
    int j;
    int x;
    int buf;
    int *addr;
    int i;
    int y;

    half = n / 2;
    j = 0;
    if (j < n) {
        y = cy - half;
        x0 = cx - half;
        do {
            for (i = 0; i < n; i++) {
                if (data_ov006_0213f9e4[j] & (1 << (i * 4))) {
                    x = i + x0;
                    if (x >= 0 && x < 0x100) {
                        if (y >= -0xc0 - data_ov004_020beb6c && y >= 0 && y < 0xc0) {
                            addr = (int *)((char *)G2S::GetBG0CharPtr() + ((x / 8) + (y / 8) * 32) * 32 + (y & 7) * 4);
                            MultiCopy_Int(addr, &buf, 4);
                            buf = (buf & (-1 ^ (0xf << ((x & 7) * 4)))) | (val << ((x & 7) * 4));
                            MultiCopy_Int(&buf, addr, 4);
                        }
                    }
                }
            }
            y++;
            j++;
        } while (j < n);
    }
}
#pragma pop


// @symbol _ZN13dScMgTeresa_c9Virtual50Ev
/* Minigame slot 20; Virtual50 is a placeholder, not an original name. The
   reconstructed void contract is documented in dScMgBase_c.h. A thunk:
   FreeGfxSlotsById(8). */
void dScMgTeresa_c::Virtual50()
{
    FreeGfxSlotsById(8);
}


// @symbol _ZN13dScMgTeresa_c13OnYoshiTryEatEi
/* Vtable slot 18, an override of dScMgBase_c::OnYoshiTryEat(int). The
   signature must repeat the base declaration exactly, or mwcc appends a slot
   instead of overriding. */
void dScMgTeresa_c::OnYoshiTryEat(int reset)
{
    char *self = (char *)this;

    volatile unsigned short val;
    if (reset == 0) {
        /* `const` on the read is load-bearing: without it mwcc CSEs the +0xbc
           field address into its own register (add r2,r4,#0xbc / ldr [r2] /
           str [r2]) and the function grows a word; the cartridge re-issues
           ldr r1,[r4,#0xbc]. Same lever as dScMgHanachan_c::OnYoshiTryEat. */
        *(unsigned int*)(self + 0xbc) = *(const unsigned int*)(self + 0xbc) + 1;
        if (*(unsigned int*)(self + 0xbc) > 0x270e)
            *(unsigned int*)(self + 0xbc) = 0x270e;
    } else {
        *(int*)(self + 0xb4) = 0;
        *(unsigned int*)(self + 0xbc) = 0;
        if (*(unsigned int*)(self + 0xbc) > 0x270e)
            *(unsigned int*)(self + 0xbc) = 0x270e;
    }
    func_ov006_0211fbf8();
    func_ov006_0211dd6c();
    func_ov006_0211d7b4();
    func_ov006_0211f77c();
    char* dst = (char *)G2S::GetBG0CharPtr();
    val = 0x1111;
    MultiStore16(val, dst, 0x6000);
    *(int*)(self + 0x4be8) = 0;
    FreeGfxSlotsById(0x1d);
    *(short*)(self + 0x4c16) = 0x20;
    func_ov004_020b0cac(0xd, 0x80, 0xa8, 1, -1, 0xd);
    void *h = LoadFile(0x101);
    DecompressLZ16(h, (void *)0x6400000);
    Deallocate(h);
}


// @symbol _ZN13dScMgTeresa_c6RenderEv
/* Vtable slot 9. */
s32 dScMgTeresa_c::Render()
{
    func_ov004_020b1e34(this, 0xe0, 0x14, 1);
    func_ov006_0211ddcc();
    func_ov006_0211e29c();
    func_ov006_0211e460();
    func_ov006_0211e118();
    func_ov006_0211e72c();
    func_ov006_0211d7ec();
    func_ov006_0211d75c();
    func_ov006_0211cca8();
    return 1;
}


// @symbol _ZN13dScMgTeresa_c8BehaviorEv
/* Vtable slot 6: runs the current state's handler from the pointer-to-member
   table data_ov006_02142eb0, indexed by unk_4be8. */
s32 dScMgTeresa_c::Behavior()
{
    (this->*data_ov006_02142eb0[unk_4be8].pmf)();
    func_ov006_0211e4e0();
    return 1;
}


// @symbol _ZN13dScMgTeresa_c13InitResourcesEv
/* Vtable slot 0. */
s32 dScMgTeresa_c::InitResources()
{
    char *self = (char *)this;
    void *f1;
    void *f2;
    void *t;
    volatile u16 fa;
    volatile u16 fb;
    volatile u16 fc;
    *((volatile u16 *) 0x400000c) &= ~3;
    *((volatile u16 *) 0x400000c) &= ~0x40;
    *((volatile u32 *) 0x4000018) = 0;
    *((volatile u16 *) 0x400000c) = ((*((volatile u16 *) 0x400000c)) & 0x43) | 0x1210;
    {
        void *p = _ZN2G212GetBG2ScrPtrEv();
        fa = 0x5300;
        MultiStore16(fa, p, 0x800);
    }
    data_0209d45c |= 8;
    *((volatile u16 *) 0x400000e) = (*((volatile u16 *) 0x400000e)) & (~3);
    *((volatile u16 *) 0x400000e) &= ~0x40;
    *((volatile u32 *) 0x400001c) = 0;
    *((volatile u16 *) 0x400000e) = ((*((volatile u16 *) 0x400000e)) & 0x43) | 0x9310;
    t = LoadFile(0x9b);
    DecompressLZ16(t, _ZN2G213GetBG2CharPtrEv());
    Deallocate(t);
    t = LoadFile(0x9c);
    GX::LoadBGPltt(t, 0x60, 0x1a0);
    Deallocate(t);
    t = LoadFile(0x9e);
    func_02056314(t, 0, 0x800);
    Deallocate(t);
    t = LoadFile(0x9d);
    func_02056314(t, 0x800, 0x800);
    Deallocate(t);
    f1 = LoadFile(0x101);
    f2 = LoadFile(0x102);
    DecompressLZ16(f1, (void *) 0x6400000);
    GX::LoadOBJPltt(f2, 0, 0x100);
    *((volatile u16 *) 0x4001008) = ((*((volatile u16 *) 0x4001008)) & 0x43) | 0x218;
    *((volatile u16 *) 0x4001008) &= ~0x40;
    *((volatile u32 *) 0x4001010) = 0;
    *((volatile u16 *) 0x4001008) &= ~3;
    {
        void *p = G2S::GetBG0ScrPtr();
        fb = 0;
        MultiStore16(fb, p, 0x800);
    }
    func_ov004_020af2f8(self, 0, 0, 0);
    {
        void *p = G2S::GetBG0CharPtr();
        fc = 0x1111;
        MultiStore16(fc, p, 0x6000);
    }
    data_0209d454 |= 4;
    *((volatile u16 *) 0x400100c) = ((*((volatile u16 *) 0x400100c)) & (~3)) | 1;
    *((volatile u16 *) 0x400100c) &= ~0x40;
    *((volatile u32 *) 0x4001018) = 0;
    *((volatile u16 *) 0x400100c) = ((*((volatile u16 *) 0x400100c)) & 0x43) | 0x408;
    t = LoadFile(0x9f);
    DecompressLZ16(t, G2S::GetBG2CharPtr());
    Deallocate(t);
    t = LoadFile(0xa0);
    GXS::LoadBGPltt(t, 0x60, 0x1a0);
    Deallocate(t);
    t = LoadFile(0xa1);
    func_02056374(t, 0, 0x800);
    Deallocate(t);
    DecompressLZ16(f1, (void *) 0x6600000);
    GXS::LoadOBJPltt(f2, 0, 0x100);
    Deallocate(f1);
    Deallocate(f2);
    func_ov004_020b04d0(0x20);
    func_ov006_0211fbf8();
    *((u8 *) (self + 0x4c23)) = 0xff;
    func_ov006_0211d7b4();
    func_ov006_0211dd6c();
    func_ov006_0211f77c();
    *((int *) (self + 0x4be8)) = 1;
    *((u16 *) (self + 0x4c16)) = 0x20;
    func_ov004_020b0cac(0xd, 0x80, 0xa8, 1, -1, 0xd);
    data_ov004_020bc880 = 0x80;
    data_ov004_020bc884 = -128;
    *((int *) (self + 0xb4)) = 0;
    return 1;
}
