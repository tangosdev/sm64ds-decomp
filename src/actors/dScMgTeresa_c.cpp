//cpp
/* Hide and Boo Seek. The player watches the Boo cross the room, the lights
 * go out, and they rub the Touch Screen to uncover every hiding Boo. The
 * first 15 rounds give three seconds; after that the clock is two. This
 * file is the low 48 functions of dScMgTeresa_c (.text
 * 0x0211cbd0..0x0211e72c): that Boo's flight, the countdown and the reveal
 * sprites.
 *
 * func_ov006_0211e72c, next in ROM, is not matched, so the 32 functions
 * above the hole stay in their own files. Nothing in the ROM names the
 * helpers, and those files call them by their func_ov006_ names.
 *
 * Functions run in ROM order, lowest address first, because of
 * `#pragma defer_codegen off` below. Do not reorder, and do not drop that
 * pragma. It makes the out-of-line destructor come out D1, D0 as in the
 * ROM (the unused D2 trails it and is deadstripped), and it is what lets
 * the push and pop brackets on func_ov006_0211d924, 0211db7c and 0211e220
 * bind. All three brackets are needed; each was measured. The destructor
 * stays out of line: inline, the key function would be InitResources,
 * which is above the gap, and this file would emit no vtable. Out of
 * line, this file emits the vtable and RTTI for the whole base chain.
 *
 * OAM::Render takes Fix12<int> by value, so that call stays mangled. The
 * two pointer-to-member receivers stay incomplete on purpose.
 *
 * Leftover: dScMgTeresa_c.h leaves 0x4660..0x4be8 as padding, so the
 * 0x20-stride pair at 0x4bac, the HUD sprites at 0x4960, and the bytes at
 * 0x4c1b and 0x4c1f are still reached through local views or raw offsets.
 * Naming them in the header is what would unblock typed access.
 */

#pragma defer_codegen off

#include "types.h"
#include "OamAttr.h"
#include "dScMgTeresa_c.h"
#include "decl_common.h"
#include "G2x.h"

/* Local views of object ranges the header does not type yet. Three of the
 * merged files each had their own `struct C` and they disagree offset for
 * offset, so they are kept apart. */

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

/* func_ov006_0211d5a8's `struct C`: the flag at +0x4be0 and the state index at
   +0x4be1, called through the PMF table data_ov006_02142f18. */
struct TeresaPmfA;
typedef void (TeresaPmfA::*PmfA)(int);
struct TeresaPmfA { char pad[0x4be0]; unsigned char g; unsigned char idx; };

/* func_ov006_0211dd0c's `struct C`: a DIFFERENT pair -- index at +0x4bba and
   flag at +0x4bbc -- driving a second PMF table, data_ov006_02142e88. */
struct TeresaPmfB;
typedef void (TeresaPmfB::*PmfB)(int);
struct TeresaPmfB { char pad[0x4bba]; unsigned char idx; char gap; unsigned char g; };

/* func_ov006_0211e0c8's `struct C`: a 0x24-stride row walk, no fields named. */
struct Row { char pad[0x24]; };
struct RowArray { struct Row rows[1]; };

/* func_ov006_0211dad0's view of the 0x20-stride element pair at +0x4bac. */
typedef struct SlotElem {
    int word0;
    short pad04;
    unsigned short cnt;
    unsigned short h08;
    short pad0a;
    unsigned char b0c;
    unsigned char pad0d;
    unsigned char b0e;
    unsigned char pad0f;
    unsigned char pad10;
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

/* Everything this file calls or reads that decl_common.h does not declare.
 * Repeating one of its declarations with other types is an error, so the
 * definitions below that it declares use its parameter types. */
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

/* The two pointer-to-member tables, each typed to its own view of the object. */
extern PmfA data_ov006_02142f18[];
extern PmfB data_ov006_02142e88[];

/* A member of this class above the gap, still in its own file. */
extern void func_ov006_0211f51c(char *c);

/* Defined below. The ones decl_common.h declares are left out. */
extern void func_ov006_0211cc2c(unsigned char *self);
extern void func_ov006_0211cc90(unsigned char *base);
extern void func_ov006_0211cd24(void *self, int idx);
extern void func_ov006_0211ce90(void);
extern void func_ov006_0211ce94(struct BooWalk *base, int index);
extern void func_ov006_0211cef4(char *c, int i);
extern void func_ov006_0211d018(char *base, int idx);
extern void func_ov006_0211d0f8(char *base, int i);
extern void func_ov006_0211d224(char *c, int i);
extern void func_ov006_0211d368(char *obj, int i);
extern void func_ov006_0211d4e8(char *c, int i);
extern void func_ov006_0211d5a8(TeresaPmfA *c);
extern void func_ov006_0211d608(char *c);
extern void func_ov006_0211d688(char *p);
extern void func_ov006_0211d69c(char *obj);
extern void func_ov006_0211d7b0(void);
extern void func_ov006_0211d7d8(char *p);
extern void func_ov006_0211d86c(char *thiz, int idx);
extern void func_ov006_0211d924(char *p, int i);
extern void func_ov006_0211dad0(S *s, int i);
extern void func_ov006_0211db7c(char *c, int i);
extern void func_ov006_0211dce0(char *base, int i);
extern void func_ov006_0211dd0c(TeresaPmfB *c);
extern void func_ov006_0211ddb8(char *p);
extern void func_ov006_0211de54(char *p);
extern int  func_ov006_0211de7c(char *c);
extern void func_ov006_0211dec0(void *arg);
extern void func_ov006_0211e020(char *c, int i);
extern void func_ov006_0211e0c8(RowArray *c);
extern void func_ov006_0211e184(char *base);
extern void func_ov006_0211e220(unsigned char *c, int param);
extern void func_ov006_0211e318(char *c);
extern void func_ov006_0211e3e0(char *c);
extern void func_ov006_0211e4e0(char *base);
extern void func_ov006_0211e55c(char *c, int idx);
extern void func_ov006_0211e5cc(char *c);
extern void func_ov006_0211e658(unsigned char *base);

}  /* extern "C" */

namespace Sound { void PlayBank2_2D(unsigned int id); }


// @symbol _ZN13dScMgTeresa_cD1Ev
// @symbol _ZN13dScMgTeresa_cD0Ev
/* One out-of-line definition; with codegen not deferred it emits D1, then
 * D0, as in the ROM. */
dScMgTeresa_c::~dScMgTeresa_c()
{
}


// @symbol func_ov006_0211cc2c
extern "C" void func_ov006_0211cc2c(unsigned char *raw) {
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


// @symbol func_ov006_0211cc90
extern "C" void func_ov006_0211cc90(unsigned char *base) {
    base += 0x4000;
    if (base[0xbe0] != 0) {
        base[0xbe5] = 1;
    }
}


// @symbol func_ov006_0211cca8
/* decl_common.h declares this one with a `void *` parameter. */
extern "C" void func_ov006_0211cca8(void *arg){
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
// @symbol func_ov006_0211cd24
extern "C" void func_ov006_0211cd24(void* raw, int idx) {
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


// @symbol func_ov006_0211ce90
extern "C" void func_ov006_0211ce90(void)
{
}


// @symbol func_ov006_0211ce94
extern "C" void func_ov006_0211ce94(struct BooWalk *base, int index) {
    void *data;
    base->boo[index].xVel = 0;
    base->boo[index].yVel = 0;
    base->boo[index].shown = 0;
    data = LoadFile(0x103);
    DecompressLZ16(data, (void *)0x6400000);
    base->boo[index].state = 7;
    Deallocate(data);
}


// @symbol func_ov006_0211cef4
extern "C" void func_ov006_0211cef4(char *c, int i)
{
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


// @symbol func_ov006_0211d018
extern "C" void func_ov006_0211d018(char *raw, int idx)
{
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


// @symbol func_ov006_0211d0f8
extern "C" void func_ov006_0211d0f8(char* raw, int i)
{
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


// @symbol func_ov006_0211d224
extern "C" void func_ov006_0211d224(char* raw, int i) {
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


// @symbol func_ov006_0211d368
extern "C" void func_ov006_0211d368(char *raw, int i)
{
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


// @symbol func_ov006_0211d4e8
extern "C" void func_ov006_0211d4e8(char* raw, int i) {
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


// @symbol func_ov006_0211d5a8
extern "C" void func_ov006_0211d5a8(TeresaPmfA *c){
  if (c->g == 0) return;
  (c->*data_ov006_02142f18[c->idx])(0);
}


// @symbol func_ov006_0211d608
extern "C" void func_ov006_0211d608(char *raw)
{
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


// @symbol func_ov006_0211d688
extern "C" void func_ov006_0211d688(char *raw)
{
    BOO(raw)->active = 0;
    BOO(raw)->shown = 0;
}


// @symbol func_ov006_0211d69c
extern "C" void func_ov006_0211d69c(char *raw)
{
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


// @symbol func_ov006_0211d75c
/* decl_common.h declares this one with a `void *` parameter. */
extern "C" void func_ov006_0211d75c(void* arg){
  char* raw = (char*)arg;
  if(*(unsigned char*)(raw + 0x4bc9)==0) return;
  RenderOamMainScreen(data_ov006_0213a5f4,
                      *(int*)(raw + 0x4bc0)>>0xc,
                      *(int*)(raw + 0x4bc4)>>0xc,
                      -1, -1);
}


// @symbol func_ov006_0211d7b0
extern "C" void func_ov006_0211d7b0(void)
{
}


// @symbol func_ov006_0211d7b4
extern "C" void func_ov006_0211d7b4(char *raw)
{
    *(int *)(raw + 0x4bc0) = 327680;
    *(int *)(raw + 0x4bc4) = 262144;
    *(char *)(raw + 0x4bc8) = 1;
    *(char *)(raw + 0x4bc9) = 1;
}


// @symbol func_ov006_0211d7d8
extern "C" void func_ov006_0211d7d8(char *raw)
{
    *(char *)(raw + 0x4bc8) = 0;
    *(char *)(raw + 0x4bc9) = 0;
}


// @symbol func_ov006_0211d7ec
/* decl_common.h declares this one with a `void *` parameter. */
extern "C" void func_ov006_0211d7ec(void *arg)
{
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


// @symbol func_ov006_0211d86c
extern "C" void func_ov006_0211d86c(char *raw, int idx)
{
    char *base = raw + (idx << 5);
    if (*(unsigned short*)(base + 0x4bb4) != 0) {
        *(unsigned short*)(raw + 0x4bb4 + (idx << 5)) =
            *(unsigned short*)(raw + 0x4bb4 + (idx << 5)) - 1;
        return;
    }
    *(unsigned char*)(base + 0x4bb9) = 0;
    *(unsigned char*)(base + 0x4bbc) = 0;
    func_ov006_0211d7d8(raw);
    data_0209d45c |= 4;
    data_0209d454 |= 1;
    Sound::PlayBank2_2D(0x1f6);
    *(int*)(raw + 0x4be8) = 2;
    func_ov006_0211f51c(raw);
    func_ov006_0211d608(raw);
    Sound::PlayBank2_2D(0x1f5);
}


#pragma push
#pragma opt_common_subs off
#define AT(p,off) ((void*)(int)((char*)(p)+(off)))
// @symbol func_ov006_0211d924
extern "C" void func_ov006_0211d924(char* raw, int i)
{
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


// @symbol func_ov006_0211dad0
extern "C" void func_ov006_0211dad0(S *s, int i) {
    s->arr[i].cnt++;
    if (s->arr[i].cnt < 8) return;
    s->arr[i].cnt = 0;
    s->arr[i].b11++;
    if (s->arr[i].b11 >= 8) {
        s->arr[i].b11 = 0;
        s->arr[i].b0c = 0;
        s->arr[i].b0e = 3;
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
// @symbol func_ov006_0211db7c
extern "C" void func_ov006_0211db7c(char *raw, int i)
{
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


// @symbol func_ov006_0211dce0
extern "C" void func_ov006_0211dce0(char *raw, int i)
{
  char *p = raw + (i << 5);
  *((raw + (i << 5)) + 0x4bba) = 1;
  *((short *) ((raw + (i << 5)) + 0x4bb4)) = 0x40;
  *((int *) (p + 0x4ba8)) = -0xb00;
}


// @symbol func_ov006_0211dd0c
extern "C" void func_ov006_0211dd0c(TeresaPmfB *c){
  if (c->g == 0) return;
  (c->*data_ov006_02142e88[c->idx])(0);
}


// @symbol func_ov006_0211dd6c
extern "C" void func_ov006_0211dd6c(char *raw)
{
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


// @symbol func_ov006_0211ddb8
extern "C" void func_ov006_0211ddb8(char *raw)
{
    *(char *)(raw + 0x4bb9) = 0;
    *(char *)(raw + 0x4bbc) = 0;
}


// @symbol func_ov006_0211ddcc
/* decl_common.h declares this one with a `void *` parameter, and
   func_ov004_020af948 as `(void *, int, int, void *)`. */
extern "C" void func_ov006_0211ddcc(void *c_){
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


// @symbol func_ov006_0211de54
extern "C" void func_ov006_0211de54(char *p)
{
    int i;
    for (i = 0; i < 0x10; i++) {
        *(unsigned char *)(p + 0x4a70) = 0;
        *(unsigned char *)(p + 0x4a71) = 0;
        p += 0x14;
    }
}


// @symbol func_ov006_0211de7c
extern "C" int func_ov006_0211de7c(char *c) {
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


// @symbol func_ov006_0211dec0
extern "C" void func_ov006_0211dec0(void *arg) {
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


// @symbol func_ov006_0211e020
extern "C" void func_ov006_0211e020(char *raw, int i)
{
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


// @symbol func_ov006_0211e0c8
/* The call to func_ov006_0211e020 is cast because that function takes
   `char *`. */
extern "C" void func_ov006_0211e0c8(RowArray* c) {
    int i;
    struct Row* r = c->rows;
    for (i = 0; i < 0x10; i++) {
        if (((unsigned char*)r + 0x4000)[0x677] != 0 && ((unsigned char*)r + 0x4000)[0x678] == 5)
            func_ov006_0211e020((char*)c, i);
        r++;
    }
}


// @symbol func_ov006_0211e118
/* decl_common.h declares this one with a `void *` parameter. */
extern "C" void func_ov006_0211e118(void* a0_) {
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
// @symbol func_ov006_0211e184
extern "C" void func_ov006_0211e184(char *raw)
{
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
// @symbol func_ov006_0211e220
extern "C" void func_ov006_0211e220(unsigned char* raw,int param){
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


// @symbol func_ov006_0211e29c
/* decl_common.h declares this one with a `void *` parameter. */
extern "C" void func_ov006_0211e29c(void* arg){
  unsigned char* raw = (unsigned char*)arg;
  if(*(unsigned char*)(raw+0x4c1b)==0) return;
  func_ov004_020b2220(0x80,0x60,*(unsigned short*)(raw+0x4c14),1,0,0x800,0);
  DrawOamSprite(data_ov006_0213f9d0[GetGameLanguage()][3],0x80,0x48,0);
}


// @symbol func_ov006_0211e318
extern "C" void func_ov006_0211e318(char *raw){
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


// @symbol func_ov006_0211e3e0
extern "C" void func_ov006_0211e3e0(char *raw)
{
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


// @symbol func_ov006_0211e460
/* decl_common.h declares this one with a `void *` parameter. */
extern "C" void func_ov006_0211e460(void *c_)
{
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
// @symbol func_ov006_0211e4e0
extern "C" void func_ov006_0211e4e0(char *base)
{
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


// @symbol func_ov006_0211e55c
extern "C" void func_ov006_0211e55c(char* raw, int idx) {
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


// @symbol func_ov006_0211e5cc
extern "C" void func_ov006_0211e5cc(char* raw)
{
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


// @symbol func_ov006_0211e658
extern "C" void func_ov006_0211e658(unsigned char* raw)
{
    if (*(unsigned short*)(raw + 0x4c14) == 0 && *(unsigned char*)(raw + 0x4c20) != 0) {
        *(unsigned char*)(raw + 0x4c1f) = 0;
        *(int*)(raw + 0x4be8) = 3;
        *(unsigned short*)(raw + 0x4c0c) = 0x60;
        func_ov006_0211cc90(raw);
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
        func_ov006_0211cc90(raw);
        *(unsigned char*)(raw + 0x4c1f) = 1;
    }
}
