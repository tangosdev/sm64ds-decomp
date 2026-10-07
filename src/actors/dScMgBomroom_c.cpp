//cpp
/* Bob-omb sorting minigame. Each bomb is one of two colors; a stylus
 * grab carries it by the offset at the touch. Its own pen settles it;
 * the wrong pen explodes it, which knocks that pen's settled bombs and the
 * loose ones back into play. When 40 bombs of one color are settled the
 * round ends, and +0x62f5 records the color.
 *
 * A bomb is sorted once dropped in its pen: x under 0x40 or over 0xc0, y
 * between 0x40 and 0x80.
 *
 * This TU is the class's whole linker unit: 80 functions, .text
 * 0x020d5a54..0x020d9574. It opens with the destructor, which the header
 * declares first and out of line, so this file is the key function and
 * emits the vtable and the RTTI chain. Then come the unnamed helpers and
 * round states, and it closes with OnYoshiTryEat, Render, Behavior and
 * InitResources. dScMgBomroom_c_classInit, the factory just above, stays
 * in src/d_s_mg_bomroom.cpp. Functions run in ROM order under
 * `#pragma defer_codegen off`; do not reorder. cstd::atan2 takes Fix12 by
 * value, so its call stays mangled.
 *
 * Each pragma bracket below carries the file-global pragma of the shard
 * that function came from; every bracket moves bytes, and they bind only
 * under defer_codegen off.
 *
 * Leftover, measured: a state field (and &state) differs from
 * (u8 *)(bomb + 0x4697) in func_ov006_020d68a8, and that function's
 * idle-bomb timer store must stay raw + i*0x40 + 0x4690. &f3c differs
 * from (unsigned char *)(int)(bomb + 0x469c) in func_ov006_020d7604.
 * func_ov006_020d7c4c keeps the sine-table value as the first factor of
 * each product (named s16 sinv/cosv); inline table reads flip the smull
 * operand order.
 */

#include "dScMgBomroom_c.h"
#include "types.h"
#include "decl_common.h"
#include "Sound.h"
#include "G2x.h"

extern "C" {
extern void Hud_RenderSprite(void* a0, int a1, int a2, int a3, int a4);
extern int data_ov006_021344ec[];
extern int data_ov006_0212e2c8[];
extern int data_ov006_0212e2e0[];
extern int data_ov006_021342bc[];
extern int data_ov006_021343b0[];
extern void func_ov006_020d5d90(char *o, int i);
extern int data_ov006_0212e2d8[];
extern int data_ov006_0212e2d0[];
extern int func_020126e8(int a);
extern int func_02012468(int a, int b, int c, int d, int e, int f, int g, short h);
extern unsigned char *data_ov006_0213bb08[];
extern unsigned char data_ov006_0213b9bc[];
extern s16 data_02082214[];
extern s16 _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
extern void func_ov004_020afdd0(void* a0, int a1, int a2, int a3, int a4);
extern unsigned char* data_ov006_0213bb18[];
extern unsigned char* data_ov006_0213bb28[];
extern void* data_ov006_0213bb4c[];
extern void func_ov006_020d5e3c(void *a);
extern void RenderOamMainScreen(int a0, int a1, int a2, int a3, int a4);
extern void *data_ov006_02134f30;
extern void *data_ov006_02133ae0[];
extern void *data_ov006_02133a70[];
extern int data_ov006_0212e2c0[];
extern int func_ov004_020adbc0(void);
extern int RandomIntInternal(int *seed);
// local extern: this file needs a record-view spelling of one of the touch lanes (the ROM scales the slot in the addressing mode), which conflicts with PlayerInput.h; the header is not included and all five symbols are declared here.
extern u8 gTouchX[];
// local extern: see above.
extern u8 gTouchY[];
extern u16 data_ov006_0212e2e8[];
extern int data_ov006_021416a0[];
extern void func_ov006_020d8904(char *p);
extern void func_ov006_020d836c(char *c);
extern int func_ov006_020d8c88(char *c);
extern int LoadFile(int handle);
extern void DecompressLZ16(void *src, void *dst);
extern void _ZN2GX10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
extern void *_ZN2G212GetBG2ScrPtrEv(void);
extern void MultiStore16(u16 val, char *dst, int nbytes);
extern void _ZN2GX11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN3GXS10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN3GXS11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern void func_02056554(const void *src, int offset, int count);
extern u8 data_0209d45c;
extern u8 data_0209d454;
}

namespace cstd { int sqrt(u64 value); }

/* One bomb: 0x40 bytes at scene + 0x4660. x, y, grabX, grabY and speed are Fix12. */
struct Bomb {
    int x;            /* 0x00 */
    int y;            /* 0x04 */
    int grabX;        /* 0x08 */
    int grabY;        /* 0x0c */
    int speed;        /* 0x10 */
    char pad14[0x10];
    int sound;        /* 0x24 */
    char pad28[4];
    u16 angle;        /* 0x2c */
    u16 f2e;          /* 0x2e */
    u16 f30;          /* 0x30 */
    s16 f32;          /* 0x32 */
    u8 type;          /* 0x34 */
    u8 frame;         /* 0x35 */
    u8 color;         /* 0x36 */
    u8 state;         /* 0x37 */
    u8 f38;           /* 0x38 */
    u8 f39;           /* 0x39 */
    u8 f3a;           /* 0x3a */
    u8 f3b;           /* 0x3b */
    u8 f3c;           /* 0x3c */
    u8 f3d;           /* 0x3d */
    u8 f3e;           /* 0x3e */
    u8 pad3f[1];
};

#pragma defer_codegen off

/* No member needs explicit destruction: the empty body is the compiler's
 * own vtable store and base-destructor call. The deleting variant reaches
 * dScMgBase_c's operator delete, its immediate base's. */
// @symbol _ZN14dScMgBomroom_cD1Ev
// @symbol _ZN14dScMgBomroom_cD0Ev
dScMgBomroom_c::~dScMgBomroom_c()
{
}

// @symbol func_ov006_020d5ab0
/* Draws the round-over sprite once +0x62fa is set. */
extern "C" {
void func_ov006_020d5ab0(void *p)
{
    char *c = (char *)p;
    if (*(unsigned char *)(c + 0x6000 + 0x2fa) == 0) return;
    RenderOamMainScreen((int)data_ov006_02134f30, 0x80, 0xc0, -1, 1);
}
}

// @symbol func_ov006_020d5b00
extern "C" {
void func_ov006_020d5b00(char *p)
{
    *(char *)(p + 0x62fa) = 1;
}
}

// @symbol func_ov006_020d5b10
/* Steps the BG2 y offset (Fix12 at +0x62dc): state 1 raises it by 3 a
 * frame to 0xc0, state 2 waits out the +0x62f2 count, then lowers it by 3
 * a frame back to 0. */
extern "C" {
void func_ov006_020d5b10(char *c)
{
    u8 state = *(u8 *)(c + 0x62f4);

    if (state == 0) {
        return;
    }

    if (state == 1) {
        s32 v;
        s32 shifted;
        *(s32 *)(c + 0x62dc) += 0x3000;
        v = *(s32 *)(c + 0x62dc);
        shifted = v >> 0xc;
        if (shifted >= 0xc0) {
            *(s32 *)(c + 0x62dc) = 0xc0000;
            *(u8 *)(c + 0x62f4) = 2;
            shifted = 0xc0;
            *(u16 *)(c + 0x62f2) = shifted;
            Sound::PlayBank2_2D(0x1de);
            func_ov006_020d8904(c);
        }
        SetBg2Offset(0, shifted);
        return;
    }

    if (state != 2) {
        return;
    }

    if (*(u16 *)(c + 0x62f2) != 0) {
        *(u16 *)(c + 0x62f2) -= 1;
        if (*(u16 *)(c + 0x62f2) != 0) {
            return;
        }
        Sound::PlayBank2_2D(0x1df);
        return;
    }

    {
        s32 v;
        s32 shifted;
        *(s32 *)(c + 0x62dc) -= 0x3000;
        v = *(s32 *)(c + 0x62dc);
        shifted = v >> 0xc;
        if (shifted <= 0) {
            shifted = 0;
            *(s32 *)(c + 0x62dc) = 0;
            *(u8 *)(c + 0x62f4) = 0;
            Sound::PlayBank2_2D(0x1de);
        }
        SetBg2Offset(0, shifted);
    }
}
}

// @symbol func_ov006_020d5c60
extern "C" {
void func_ov006_020d5c60(char *p)
{
    int zero = 0;
    *(int *)(p + 0x62dc) = zero;
    *(unsigned char *)(p + 0x62f4) = zero;
    *(unsigned short *)(p + 0x62f2) = zero;
    SetBg2Offset(zero, zero);
}
}

// @symbol func_ov006_020d5c88
/* Draws the two 0x10-byte sprite records at +0x62b0 that are active. */
extern "C" {
struct Marker_5c88 {
    int x;                  /* 0x00 */
    int y;                  /* 0x04 */
    unsigned char pad[5];   /* 0x08 */
    unsigned char active;   /* 0x0d */
    unsigned char idx;      /* 0x0e */
    unsigned char pad2;     /* 0x0f */
};

struct Scene_5c88 {
    unsigned char pad[0x62b0];
    struct Marker_5c88 subs[2];
};

void func_ov006_020d5c88(void *p)
{
    struct Scene_5c88 *b = (struct Scene_5c88 *)p;
    int i;
    for (i = 0; i < 2; i++) {
        if (b->subs[i].active != 0) {
            int xv = b->subs[i].x;
            int yv = b->subs[i].y;
            int idx = b->subs[i].idx;
            int a1 = xv >> 12;
            int a2 = yv >> 12;
            Hud_RenderSprite((i != 0) ? data_ov006_02133a70[idx]
                                      : data_ov006_02133ae0[idx],
                             a1, a2, -1, 1);
        }
    }
}
}

// @symbol func_ov006_020d5d08
#pragma push
#pragma opt_strength_reduction off
extern "C" {
void func_ov006_020d5d08(char *c)
{
    int i;
    for (i = 0; i < 2; i++) {
        char *b = c + (i << 4);
        if (*(unsigned char *)(b + 0x6000 + 0x2bc)) {
            unsigned short *h = (unsigned short *)(((int)b + 0x62b8));
            *h = *h + 1;
            if (*h >= 4) {
                unsigned char *p;
                *h = 0;
                p = (unsigned char *)(((int)b + 0x62be));
                *p = *p + 1;
                *p = *p & 3;
            }
        }
    }
}
}
#pragma pop

// @symbol func_ov006_020d5d90
extern "C" {
void func_ov006_020d5d90(char *base, int idx)
{
    char *ip = base + idx * 16;
    *(unsigned char *)(ip + 0x6000 + 0x2bc) = 1;
    *(unsigned char *)(ip + 0x6000 + 0x2bd) = 1;
    *(int *)(ip + 0x6000 + 0x2b0) = 0x80000;
    *(int *)(ip + 0x6000 + 0x2b4) = data_ov006_0212e2c0[idx] << 12;
    *(short *)(ip + 0x6200 + 0xb8) = 0;
    *(unsigned char *)(ip + 0x6000 + 0x2be) = 0;
}
}

/* The scene keeps its per-slot UI state as 0x10-stride entries at
 * +0x6260, +0x6280 and +0x62b0, two slots each. The loop helpers
 * (5dd4, 6278, 63ac, 65c8, 669c) address an entry as slot * 0x10 bytes into
 * scene storage and do not model it as a 16-byte array, which they would
 * index far past its end. That byte-base form matches only with strength
 * reduction off, so each of the five carries its own bracket. The PMF
 * receiver classes stay incomplete. */
// @symbol func_ov006_020d5dd4
#pragma push
#pragma opt_strength_reduction off
extern "C" {
void func_ov006_020d5dd4(char* sceneBytes)
{
    int slot;
    for (slot = 0; slot < 2; slot++) {
        char* slotBase = sceneBytes + slot * 0x10;
        slotBase[0x62bc] = 0;
        slotBase[0x62bd] = 0;
    }
}
}
#pragma pop

// @symbol func_ov006_020d5dfc
extern "C" {
void func_ov006_020d5dfc(void)
{
    func_ov004_020b1a5c(func_ov004_020adbc0(), 4);
}
}

// @symbol func_ov006_020d5e1c
extern "C" {
void func_ov006_020d5e1c(void *a)
{
    *(((unsigned char *)a) + 0x62ac) = 3;
    Sound::PlayBank2_2D(0x1e3);
}
}

// @symbol func_ov006_020d5e3c
extern "C" {
void func_ov006_020d5e3c(void *a)
{
    *(((unsigned char *)a) + 0x62ac) = 1;
    Sound::PlayBank2_2D(0x1e2);
}
}

// @symbol func_ov006_020d5e5c
extern "C" {
void func_ov006_020d5e5c(char *c)
{
    c += 0x6000;
    if (*(unsigned char *)(c + 0x2af) == 0) return;
    RenderOamMainScreen(data_ov006_021343b0[*(unsigned char *)(c + 0x2ae)],
                        *(int *)(c + 0x2a0) >> 0xc, *(int *)(c + 0x2a4) >> 0xc, -1, 2);
}
}

// @symbol func_ov006_020d5eb8
extern "C" {
void func_ov006_020d5eb8(unsigned char* raw, int index) {
    unsigned char* rec = raw + (index << 4);
    int off = index << 4;
    if (*(unsigned char*)(rec + 0x62ae) != 0) {
        *(unsigned short*)(raw + 0x62a8 + off) =
            *(unsigned short*)(raw + 0x62a8 + off) + 1;
        if (*(unsigned short*)(rec + 0x62a8) < 3) return;
        *(unsigned short*)(rec + 0x62a8) = 0;
        *(unsigned char*)(raw + 0x62ae + off) =
            *(unsigned char*)(raw + 0x62ae + off) - 1;
    } else {
        *(unsigned char*)(rec + 0x62ac) = 0;
    }
}
}

// @symbol func_ov006_020d5f28
extern "C" {
void func_ov006_020d5f28(void)
{
}
}

// @symbol func_ov006_020d5f2c
extern "C" {
void func_ov006_020d5f2c(char *raw, int index)
{
  char *rec;
  char *hi;
  unsigned char count;
  rec = raw + (index * 0x10);
  if (((unsigned char) (*((unsigned char *) ((rec + 0x6000) + 0x2ae)))) >= 4)
  {
    *((unsigned char *) ((rec + 0x6000) + 0x2ae)) = 4;
    *((unsigned char *) ((rec + 0x6000) + 0x2ac)) = 2;
    return;
  }
  {
    unsigned short *p = (unsigned short *) ((raw + 0x62a8) + (index * 0x10));
    *p = (*p) + 1;
  }
  if ((*((unsigned short *) (rec + 0x62a8))) < 3)
  {
    return;
  }
  *((unsigned short *) (rec + 0x62a8)) = 0;
  hi = rec + 0x6000;
  {
    unsigned char *q = (unsigned char *) ((raw + 0x62ae) + (index * 0x10));
    *q = (*q) + 1;
  }
  count = (unsigned char) (*((unsigned char *) (hi + 0x2ae)));
  if (count >= 4)
  {
    *((unsigned char *) ((rec + 0x6000) + 0x2ae)) = 4;
    *((unsigned char *) (hi + 0x2ac)) = 2;
  }
}
}

// @symbol func_ov006_020d5fd8
extern "C" {
typedef struct { char pad[0xa8]; short f; } BrEnt5fd8;
void func_ov006_020d5fd8(int raw, int index){
  BrEnt5fd8 *rec = (BrEnt5fd8*)(raw + (index<<4) + 0x6200);
  rec->f = 0;
}
}

// @symbol func_ov006_020d5fec
extern "C" {
struct C_5fec; typedef void (C_5fec::*PMF_5fec)(int);
struct Entry_5fec { PMF_5fec pmf; };
extern "C" Entry_5fec data_ov006_02141660[];
struct C_5fec { char pad[0x62ac]; unsigned char idx; unsigned char guard; };
extern "C" void func_ov006_020d5fec(C_5fec* scene) {
    if (!scene->guard) return;
    (scene->*(data_ov006_02141660[scene->idx].pmf))(0);
}
}

// @symbol func_ov006_020d604c
extern "C" {
struct S_604c{char b[0x10000];};
void func_ov006_020d604c(void *raw){
  struct S_604c *view = (struct S_604c *)raw;
  *(unsigned char*)((char*)view+0x62ad)=1;
  *(unsigned char*)((char*)view+0x62ac)=0;
  *(unsigned char*)((char*)view+0x62ae)=0;
  *(unsigned char*)((char*)view+0x62af)=1;
  *(short*)((char*)view+0x62a8)=0;
  *(int*)((char*)view+0x62a0)=0x80000;
  *(int*)((char*)view+0x62a4)=0x8000;
}
}

// @symbol func_ov006_020d6084
extern "C" {
extern "C" void func_ov006_020d6084(char *raw)
{
    *(char *)(raw + 0x62ad) = 0;
    *(char *)(raw + 0x62af) = 0;
}
}

// @symbol func_ov006_020d6098
extern "C" {
typedef struct {
    int x;              /* 0x00 */
    int y;              /* 0x04 */
    char pad[7];
    unsigned char idx;  /* 0x0f */
} Rec_6098;
typedef struct {
    char head[0x6280];
    Rec_6098 recs[2];
} Obj_6098;
void func_ov006_020d6098(void *p) {
    Obj_6098 *a0 = (Obj_6098 *)p;
    int i;
    for (i = 0; i < 2; i++) {
        int xv = a0->recs[i].x >> 12;
        int yv = a0->recs[i].y >> 12;
        int k = a0->recs[i].idx;
        if (i != 0) k += 5;
        Hud_RenderSprite((void*)data_ov006_021344ec[k], xv, yv, -1, -1);
    }
}
}

// @symbol func_ov006_020d6100
extern "C" {
void func_ov006_020d6100(unsigned char* raw, int index) {
    unsigned char* rec = raw + (index << 4);
    int off = index << 4;
    if (*(unsigned char*)(rec + 0x628f) != 0) {
        *(unsigned short*)(raw + 0x6288 + off) =
            *(unsigned short*)(raw + 0x6288 + off) + 1;
        if (*(unsigned short*)(rec + 0x6288) < 4) return;
        *(unsigned short*)(rec + 0x6288) = 0;
        *(unsigned char*)(raw + 0x628f + off) =
            *(unsigned char*)(raw + 0x628f + off) - 1;
    } else {
        *(unsigned char*)(rec + 0x628c) = 0;
    }
}
}

// @symbol func_ov006_020d6170
extern "C" {
void func_ov006_020d6170(char* raw, int index)
{
    char* rec = raw + (index << 4);
    unsigned short* timer = (unsigned short*)(rec + 0x6288);

    if (*timer != 0) {
        unsigned short* timer2 = (unsigned short*)(raw + 0x6288 + (index << 4));
        *timer2 = (unsigned short)(*timer2 - 1);
        if (*(short*)(rec + 0x6288) < 0)
            *(short*)(rec + 0x6288) = 0;
        return;
    }

    *(unsigned char*)(rec + 0x628c) = 3;
}
}

// @symbol func_ov006_020d61dc
extern "C" {
typedef struct {
    char _pad0[8];
    u16 timer;   /* +0x08 */
    char _pad1[2];
    u8 state;    /* +0x0c */
    char _pad2[2];
    u8 level;    /* +0x0f */
} Slot_61dc; /* 0x10 */
typedef struct {
    char _pad0[0x6280];
    Slot_61dc slots[16];
} Work_61dc;
void func_ov006_020d61dc(char* raw, int index)
{
    Work_61dc* view = (Work_61dc*)raw;
    view->slots[index].timer++;
    if (view->slots[index].timer < 5)
        return;
    view->slots[index].timer = 0;
    view->slots[index].level++;
    if (view->slots[index].level >= 4) {
        view->slots[index].state = 2;
        view->slots[index].timer = 0x20;
    }
}
}

// @symbol func_ov006_020d6264
extern "C" {
typedef struct { char pad[0x88]; short f; } BrEnt6264;
void func_ov006_020d6264(int raw, int index){
  BrEnt6264 *rec = (BrEnt6264*)(raw + (index<<4) + 0x6200);
  rec->f = 0;
}
}

// @symbol func_ov006_020d6278
#pragma push
#pragma opt_strength_reduction off
extern "C" {
class C_6278;
typedef void (C_6278::*PMF_6278)(int);
extern "C" PMF_6278 data_ov006_021416c0[];
extern "C" void func_ov006_020d6278(C_6278 *self)
{
    u8* sceneBytes = (u8*)self;
    for (int slot = 0; slot < 2; slot++) {
        u8* slotBase = sceneBytes + slot * 0x10;
        if (slotBase[0x628d]) {
            u8 state = slotBase[0x628c];
            (self->*data_ov006_021416c0[state])(slot);
        }
    }
}
}
#pragma pop

// @symbol func_ov006_020d62e0
#pragma push
#pragma opt_strength_reduction off
extern "C" {
void func_ov006_020d62e0(void *p) {
    char *c = (char *)p;
    int i;
    for (i = 0; i < 2; i++) {
        char *p = c + i * 0x10;
        *(unsigned char *)(p + 0x628d) = 1;
        *(unsigned char *)(p + 0x628c) = 0;
        *(unsigned char *)(p + 0x628e) = 1;
        *(int *)(p + 0x6280) = data_ov006_0212e2c8[i] << 0xc;
        *(int *)(p + 0x6284) = data_ov006_0212e2e0[i] << 0xc;
        *(unsigned char *)(p + 0x628f) = 0;
        *(short *)(p + 0x6288) = 0;
    }
}
}
#pragma pop

// @symbol func_ov006_020d634c
extern "C" {
void func_ov006_020d634c(char *raw, int index)
{
    char *rec = raw + index * 16;
    unsigned char st = *(unsigned char *)(rec + 0x628c);
    if (st != 0) {
        if (st == 2) {
            *(short *)(rec + 0x6288) = 0x20;
        } else {
            *(unsigned char *)(rec + 0x628c) = 1;
        }
        return;
    }
    *(unsigned char *)(rec + 0x628c) = 1;
    SetBg0Offset(0x100, 0);
}
}

// @symbol func_ov006_020d63ac
#pragma push
#pragma opt_strength_reduction off
extern "C" {
void func_ov006_020d63ac(char* sceneBytes)
{
    int slot;
    for (slot = 0; slot < 2; slot++) {
        char* slotBase = sceneBytes + slot * 0x10;
        slotBase[0x628d] = 0;
        slotBase[0x628e] = 0;
    }
}
}
#pragma pop

// @symbol func_ov006_020d63d4
extern "C" {
typedef struct {
    int x;              /* 0x00 */
    int y;              /* 0x04 */
    char pad2[6];
    unsigned char flag; /* 0x0e */
    unsigned char idx;  /* 0x0f */
} Rec_63d4;
typedef struct {
    char head[0x6260];
    Rec_63d4 recs[2];
} Obj_63d4;
void func_ov006_020d63d4(void *p) {
    Obj_63d4 *a0 = (Obj_63d4 *)p;
    int i;
    for (i = 0; i < 2; i++) {
        if (a0->recs[i].flag != 0) {
            int xv = a0->recs[i].x >> 12;
            int yv = a0->recs[i].y >> 12;
            int k = a0->recs[i].idx;
            int v = (i == 0) ? data_ov006_021343b0[k] : data_ov006_021342bc[k];
            Hud_RenderSprite((void*)v, xv, yv, -1, 1);
        }
    }
}
}

// @symbol func_ov006_020d6454
extern "C" {
void func_ov006_020d6454(unsigned char* raw, int index) {
    unsigned char* rec = raw + (index << 4);
    if (*(unsigned char*)(rec + 0x626f) != 0) {
        unsigned short* timer = (unsigned short*)(raw + 0x6268 + (index << 4));
        *timer = *timer + 1;
        if (*(unsigned short*)(rec + 0x6268) < 3) return;
        *(unsigned short*)(rec + 0x6268) = 0;
        *(unsigned char*)(raw + 0x626f + (index << 4)) =
            *(unsigned char*)(raw + 0x626f + (index << 4)) - 1;
    } else {
        *(unsigned char*)(rec + 0x626c) = 0;
    }
}
}

// @symbol func_ov006_020d64c4
extern "C" {
void func_ov006_020d64c4(void)
{
}
}

// @symbol func_ov006_020d64c8
#pragma push
#pragma opt_common_subs off
#pragma opt_strength_reduction off
extern "C" {
void func_ov006_020d64c8(char *raw, int index)
{
    *(u16 *)(raw + 0x6268 + index * 16) += 1;
    if (*(u16 *)(raw + index * 16 + 0x6268) < 5)
        return;
    *(u16 *)(raw + index * 16 + 0x6268) = 0;
    *(u8 *)(raw + 0x626f + index * 16) += 1;
    if (index == 0) {
        u8 level = *(u8 *)(raw + index * 16 + 0x626f);
        if (level == 2 || level == 4) {
            int *offsetY = (int *)(raw + 0x62b4);
            *offsetY += 0x8000;
        }
    }
    if (*(u8 *)(raw + index * 16 + 0x626f) == 3 && index == 1)
        func_ov006_020d5d90(raw, index);
    if (*(u8 *)(raw + index * 16 + 0x626f) >= 4)
        *(u8 *)(raw + index * 16 + 0x626c) = 2;
}
}
#pragma pop

// @symbol func_ov006_020d65b4
extern "C" {
typedef struct { char pad[0x68]; short f; } BrEnt65b4;
void func_ov006_020d65b4(int raw, int index){
  BrEnt65b4 *rec = (BrEnt65b4*)(raw + (index<<4) + 0x6200);
  rec->f = 0;
}
}

// @symbol func_ov006_020d65c8
#pragma push
#pragma opt_strength_reduction off
extern "C" {
class C_65c8;
typedef void (C_65c8::*PMF_65c8)(int);
extern "C" PMF_65c8 data_ov006_02141680[];
extern "C" void func_ov006_020d65c8(C_65c8 *self)
{
    u8* sceneBytes = (u8*)self;
    for (int slot = 0; slot < 2; slot++) {
        u8* slotBase = sceneBytes + slot * 0x10;
        if (slotBase[0x626d]) {
            u8 state = slotBase[0x626c];
            (self->*data_ov006_02141680[state])(slot);
        }
    }
}
}
#pragma pop

// @symbol func_ov006_020d6630
#pragma push
#pragma opt_strength_reduction off
extern "C" {
void func_ov006_020d6630(void *p) {
    char *c = (char *)p;
    int i;
    for (i = 0; i < 2; i++) {
        *(unsigned char *)(c + i * 0x10 + 0x626d) = 1;
        *(unsigned char *)(c + i * 0x10 + 0x626c) = 0;
        *(int *)(c + i * 0x10 + 0x6260) = data_ov006_0212e2d8[i] << 0xc;
        *(int *)(c + i * 0x10 + 0x6264) = data_ov006_0212e2d0[i] << 0xc;
        *(unsigned char *)(c + i * 0x10 + 0x626f) = 0;
        *(short *)(c + i * 0x10 + 0x6268) = 0;
        *(unsigned char *)(c + i * 0x10 + 0x626e) = 1;
    }
}
}
#pragma pop

// @symbol func_ov006_020d669c
#pragma push
#pragma opt_strength_reduction off
extern "C" {
void func_ov006_020d669c(char* sceneBytes)
{
    int slot;
    for (slot = 0; slot < 2; slot++) {
        char* slotBase = sceneBytes + slot * 0x10;
        slotBase[0x626d] = 0;
        slotBase[0x626e] = 0;
    }
}
}
#pragma pop

// @symbol func_ov006_020d66c4
extern "C" {
extern "C" void func_ov006_020d66c4(char *raw, int index) {
    char *rec = raw + index * 16;
    if (*(unsigned char*)(rec + 0x626c) != 0) return;
    *(unsigned char*)(rec + 0x626c) = 1;
    Sound::PlayBank2_2D(0x1e2);
    if (index != 0) return;
    func_ov006_020d5d90(raw, index);
}
}

// @symbol func_ov006_020d672c
extern "C" {
typedef struct { char pad[0x2f9]; unsigned char on; } BrFlag672c;
typedef struct { char pad[0xee]; unsigned short v; } BrEnt672c;
void func_ov006_020d672c(void *raw){
  int addr = (int)raw;
  if(((BrFlag672c*)(addr+0x6000))->on==0) return;
  func_ov004_020b2444(0x80,0xc,((BrEnt672c*)(addr+0x6200))->v,1,-1,0,0);
}
}

// @symbol func_ov006_020d6784
#pragma push
#pragma opt_strength_reduction off
extern "C" {
void func_ov006_020d6784(char *raw)
{
    int plain, colored, i;
    colored = 0;
    plain = 0;
    for (i = 0; i < 0x70; i++) {
        if (*(unsigned char *)(raw + (i << 6) + 0x4698) == 0) continue;
        if (*(unsigned char *)(raw + (i << 6) + 0x4697) != 5) continue;
        if (*(unsigned char *)(raw + (i << 6) + 0x4696) != 0) colored++;
        else plain++;
    }
    if (plain >= 0x28) {
        *(int *)(raw + 0x62d0) = 3;
        *(int *)(raw + 0x62d4) = 0;
        *(unsigned char *)(raw + 0x62f8) = 0;
        *(unsigned char *)(raw + 0x62f5) = 0;
        *(volatile unsigned int *)(raw + 0xbc) = *(unsigned int *)(raw + 0xbc) + 1;
        if (*(unsigned int *)(raw + 0xbc) > 0x270e) *(unsigned int *)(raw + 0xbc) = 0x270e;
        *(unsigned char *)(raw + 0x62fb) = 1;
        Sound::PlayBank2_2D(0x1e6);
    } else if (colored >= 0x28) {
        *(int *)(raw + 0x62d0) = 3;
        *(int *)(raw + 0x62d4) = 0;
        *(unsigned char *)(raw + 0x62f8) = 0;
        *(unsigned char *)(raw + 0x62f5) = 1;
        *(volatile unsigned int *)(raw + 0xbc) = *(unsigned int *)(raw + 0xbc) + 1;
        if (*(unsigned int *)(raw + 0xbc) > 0x270e) *(unsigned int *)(raw + 0xbc) = 0x270e;
        *(unsigned char *)(raw + 0x62fb) = 1;
        Sound::PlayBank2_2D(0x1e6);
    }
}
}
#pragma pop

// @symbol func_ov006_020d68a8
/* Bomb `picked` has gone off. Pen byte +0x3a of that bomb is 0 when it
 * was loose: every other live bomb not yet sorted (state 5) is knocked into
 * state 3 for 0x20 frames. Otherwise only the bombs of that pen's color and
 * the idle ones are. The first blast also records its pen at 0x62f8 and sets
 * the scene state at 0x62d0 to 3. */
#pragma push
#pragma opt_strength_reduction off
#pragma opt_common_subs off
extern "C" {
void func_ov006_020d68a8(char *raw, int picked)
{
    int i;
    u8 *cur = (u8 *)(((int)(raw + picked * 0x40) + 0x469a));
    for (i = 0; i < 0x70; i++) {
        char *bomb = raw + i * 0x40;
        if (((Bomb *)(bomb + 0x4660))->f38 != 0) {
            if (i != picked) {
                u8 pen = *cur;
                if (pen == 0) {
                    u8 *state = (u8 *)(bomb + 0x4697);
                    if (*state != 5) {
                        *state = 3;
                        ((Bomb *)(bomb + 0x4660))->f30 = 0x20;
                    }
                } else {
                    u8 *state = (u8 *)(bomb + 0x4697);
                    if (*state == 5) {
                        if (((Bomb *)(bomb + 0x4660))->color + 1 == pen) {
                            *state = 3;
                            ((Bomb *)(bomb + 0x4660))->f30 = 0x20;
                        }
                    }
                    if (*state <= 1) {
                        *state = 3;
                        *(u16 *)(raw + i * 0x40 + 0x4690) = 0x20;
                    }
                }
            }
        }
    }
    if (*(u8 *)(raw + 0x62f8) != 0)
        return;
    *(u8 *)(raw + 0x62f8) = *(u8 *)(raw + picked * 0x40 + 0x469a) + 1;
    *(u16 *)(raw + 0x62e8) = 0x60;
    *(int *)(raw + 0x62d0) = 3;
    *(int *)(raw + 0x62d4) = 0;
}
}
#pragma pop

// @symbol func_ov006_020d69b8
/* Steps one bomb's animation: the frame advances when the counter at +0x2e
 * reaches the per-type frame time, playing its sound effects, and
 * wraps (or holds, for type 3) at the per-type frame count. */
extern "C" {

struct Scene_69b8 {
    char pad0[0x2d0];
    int f2d0;
};
struct Bomb_69b8 {
    int f660;
    char pad1[0x20];
    int sound;
    char pad2[6];
    unsigned short f68e;
    char pad3[4];
    unsigned char f694;
    unsigned char f695;
    unsigned char f696;
    unsigned char f697;
    unsigned char f698;
    unsigned char f699;
    char pad4[6];
};
#define BOMB (((struct Bomb_69b8 *)(raw + 0x4660))[index])
#define SCENE ((struct Scene_69b8 *)(raw + 0x6000))
void func_ov006_020d69b8(char *raw, int index)
{
    int type;
    int frame;
    int max;

    *(unsigned short *)(raw + 0x468e + (index << 6)) += 1;
    type = BOMB.f694;
    frame = BOMB.f695;

    if (type != 3 && type != 0 && SCENE->f2d0 != 3) {
        BOMB.sound = func_02012468(BOMB.sound, 2, 0x1db, 4, 0, 0,
                               func_020126e8(BOMB.f660), 0);
    }

    if (BOMB.f68e >= data_ov006_0213bb08[type][frame]) {
        frame++;
        BOMB.f695 = frame;
        BOMB.f68e = 0;
        if (type != 3 && BOMB.f697 != 2 && (type != 0 || BOMB.f697 != 5)) {
            if (BOMB.f698 == 1 && SCENE->f2d0 != 3) {
                if (BOMB.f696 != 0) {
                    func_02012718((int)0x1d9, BOMB.f660);
                } else {
                    func_02012718((int)0x1da, BOMB.f660);
                }
            }
        }
        if (type == 3 && frame == 1) {
            func_02012718((int)0x1e1, BOMB.f660);
        }
    }

    max = data_ov006_0213b9bc[type];
    if (frame < max) {
        return;
    }
    if (type == 3) {
        BOMB.f698 = 0;
        BOMB.f699 = 0;
    } else {
        max = 0;
    }
    BOMB.f695 = max;
}
}

// @symbol func_ov006_020d6b88
extern "C" {

void func_ov006_020d6b88(char *raw, int index)
{
    char *bomb = raw + index * 0x40;
    u8 color = ((Bomb *)(bomb + 0x4660))->color;
    int x = ((Bomb *)(bomb + 0x4660))->x >> 0xc;
    int y = ((Bomb *)(bomb + 0x4660))->y >> 0xc;
    if (color != 0) {
        if (x <= 0xc0) return;
        if (y <= 0x40) return;
        if (y >= 0x80) return;
        ((Bomb *)(bomb + 0x4660))->state = 5;
        ((Bomb *)(bomb + 0x4660))->type = 0;
        ((Bomb *)(bomb + 0x4660))->frame = 0;
        ((Bomb *)(bomb + 0x4660))->f2e = 0;
        ((Bomb *)(bomb + 0x4660))->speed = 0x999;
        func_02012718((int)0x1dc, ((Bomb *)(bomb + 0x4660))->x);
    } else {
        if (x >= 0x40) return;
        if (y <= 0x40) return;
        if (y >= 0x80) return;
        ((Bomb *)(bomb + 0x4660))->state = 5;
        ((Bomb *)(bomb + 0x4660))->type = 0;
        ((Bomb *)(bomb + 0x4660))->frame = 0;
        ((Bomb *)(bomb + 0x4660))->f2e = 0;
        ((Bomb *)(bomb + 0x4660))->speed = 0x999;
        func_02012718((int)0x1dc, ((Bomb *)(bomb + 0x4660))->x);
    }
}
}

// @symbol func_ov006_020d6c90
extern "C" {
void func_ov006_020d6c90(char *raw, int index)
{
    char *bomb = raw + (index << 6);
    unsigned char color = ((Bomb *)(bomb + 0x4660))->color;
    int x = ((Bomb *)(bomb + 0x4660))->x >> 12;
    int y = ((Bomb *)(bomb + 0x4660))->y >> 12;

    if (color != 0) {
        if (x >= 0x40)
            return;
        if (y <= 0x40)
            return;
        if (y >= 0x80)
            return;
        ((Bomb *)(bomb + 0x4660))->f32 = 0;
        ((Bomb *)(bomb + 0x4660))->state = 4;
        ((Bomb *)(bomb + 0x4660))->type = 3;
        ((Bomb *)(bomb + 0x4660))->frame = 0;
        ((Bomb *)(bomb + 0x4660))->f2e = 0;
        ((Bomb *)(bomb + 0x4660))->f3a = 1;
        ((Bomb *)(bomb + 0x4660))->f3d = 1;
        func_ov006_020d68a8(raw, index);
    } else {
        if (x <= 0xc0)
            return;
        if (y <= 0x40)
            return;
        if (y >= 0x80)
            return;
        ((Bomb *)(bomb + 0x4660))->f32 = 0;
        ((Bomb *)(bomb + 0x4660))->state = 4;
        ((Bomb *)(bomb + 0x4660))->type = 3;
        ((Bomb *)(bomb + 0x4660))->frame = 0;
        ((Bomb *)(bomb + 0x4660))->f2e = 0;
        ((Bomb *)(bomb + 0x4660))->f3a = 2;
        ((Bomb *)(bomb + 0x4660))->f3d = 1;
        func_ov006_020d68a8(raw, index);
    }
}
}

// @symbol func_ov006_020d6d7c
/* Picks up a bomb under the stylus: a touch within 12 units across and 15
 * down of it, while nothing is held (0x62f6 is 0xff). grabX/grabY keep the
 * stylus-minus-bomb offset. */
#pragma push
#pragma opt_propagation off
extern "C" {

typedef struct { u8 f0, f1, f2, f3; } Tab;
// local extern: see above.
extern u8 gActivePlayerSlot;
// local extern: see above.
extern Tab gTouchHeld[];

void func_ov006_020d6d7c(char *raw, int index) {
    u8 touch = gActivePlayerSlot;
    int touching = 0;
    char *bomb;
    int dx, dy;
    if (gTouchHeld[touch].f0 != 0) {
        if (gTouchHeld[touch].f1 != 0) touching = 1;
    }
    if (touching == 0) return;
    if (*(u8*)(raw + 0x62f6) != 0xff) return;
    bomb = raw + index * 0x40;
    dx = gTouchHeld[touch].f2 - (((Bomb *)(bomb + 0x4660))->x >> 0xc);
    dy = gTouchHeld[touch].f3 - (((Bomb *)(bomb + 0x4660))->y >> 0xc);
    if (dx > 0xc) return;
    if (dx < -0xc) return;
    if (dy > 0xf) return;
    if (dy < -0xf) return;
    *(u8*)(raw + 0x62f6) = ((Bomb *)(bomb + 0x4660))->color;
    ((Bomb *)(bomb + 0x4660))->type = 1;
    ((Bomb *)(bomb + 0x4660))->state = 2;
    ((Bomb *)(bomb + 0x4660))->grabX = dx << 0xc;
    ((Bomb *)(bomb + 0x4660))->grabY = dy << 0xc;
    ((Bomb *)(bomb + 0x4660))->f3e = 0;
    func_02012718((int)0x1d2, ((Bomb *)(bomb + 0x4660))->x);
}
}
#pragma pop

// @symbol func_ov006_020d6e8c
#pragma push
#pragma opt_common_subs off
extern "C" {
void func_ov006_020d6e8c(char* raw, int index)
{
    int x, y, py, dx, dy1, ang, i2, a, dy2, sy, px;

    x = ((Bomb *)(raw + 0x4660 + (index<<6)))->x >> 12;
    y = ((Bomb *)(raw + 0x4660 + (index<<6)))->y >> 12;

    if (x + 12 > 0x100) {
        ((Bomb *)(raw + 0x4660 + (index<<6)))->angle = 0x8000 - ((Bomb *)(raw + 0x4660 + (index<<6)))->angle;
        ((Bomb *)(raw + 0x4660 + (index<<6)))->x = 0xF4000;
    } else if (x - 12 < 0) {
        ((Bomb *)(raw + 0x4660 + (index<<6)))->angle = 0x8000 - ((Bomb *)(raw + 0x4660 + (index<<6)))->angle;
        ((Bomb *)(raw + 0x4660 + (index<<6)))->x = 0xC000;
    }

    if (y + 12 > 0xB8) {
        ((Bomb *)(raw + 0x4660 + (index<<6)))->angle = 0 - ((Bomb *)(raw + 0x4660 + (index<<6)))->angle;
        ((Bomb *)(raw + 0x4660 + (index<<6)))->y = 0xAC000;
    } else if (y - 12 < 0) {
        ((Bomb *)(raw + 0x4660 + (index<<6)))->angle = 0 - ((Bomb *)(raw + 0x4660 + (index<<6)))->angle;
        ((Bomb *)(raw + 0x4660 + (index<<6)))->y = 0xC000;
    }

    px = ((Bomb *)(raw + 0x4660 + (index<<6)))->x >> 12;
    py = ((Bomb *)(raw + 0x4660 + (index<<6)))->y >> 12;

    if (px + 12 > 0xC0 && py + 12 > 0x40 && py - 12 < 0x80) {
        dx = px - 0xC0;
        dy1 = py - 0x40;
        dy2 = 0x80 - py;
        if (px <= 0xC0 && py >= 0x40 && py <= 0x80) {
            ((Bomb *)(raw + 0x4660 + (index<<6)))->angle = 0x8000 - ((Bomb *)(raw + 0x4660 + (index<<6)))->angle;
            ((Bomb *)(raw + 0x4660 + (index<<6)))->x = 0xB4000;
        } else if (px > 0xC0 && py < 0x40) {
            ((Bomb *)(raw + 0x4660 + (index<<6)))->angle = 0 - ((Bomb *)(raw + 0x4660 + (index<<6)))->angle;
            ((Bomb *)(raw + 0x4660 + (index<<6)))->y = 0x34000;
        } else if (px > 0xC0 && py > 0x80) {
            ((Bomb *)(raw + 0x4660 + (index<<6)))->angle = 0 - ((Bomb *)(raw + 0x4660 + (index<<6)))->angle;
            ((Bomb *)(raw + 0x4660 + (index<<6)))->y = 0x8C000;
        } else if (px > 0xC0 && py > 0x40 && py < 0x80) {
            if (dy1 < dy2) {
                if (dx < dy1) {
                    ((Bomb *)(raw + 0x4660 + (index<<6)))->angle = 0x8000 - ((Bomb *)(raw + 0x4660 + (index<<6)))->angle;
                    ((Bomb *)(raw + 0x4660 + (index<<6)))->x = 0xB4000;
                } else {
                    ((Bomb *)(raw + 0x4660 + (index<<6)))->angle = 0 - ((Bomb *)(raw + 0x4660 + (index<<6)))->angle;
                    ((Bomb *)(raw + 0x4660 + (index<<6)))->y = 0x34000;
                }
            } else {
                if (dx < dy2) {
                    ((Bomb *)(raw + 0x4660 + (index<<6)))->angle = 0x8000 - ((Bomb *)(raw + 0x4660 + (index<<6)))->angle;
                    ((Bomb *)(raw + 0x4660 + (index<<6)))->x = 0xB4000;
                } else {
                    ((Bomb *)(raw + 0x4660 + (index<<6)))->angle = 0 - ((Bomb *)(raw + 0x4660 + (index<<6)))->angle;
                    ((Bomb *)(raw + 0x4660 + (index<<6)))->y = 0x8C000;
                }
            }
        } else {
            if (py >= 0x60) {
                sy = 0xC0 - px;
            } else {
                dy2 = 0x40 - py;
                sy = 0xC0 - px;
            }
            cstd::sqrt((s64)(sy * sy + dy2 * dy2));
            ang = (u16)_ZN4cstd5atan2E5Fix12IiES1_(dy2, sy);
            i2 = (ang >> 4) * 2;
            ((Bomb *)(raw + 0x4660 + (index<<6)))->x =
                (0xC0 - (int)(((s64)data_02082214[i2 + 1] * 0xF + 0x800) >> 12)) << 12;
            if (py >= 0x60) {
                ((Bomb *)(raw + 0x4660 + (index<<6)))->y =
                    (0x80 - (int)(((s64)data_02082214[i2] * 0xF + 0x800) >> 12)) << 12;
            } else {
                ((Bomb *)(raw + 0x4660 + (index<<6)))->y =
                    (0x40 - (int)(((s64)data_02082214[i2] * 0xF + 0x800) >> 12)) << 12;
            }
            a = ((Bomb *)(raw + 0x4660 + (index<<6)))->angle;
            if (data_02082214[(a >> 4) * 2 + 1] > 0) {
                ((Bomb *)(raw + 0x4660 + (index<<6)))->angle = 0x8000 - a;
            } else {
                ((Bomb *)(raw + 0x4660 + (index<<6)))->angle = 0 - a;
            }
        }
    }

    if (px - 12 >= 0x40) return;
    if (py + 12 <= 0x40) return;
    if (py - 12 >= 0x80) return;

    dy2 = 0x40 - px;
    dy1 = py - 0x40;
    sy = 0x80 - py;
    if (px >= 0x40 && py >= 0x40 && py <= 0x80) {
        ((Bomb *)(raw + 0x4660 + (index<<6)))->angle = 0x8000 - ((Bomb *)(raw + 0x4660 + (index<<6)))->angle;
        ((Bomb *)(raw + 0x4660 + (index<<6)))->x = 0x4C000;
        return;
    }
    if (px < 0x40 && py < 0x40) {
        ((Bomb *)(raw + 0x4660 + (index<<6)))->angle = 0 - ((Bomb *)(raw + 0x4660 + (index<<6)))->angle;
        ((Bomb *)(raw + 0x4660 + (index<<6)))->y = 0x34000;
        return;
    }
    if (px < 0x40 && py > 0x80) {
        ((Bomb *)(raw + 0x4660 + (index<<6)))->angle = 0 - ((Bomb *)(raw + 0x4660 + (index<<6)))->angle;
        ((Bomb *)(raw + 0x4660 + (index<<6)))->y = 0x8C000;
        return;
    }
    if (px < 0x40 && py > 0x40 && py < 0x80) {
        if (dy1 < sy) {
            if (dy2 < dy1) {
                ((Bomb *)(raw + 0x4660 + (index<<6)))->angle = 0x8000 - ((Bomb *)(raw + 0x4660 + (index<<6)))->angle;
                ((Bomb *)(raw + 0x4660 + (index<<6)))->x = 0x4C000;
                return;
            }
            ((Bomb *)(raw + 0x4660 + (index<<6)))->angle = 0 - ((Bomb *)(raw + 0x4660 + (index<<6)))->angle;
            ((Bomb *)(raw + 0x4660 + (index<<6)))->y = 0x34000;
            return;
        }
        if (dy2 < sy) {
            ((Bomb *)(raw + 0x4660 + (index<<6)))->angle = 0x8000 - ((Bomb *)(raw + 0x4660 + (index<<6)))->angle;
            ((Bomb *)(raw + 0x4660 + (index<<6)))->x = 0x4C000;
            return;
        }
        ((Bomb *)(raw + 0x4660 + (index<<6)))->angle = 0 - ((Bomb *)(raw + 0x4660 + (index<<6)))->angle;
        ((Bomb *)(raw + 0x4660 + (index<<6)))->y = 0x8C000;
        return;
    }

    if (py < 0x60) {
        sy = 0x40 - py;
    }
    cstd::sqrt((s64)(dy2 * dy2 + sy * sy));
    ang = (u16)_ZN4cstd5atan2E5Fix12IiES1_(sy, dy2);
    i2 = (ang >> 4) * 2;
    ((Bomb *)(raw + 0x4660 + (index<<6)))->x =
        (0x40 - (int)(((s64)data_02082214[i2 + 1] * 0xF + 0x800) >> 12)) << 12;
    if (py >= 0x60) {
        ((Bomb *)(raw + 0x4660 + (index<<6)))->y =
            (0x80 - (int)(((s64)data_02082214[i2] * 0xF + 0x800) >> 12)) << 12;
    } else {
        ((Bomb *)(raw + 0x4660 + (index<<6)))->y =
            (0x40 - (int)(((s64)data_02082214[i2] * 0xF + 0x800) >> 12)) << 12;
    }
    a = ((Bomb *)(raw + 0x4660 + (index<<6)))->angle;
    if (data_02082214[(a >> 4) * 2 + 1] < 0) {
        ((Bomb *)(raw + 0x4660 + (index<<6)))->angle = 0x8000 - a;
    } else {
        ((Bomb *)(raw + 0x4660 + (index<<6)))->angle = 0 - a;
    }
}
}
#pragma pop

// @symbol func_ov006_020d7524
extern "C" {
typedef struct { int f660; int f664; char pad08[0x2c]; unsigned char b694; unsigned char b695; unsigned char b696; unsigned char b697; unsigned char pad38; unsigned char b699; unsigned char pad3a; unsigned char b69b; char pad3c[4]; } E_7524;
typedef struct { char pad0[0x4660]; E_7524 e[112]; char pad1[0x7c]; int f62dc; } Self_7524;
void func_ov006_020d7524(void *raw) {
    Self_7524 *view = (Self_7524 *)raw;
    int i;
    for (i = 0; i < 0x70; i++) {
        if (view->e[i].b699 == 0) continue;
        {
            int b694 = view->e[i].b694;
            int palette = 1;
            int b695, b696;
            unsigned char sprite;
            int b697;
            int f664;
            int x, y;
            if (b694 == 3) palette = 0;
            b696 = view->e[i].b696;
            b695 = view->e[i].b695;
            if (b696 != 0) { sprite = data_ov006_0213bb28[b694][b695]; }
            else { sprite = data_ov006_0213bb18[b694][b695]; }
            b697 = view->e[i].b697;
            x = view->e[i].f660 >> 12;
            f664 = view->e[i].f664;
            y = f664 >> 12;
            if (b697 == 6) {
                if (view->e[i].b69b == 4) { y = (f664 - view->f62dc) >> 12; }
            }
            func_ov004_020afdd0(data_ov006_0213bb4c[sprite], x, y, -1, palette);
        }
    }
}
}

// @symbol func_ov006_020d7604
#pragma push
#pragma opt_strength_reduction off
#pragma opt_common_subs off
extern "C" {
void func_ov006_020d7604(void *raw)
{
    unsigned char *bytes = (unsigned char *)raw;
    int ca = 0;
    int cb = 0;
    int cc = 0;
    int i;

    for (i = 0; i < 0x70; i++) {
        unsigned char *bomb = bytes + i * 64;
        if (bomb[0x4698] != 0) {
            if (bytes[0x62f8] == 1) {
                if (bomb[0x4696] != 0) {
                    int v;
                    int q;
                    bomb[0x469b] = 3;
                    v = ca;
                    q = 0;
                    ((Bomb *)(bomb + 0x4660))->f30 = (unsigned short)(cc * 8);
                    while (v >= 5) { v -= 5; q++; }
                    ((unsigned char *)(int)(bytes + i * 64))[0x469c] = (unsigned char)(q * 10 + v);
                    ca++;
                    cc++;
                } else {
                    int v;
                    int q;
                    unsigned char *d = (unsigned char *)(int)(bomb + 0x469c);
                    *d = (unsigned char)cb;
                    bomb[0x469b] = 3;
                    v = cb;
                    ((Bomb *)(bomb + 0x4660))->f30 = (unsigned short)(cc * 8);
                    q = 0;
                    while (v >= 5) { v -= 5; q++; }
                    *d = (unsigned char)(q * 10 + v + 5);
                    cb++;
                    cc++;
                }
            } else if (bytes[0x62f8] != 0) {
                bomb[0x469c] = (unsigned char)cc;
                bomb[0x469b] = 3;
                ((Bomb *)(bomb + 0x4660))->f30 = (unsigned short)(cc * 8);
                cc++;
            } else {
                if (bytes[0x62f5] == bomb[0x4696] && bomb[0x4697] == 6) {
                    bomb[0x469c] = (unsigned char)cc;
                    bomb[0x469b] = 3;
                    ((Bomb *)(bomb + 0x4660))->f30 = (unsigned short)(cc * 8);
                    cc++;
                }
            }
        }
    }
    func_ov006_020d5e3c(raw);
}
}
#pragma pop

// @symbol func_ov006_020d7778
extern "C" {
void func_ov006_020d7778(void)
{
}
}

// @symbol func_ov006_020d777c
#pragma push
#pragma opt_common_subs off
extern "C" {

void func_ov006_020d777c(char *raw, int index)
{
    int off = index << 6;
    if (*(u16*)(raw + (index << 6) + 0x4690) != 0) {
        *(u16*)(raw + 0x4690 + off) -= 1;
        if (*(s16*)(raw + (index << 6) + 0x4690) < 0)
            *(u16*)(raw + (index << 6) + 0x4690) = 0;
        return;
    }
    {
        int slot = *(u8*)(raw + (index << 6) + 0x469c);
        int row = 0;
        int curX, curY, targetY, targetX;
        while (slot >= 10) { slot -= 10; row++; }
        off = row * 20;
        targetX = slot * 16 + 0x38;
        targetY = off - 0xC0;
        curX = ((Bomb *)(raw + 0x4660 + (index<<6)))->x >> 12;
        curY = ((Bomb *)(raw + 0x4660 + (index<<6)))->y >> 12;
        if (targetX == curX && targetY == curY) {
            *(u8*)(raw + (index << 6) + 0x469b) = 4;
            if (*(u8*)(raw + (index << 6) + 0x4696) != 0)
                func_02012718((int)0x1d9, ((Bomb *)(raw + 0x4660 + (index<<6)))->x);
            else
                func_02012718((int)0x1da, ((Bomb *)(raw + 0x4660 + (index<<6)))->x);
            return;
        }
        if (targetY != curY) {
            *(int*)(raw + 0x4664 + (index << 6)) += ((Bomb *)(raw + 0x4660 + (index << 6)))->speed;
            if ((((Bomb *)(raw + 0x4660 + (index<<6)))->y >> 12) >= targetY)
                ((Bomb *)(raw + 0x4660 + (index<<6)))->y = targetY << 12;
            return;
        }
        if (targetX > curX) {
            *(int*)(raw + 0x4660 + (index << 6)) += ((Bomb *)(raw + 0x4660 + (index << 6)))->speed;
            if (targetX <= (((Bomb *)(raw + 0x4660 + (index<<6)))->x >> 12))
                ((Bomb *)(raw + 0x4660 + (index<<6)))->x = targetX << 12;
        } else if (targetX < curX) {
            *(int*)(raw + 0x4660 + (index << 6)) -= ((Bomb *)(raw + 0x4660 + (index << 6)))->speed;
            if (targetX >= curX)
                ((Bomb *)(raw + 0x4660 + (index<<6)))->x = targetX << 12;
        }
    }
}
}
#pragma pop

// @symbol func_ov006_020d7958
extern "C" {
void func_ov006_020d7958(void)
{
}
}

// @symbol func_ov006_020d795c
#pragma push
#pragma opt_strength_reduction off
#pragma opt_common_subs off
extern "C" {
void func_ov006_020d795c(char *raw, int index)
{
    int color, x;
    {
        s16 sine = data_02082214[((*(u16 *)(raw + index * 0x40 + 0x468c)) >> 4) * 2 + 1];
        *(int *)((char *)(raw + 0x4660) + index * 0x40) +=
            (int)(((s64)sine * ((Bomb *)(raw + 0x4660 + index * 0x40))->speed + 0x800) >> 12);
    }
    {
        s16 sine = data_02082214[((*(u16 *)(raw + index * 0x40 + 0x468c)) >> 4) * 2];
        *(int *)((char *)(raw + 0x4664) + index * 0x40) +=
            (int)(((s64)sine * ((Bomb *)(raw + 0x4660 + index * 0x40))->speed + 0x800) >> 12);
    }
    color = *(u8 *)(raw + index * 0x40 + 0x4696);
    x = *(int *)(raw + index * 0x40 + 0x4660) >> 12;
    if (color != 0) {
        if (x >= 0x110) {
            *(u8 *)(raw + index * 0x40 + 0x469b) = 2;
            *(int *)(raw + index * 0x40 + 0x4660) = 0x80000;
            *(int *)(raw + index * 0x40 + 0x4664) = -0xf0000;
        }
    } else {
        if (x <= -0x10) {
            *(u8 *)(raw + index * 0x40 + 0x469b) = 2;
            *(int *)(raw + index * 0x40 + 0x4660) = 0x80000;
            *(int *)(raw + index * 0x40 + 0x4664) = -0xf0000;
        }
    }
    func_ov006_020d634c(raw, *(u8 *)(raw + index * 0x40 + 0x4696));
}
}
#pragma pop

// @symbol func_ov006_020d7a84
#pragma push
#pragma opt_strength_reduction off
#pragma opt_common_subs off
extern "C" {
void func_ov006_020d7a84(char *raw, int index)
{
    int color, x, z, tx, ty;

    tx = (*(u8 *)(raw + index * 0x40 + 0x4696) != 0) ? 0x100 : 0;
    ty = 0x60;
    tx -= *(int *)(raw + index * 0x40 + 0x4660) >> 12;
    ty -= *(int *)(raw + index * 0x40 + 0x4664) >> 12;

    *(s16 *)(raw + index * 0x40 + 0x468c) =
        _ZN4cstd5atan2E5Fix12IiES1_(ty, tx);

    {
        s16 sine = data_02082214[
            ((*(u16 *)(raw + index * 0x40 + 0x468c) >> 4) * 2) + 1];

        *(int *)((char *)(raw + 0x4660) +
                  index * 0x40) +=
            (int)(((s64)sine *
                   ((Bomb *)(raw + 0x4660 + index * 0x40))->speed + 0x800) >> 12);
    }

    {
        s16 sine = data_02082214[
            (*(u16 *)(raw + index * 0x40 + 0x468c) >> 4) * 2];

        *(int *)((char *)(raw + 0x4664) +
                  index * 0x40) +=
            (int)(((s64)sine *
                   ((Bomb *)(raw + 0x4660 + index * 0x40))->speed + 0x800) >> 12);
    }

    color = *(u8 *)(raw + index * 0x40 + 0x4696);
    x = *(int *)(raw + index * 0x40 + 0x4660) >> 12;
    z = *(int *)(raw + index * 0x40 + 0x4664) >> 12;
    tx = color ? 0x100 : 0;

    if (x > tx + 2)
        goto done;
    if (x < tx - 2)
        goto done;
    if (z > 0x62)
        goto done;
    if (z < 0x5e)
        goto done;

    *(u8 *)(raw + index * 0x40 + 0x469b) = 1;
    if (*(u8 *)(raw + index * 0x40 + 0x4696) != 0)
        *(s16 *)(raw + index * 0x40 + 0x468c) = 0;
    else
        *(u16 *)(raw + index * 0x40 + 0x468c) = 0x8000;

done:
    func_ov006_020d634c(raw, *(u8 *)(raw + index * 0x40 + 0x4696));
}
}
#pragma pop

// @symbol func_ov006_020d7c00
/* Runs the bomb's current handler: the byte at +0x3b indexes the
 * pointer-to-member table data_ov006_02141708. */
extern "C" {
struct C_7c00;
typedef void (C_7c00::*PMF_7c00)(int);
struct Entry_7c00 { PMF_7c00 pmf; };
struct Elem_7c00 { char pad[0x40]; };
struct C_7c00 {};
extern "C" Entry_7c00 data_ov006_02141708[];
extern "C" void func_ov006_020d7c00(C_7c00* scene, int index){
  Elem_7c00* bomb = (Elem_7c00*)scene + index;
  unsigned char state = ((Bomb *)((char *)bomb + 0x4660))->f3b;
  (scene->*(data_ov006_02141708[state].pmf))(index);
}
}

// @symbol func_ov006_020d7c4c
/* Roaming step: advance x/y along the angle at speed (sine table, Fix12),
 * then bounce off the box selected by color (0xc0..0x100 x 0x40..0x80 for
 * color 1, 0..0x40 x 0x40..0x80 for color 0): a vertical wall mirrors the
 * angle (0x8000 - a), a horizontal one negates it, and the coordinate is
 * pinned just inside the wall. */
extern "C" {
void func_ov006_020d7c4c(dScMgBomroom_c *self, int idx)
{
    int xv;
    int yv;
    s16 sinv;
    s16 cosv;

    sinv = data_02082214[(self->mBombs[idx].angle >> 4) * 2 + 1];
    self->mBombs[idx].x += (s32)(((s64)sinv * self->mBombs[idx].speed + 0x800) >> 12);
    cosv = data_02082214[(self->mBombs[idx].angle >> 4) * 2];
    self->mBombs[idx].y += (s32)(((s64)cosv * self->mBombs[idx].speed + 0x800) >> 12);
    xv = self->mBombs[idx].x >> 12;
    yv = self->mBombs[idx].y >> 12;
    if (self->mBombs[idx].color != 0) {
        if (xv + 0xc > 0x100) {
            self->mBombs[idx].angle = 0x8000 - self->mBombs[idx].angle;
            self->mBombs[idx].x = 0xf4000;
        } else if (xv - 0xc < 0xc0) {
            self->mBombs[idx].angle = 0x8000 - self->mBombs[idx].angle;
            self->mBombs[idx].x = 0xcc000;
        }
        if (yv + 0xc > 0x80) {
            self->mBombs[idx].angle = -self->mBombs[idx].angle;
            self->mBombs[idx].y = 0x74000;
        } else if (yv - 0xc < 0x40) {
            self->mBombs[idx].angle = -self->mBombs[idx].angle;
            self->mBombs[idx].y = 0x4c000;
        }
    } else {
        if (xv + 0xc > 0x40) {
            self->mBombs[idx].angle = 0x8000 - self->mBombs[idx].angle;
            self->mBombs[idx].x = 0x34000;
        } else if (xv - 0xc < 0) {
            self->mBombs[idx].angle = 0x8000 - self->mBombs[idx].angle;
            self->mBombs[idx].x = 0xc000;
        }
        if (yv + 0xc > 0x80) {
            self->mBombs[idx].angle = -self->mBombs[idx].angle;
            self->mBombs[idx].y = 0x74000;
        } else if (yv - 0xc < 0x40) {
            self->mBombs[idx].angle = -self->mBombs[idx].angle;
            self->mBombs[idx].y = 0x4c000;
        }
    }
}
}

// @symbol func_ov006_020d7e7c
extern "C" {
void func_ov006_020d7e7c(char *c, int i)
{
    char *b = c + i * 64;
    if (*(unsigned char *)(b + 0x469d) == 0) return;
    if (*(unsigned char *)(b + 0x4695) != 0) return;
    if (*(unsigned short *)(b + 0x468e) != 1) return;
    Sound::PlayBank2_2D(0x1e0);
    *(short *)(c + 0x62f0) = 0x60;
}
}

// @symbol func_ov006_020d7edc
extern "C" {
void func_ov006_020d7edc(unsigned char *c, int idx)
{
    unsigned char *slot = c + idx * 0x40;
    if (*(unsigned short *)(slot + 0x4690) != 0) {
        unsigned short *p = (unsigned short *)(c + 0x4690 + idx * 0x40);
        *p = *p - 1;
        if (*(short *)(slot + 0x4690) < 0) *(unsigned short *)(slot + 0x4690) = 0;
    } else {
        *(unsigned char *)(slot + 0x4697) = 4;
        *(unsigned char *)(slot + 0x4694) = 3;
        *(unsigned char *)(slot + 0x4695) = 0;
        *(unsigned short *)(slot + 0x468e) = 0;
    }
}
}

// @symbol func_ov006_020d7f5c
/* A held bomb follows the stylus less its grab offset, and +0x69e records
 * whether it is over its own pen. With nothing held (0x62f6 is 0xff) the
 * bomb goes back to state 1 instead. */
#pragma push
#pragma opt_common_subs off
extern "C" {
#define B ((char *)self_ + idx * 0x40 + 0x4000)
void func_ov006_020d7f5c(char *self_, int idx)
{
    if (*(u8 *)(self_ + 0x62f6) == 0xff) {
        *(u8 *)(B + 0x697) = 1;
        *(u16 *)((char *)self_ + 0x4692 + idx * 0x40) += 0x40;
        func_ov006_020d6b88(self_, idx);
        func_ov006_020d6c90(self_, idx);
    } else {
        int old_x = *(int *)(B + 0x660);
        int old_y = *(int *)(B + 0x664);
        int cx, cy;
        u8 was;
        int t = gActivePlayerSlot;

        *(int *)(B + 0x660) = (gTouchX[t << 2] << 12) - *(int *)(B + 0x668);
        *(int *)(B + 0x664) = (gTouchY[t << 2] << 12) - *(int *)(B + 0x66c);
        cx = old_x >> 12;
        cy = old_y >> 12;

        func_ov006_020d6e8c(self_, idx);

        {
            int t2 = gActivePlayerSlot;
            *(int *)(B + 0x660) = (gTouchX[t2 << 2] << 12) - *(int *)(B + 0x668);
            *(int *)(B + 0x664) = (gTouchY[t2 << 2] << 12) - *(int *)(B + 0x66c);
        }

        was = *(u8 *)(B + 0x69e);
        if (*(u8 *)(B + 0x696) != 0) {
            if (cx > 0xc0 && cy > 0x40 && cy < 0x80) {
                *(u8 *)(B + 0x69e) = 1;
            } else {
                *(u8 *)(B + 0x69e) = 0;
            }
        } else {
            if (cx < 0x40 && cy > 0x40 && cy < 0x80) {
                *(u8 *)(B + 0x69e) = 1;
            } else {
                *(u8 *)(B + 0x69e) = 0;
            }
        }

        if (was == 0 && *(u8 *)(B + 0x69e) != 0) {
            func_02012718(0x1e4, *(int *)(B + 0x660));
        }

        *(int *)(B + 0x688) = func_02012468(*(int *)(B + 0x688), 2, 0x1e5, 4, 0, 0,
                                            func_020126e8(*(int *)(B + 0x660)), 0);
        *(int *)(B + 0x67c) = *(int *)(B + 0x660);
        *(int *)(B + 0x680) = *(int *)(B + 0x664);
    }
}
#undef B
}
#pragma pop

// @symbol func_ov006_020d816c
/* While the bomb's +0x32 timer runs it counts down (unless +0x62fb is
 * set) and the bomb moves along its angle; once it reaches 0 the bomb goes
 * to state 4 and func_ov006_020d68a8 takes over. */
extern "C" {
struct Ent_816c {
    int x;                  /* 0x00 */
    int y;                  /* 0x04 */
    char pad08[8];
    int spd;                /* 0x10 */
    char pad14[8];
    int px;                 /* 0x1c */
    int py;                 /* 0x20 */
    char pad24[8];
    unsigned short ang;     /* 0x2c */
    unsigned short f2e;     /* 0x2e */
    char pad30[2];
    unsigned short h;       /* 0x32 */
    unsigned char b34;      /* 0x34 */
    unsigned char b35;      /* 0x35 */
    char pad36[1];
    unsigned char b37;      /* 0x37 */
    char pad38[2];
    unsigned char b3a;      /* 0x3a */
    char pad3b[2];
    unsigned char b3d;      /* 0x3d */
    char pad3e[2];
};

struct S32_816c { int v; char pad[0x3c]; };
struct S16_816c { unsigned short v; char pad[0x3e]; };

void func_ov006_020d816c(char *self, int idx)
{
    int t;

    if (((struct Ent_816c *)(self + 0x4660))[idx].h != 0) {
        if (*(unsigned char *)(self + 0x62fb) == 0) {
            ((struct S16_816c *)(self + 0x4692))[idx].v--;
        }
        if ((short)((struct Ent_816c *)(self + 0x4660))[idx].h < 0) {
            ((struct Ent_816c *)(self + 0x4660))[idx].h = 0;
        }
        if (((struct Ent_816c *)(self + 0x4660))[idx].h <= 0x40) {
            ((struct Ent_816c *)(self + 0x4660))[idx].b34 = 2;
        }
    } else {
        ((struct Ent_816c *)(self + 0x4660))[idx].h = 0;
        ((struct Ent_816c *)(self + 0x4660))[idx].b37 = 4;
        ((struct Ent_816c *)(self + 0x4660))[idx].b34 = 3;
        ((struct Ent_816c *)(self + 0x4660))[idx].b35 = 0;
        ((struct Ent_816c *)(self + 0x4660))[idx].f2e = 0;
        ((struct Ent_816c *)(self + 0x4660))[idx].b3a = 0;
        ((struct Ent_816c *)(self + 0x4660))[idx].b3d = 1;
        func_ov006_020d68a8(self, idx);
        return;
    }

    if (*(unsigned char *)(self + 0x62fb) != 0) {
        return;
    }

    t = data_02082214[((int)((struct Ent_816c *)(self + 0x4660))[idx].ang >> 4) * 2 + 1];
    ((struct S32_816c *)(self + 0x4660))[idx].v += (int)(((long long)t * ((struct Ent_816c *)(self + 0x4660))[idx].spd + 0x800) >> 12);
    t = data_02082214[((int)((struct Ent_816c *)(self + 0x4660))[idx].ang >> 4) * 2];
    ((struct S32_816c *)(self + 0x4664))[idx].v += (int)(((long long)t * ((struct Ent_816c *)(self + 0x4660))[idx].spd + 0x800) >> 12);
    func_ov006_020d6e8c(self, idx);
    func_ov006_020d6d7c(self, idx);
    ((struct Ent_816c *)(self + 0x4660))[idx].px = ((struct Ent_816c *)(self + 0x4660))[idx].x;
    ((struct Ent_816c *)(self + 0x4660))[idx].py = ((struct Ent_816c *)(self + 0x4660))[idx].y;
}
}

// @symbol func_ov006_020d8324
extern "C" {
void func_ov006_020d8324(char *c, int i)
{
    char *b = c + (i << 6);
    if (*(unsigned short *)(b + 0x4690) != 0) {
        unsigned short *h = (unsigned short *)((c + 0x4690) + (i << 6));
        *h = *h - 1;
    } else {
        *(unsigned char *)(b + 0x4699) = 1;
        *(unsigned char *)(b + 0x4694) = 1;
        *(unsigned char *)(b + 0x4697) = 1;
    }
}
}

// @symbol func_ov006_020d836c
/* Per-frame bomb pass: with no touch, nothing is held (0x62f6 = 0xff);
 * then every live bomb runs its state handler from the pointer-to-member
 * table data_ov006_02141730, indexed by +0x697, and func_ov006_020d69b8. */
#pragma push
#pragma opt_strength_reduction off
extern "C" {
struct C_836c;
typedef void (C_836c::*PMF_836c)(int);
extern PMF_836c data_ov006_02141730[];
void func_ov006_020d836c(char *c)
{
    if (*(u8 *)(gTouchHeld + gActivePlayerSlot) == 0)
        *(u8 *)(c + 0x62f6) = 0xff;
    int i;
    for (i = 0; i < 0x70; i++) {
        u8 *o = (u8 *)c + i * 0x40;
        u8 flag;
        o += 0x4000;
        flag = o[0x698];
        if (flag != 0) {
            C_836c *cc = (C_836c *)c;
            (cc->*data_ov006_02141730[o[0x697]])(i);
            func_ov006_020d69b8(c, i);
        }
    }
}
}
#pragma pop

// @symbol func_ov006_020d8408
/* Spawner: once the +0x62e2 delay runs out, picks a step from the spawn
 * count at +0x62d8, drops that many new bombs into free records with a
 * random color and angle, and reloads the delay from
 * data_ov006_0212e2e8. */
#pragma push
#pragma opt_strength_reduction off
#pragma opt_common_subs off
#pragma opt_loop_invariants off
extern "C" {
void func_ov006_020d8408(char *c)
{
  s32 sb;
  s32 step;
  s32 ang;
  s32 i;
  s32 one;
  s32 off;
  s32 t0;
  u32 rnd;
  s32 cnt;
  s32 flag;
  s32 j;
  s32 z0;
  s32 z1;
  s32 z2;
  s32 z3;
  s32 v80000;
  s32 new_var;
  s32 v4;
  s32 v200;
  s32 vB8000;
  s32 j0;
  if ((*((u16 *) ((c + 0x6200) + 0xe2))) != 0)
  {
    (*((u16 *) ( ((int) (c + 0x62e2)))))--;
    if ((*((s16 *) ((c + 0x6200) + 0xe2))) < 0)
    {
      *((u16 *) ((c + 0x6200) + 0xe2)) = 0;
    }
    return;
  }
  t0 = *((s32 *) ((c + 0x6000) + 0x2d8));
  sb = 0;
  if (t0 >= 0x12c)
  {
    sb = 0xc;
  }
  else
    if (t0 >= 0xc6)
  {
    sb = 0xb;
  }
  else
    if (t0 >= 0x9f)
  {
    sb = 0xa;
  }
  else
    if (t0 >= 0x84)
  {
    sb = 9;
  }
  else
    if (t0 >= 0x5c)
  {
    sb = 8;
  }
  else
    if (t0 >= 0x39)
  {
    sb = 7;
  }
  else
    if (t0 >= 0x21)
  {
    sb = 6;
  }
  else
    if (t0 >= 0x1b)
  {
    sb = 5;
  }
  else
    if (t0 >= 0x15)
  {
    sb = 4;
  }
  else
    if (t0 >= 0xf)
  {
    sb = 3;
  }
  else
    if (t0 >= 9)
  {
    sb = 2;
  }
  else
    if (t0 >= 3)
  {
    sb = 1;
  }
  cnt = 1;
  step = 0;
  if (sb >= 5)
  {
    cnt = 2;
  }
  if (sb == 9)
  {
    cnt = 1;
  }
  flag = 0;
  if (sb == 7)
  {
    unsigned char *pf = (unsigned char *) ((int) ( ((int) (c + 0x62fc))));
    s32 bit = *((unsigned char *) ((c + 0x6000) + 0x2fc));
    cnt = 2;
    flag = (bit & 1) + 1;
    *pf ^= 1;
    step = 0x3000;
  }
  new_var = sb;
  if (new_var == 8)
  {
    s32 bit = (*((unsigned char *) ((c + 0x6000) + 0x2fc))) & 1;
    unsigned char *pf = (unsigned char *) ((int) ( ((int) (c + 0x62fc))));
    s32 x = *pf;
    cnt = bit + 2;
    *pf = x ^ 1;
    flag = bit + 1;
    if (cnt == 2)
    {
      step = 0x3000;
    }
    else
    {
      step = 0x1800;
    }
  }
  if (new_var == 10)
  {
    cnt = 4;
    step = 0x3000;
  }
  if (sb == 11)
  {
    unsigned char *pf = (unsigned char *) ((int) ( ((int) (c + 0x62fc))));
    s32 bit = *((unsigned char *) ((c + 0x6000) + 0x2fc));
    cnt = 3;
    flag = (bit & 1) + 1;
    *pf ^= 1;
    step = 0x1800;
  }
  if (sb >= 12)
  {
    cnt = 6;
    step = 0x1800;
  }
  i = 0;
  if (cnt > 0)
  {
    v80000 = 0x80000;
    v4 = 4;
    v200 = 0x200;
    vB8000 = 0xb8000;
    off = 0x4698;
    ang = 0;
    z0 = 0;
    z1 = 0;
    z2 = 0;
    z3 = 0;
    one = 1;
    j0 = 0;
    do
    {
      s32 z = z0;
      j = j0;
      while (1)
      {
        char *row;
        unsigned char *slot = (unsigned char *) ((c + (j << 6)) + off);
        if ((*slot) == 0)
        {
          unsigned char *ptype;
          *slot = (unsigned char) one;
          (c + (j << 6))[0x4697] = (char) z;
          (c + (j << 6))[0x469b] = (char) z;
          (c + (j << 6))[0x469c] = (char) z;
          (c + (j << 6))[0x469d] = (char) z;
          *((s32 *) ((c + (j << 6)) + 0x4660)) = v80000;
          *((s16 *) ((c + (j << 6)) + 0x4690)) = (s16) v4;
          *((s16 *) ((c + (j << 6)) + 0x4692)) = (s16) v200;
          rnd = (u32) RandomIntInternal(&data_0209d4b8);
          ptype = (unsigned char *) ((int) ( ((int) ((c + (j << 6)) + 0x4696))));
          *ptype = (((rnd >> 16) & 0x7fff) << 1) >> 15;
          *((s32 *) ((c + (j << 6)) + 0x4670)) = 0x999;
          *((s32 *) ((c + (j << 6)) + 0x4688)) = z1;
          if (sb == 0)
          {
            *ptype = (unsigned char) one;
            func_ov006_020d66c4(c, z1);
          }
          if (sb <= 1)
          {
            rnd = (u32) RandomIntInternal(&data_0209d4b8);
            *((s16 *) ((((0, c)) + (j << 6)) + 0x468c)) = (((((rnd >> 16) & 0x7fff) << 2) >> 15) << 12) + 0x2000;
            *((s32 *) ((c + (j << 6)) + 0x4664)) = z2;
          }
          else
            if (flag != 0)
          {
            if (flag == 1)
            {
              *((s16 *) ((c + (j << 6)) + 0x468c)) = ang + 0x2000;
              *((s32 *) ((c + (j << 6)) + 0x4664)) = z2;
              func_ov006_020d66c4(c, z2);
            }
            else
            {
              *((s16 *) ((c + (j << 6)) + 0x468c)) = ang + 0xa000;
              *((s32 *) ((c + (j << 6)) + 0x4664)) = vB8000;
              func_ov006_020d66c4(c, one);
            }
          }
          else
            if (((*((s32 *) ((c + 0x6000) + 0x2d8))) & 1) != 0)
          {
            if (step != 0)
            {
              *((s16 *) ((c + (j << 6)) + 0x468c)) = (step * (i >> 1)) + 0x2800;
            }
            else
            {
              rnd = (u32) RandomIntInternal(&data_0209d4b8);
              *((s16 *) ((c + (j << 6)) + 0x468c)) = (((((rnd >> 16) & 0x7fff) << 2) >> 15) << 12) + 0x2000;
            }
            *((s32 *) ((c + (j << 6)) + 0x4664)) = z3;
            func_ov006_020d66c4(c, z3);
          }
          else
          {
            if (step != 0)
            {
              *((s16 *) ((c + (j << 6)) + 0x468c)) = (step * (i >> 1)) + 0xa800;
            }
            else
            {
              rnd = (u32) RandomIntInternal(&data_0209d4b8);
              *((s16 *) ((c + (j << 6)) + 0x468c)) = (((((rnd >> 16) & 0x7fff) << 2) >> 15) << 12) + 0xa000;
            }
            *((s32 *) ((c + (j << 6)) + 0x4664)) = vB8000;
            func_ov006_020d66c4(c, one);
          }
          (*((s32 *) ((int) ( ((int) (c + 0x62d8))))))++;
          break;
        }
        j++;
        if (j >= 0x70)
        {
          break;
        }
      }

      ang += step;
      i++;
    }
    while (i < cnt);
  }
  *((u16 *) ((c + 0x6200) + 0xe2)) = data_ov006_0212e2e8[sb];
}
}
#pragma pop

// @symbol func_ov006_020d8904
extern "C" {
void func_ov006_020d8904(char *p)
{
    int i;
    for (i = 0; i < 0x70; i++) {
        if (((unsigned char (*)[0x40])(p + 0x4000))[i][0x698] != 0) {
            if (((unsigned char (*)[0x40])(p + 0x4000))[i][0x697] == 6)
                ((unsigned char (*)[0x40])(p + 0x4000))[i][0x699] = 0;
        }
    }
}
}

// @symbol func_ov006_020d893c
/* Zeroes all 0x70 bomb records (stride 0x40 at +0x4660): 11 words, 4
 * halfwords and 10 bytes each. */
extern "C" {
typedef struct {
    u32 w0;      /* +0x00 */
    u32 w1;
    u32 w2;
    u32 w3;
    u32 w4;
    u32 w5;
    u32 w6;
    u32 w7;
    u32 w8;
    u32 w9;
    u32 w10;     /* +0x28 */
    u16 h0;      /* +0x2c */
    u16 h1;
    u16 h2;
    u16 h3;      /* +0x32 */
    u8 b0;       /* +0x34 */
    u8 b1;
    u8 b2;
    u8 b3;
    u8 b4;       /* +0x38 */
    u8 b5;       /* +0x39 */
    u8 b6;       /* +0x3a */
    u8 b7;       /* +0x3b */
    u8 b8;       /* +0x3c */
    u8 b9;       /* +0x3d */
    char _pad[2];
} Entry_893c; /* 0x40 */

typedef struct {
    char _pad0[0x4660];
    Entry_893c entries[0x70];
} Work_893c;

void func_ov006_020d893c(char *c)
{
    Work_893c *w = (Work_893c *)c;
    int i;
    for (i = 0; i < 0x70; i++) {
        w->entries[i].w0 = 0;
        w->entries[i].w1 = 0;
        w->entries[i].w2 = 0;
        w->entries[i].w3 = 0;
        w->entries[i].w4 = 0;
        w->entries[i].w5 = 0;
        w->entries[i].w6 = 0;
        w->entries[i].w7 = 0;
        w->entries[i].w8 = 0;
        w->entries[i].w9 = 0;
        w->entries[i].w10 = 0;
        w->entries[i].h0 = 0;
        w->entries[i].h1 = 0;
        w->entries[i].h2 = 0;
        w->entries[i].h3 = 0;
        w->entries[i].b0 = 0;
        w->entries[i].b1 = 0;
        w->entries[i].b2 = 0;
        w->entries[i].b3 = 0;
        w->entries[i].b4 = 0;
        w->entries[i].b7 = 0;
        w->entries[i].b5 = 0;
        w->entries[i].b6 = 0;
        w->entries[i].b8 = 0;
        w->entries[i].b9 = 0;
    }
}
}

// @symbol func_ov006_020d89c4
#pragma push
#pragma opt_strength_reduction off
extern "C" {
void func_ov006_020d89c4(char *self)
{
    int j;

    func_ov006_020d836c(self);

    if (*(u16 *)(self + 0x62e8) != 0) {
        (*(u16 *)(self + 0x62e8))--;
        if (*(s16 *)(self + 0x62e8) < 0)
            *(u16 *)(self + 0x62e8) = 0;
        return;
    }

    for (j = 0; j < 0x70; j++) {
        u8 *row = (u8 *)self + j * 0x40;
        u8 *q = (u8 *)(row + 0x4698);
        if (*q == 2) {
            if (((u8 (*)[0x40])(self + 0x4000))[j][0x697] == 6
             && ((u8 (*)[0x40])(self + 0x4000))[j][0x69b] == 4) {
                *q = 0;
                ((u8 (*)[0x40])(self + 0x4000))[j][0x699] = 0;
            }
        }
    }

    if (*(u8 *)(self + 0x62f8) != 0) {
        *(u16 *)(self + 0x62e8) = 0x60;
        *(int *)(self + 0x62d0) = 4;
        *(int *)(self + 0x62d4) = 0;
        func_ov004_020adb1c(*(u16 *)(self + 0x62ee) + func_ov004_020adbc0());
    } else {
        func_ov004_020adb1c(*(u16 *)(self + 0x62ee) + func_ov004_020adbc0());
        *(int *)(self + 0x62d0) = 2;
        *(int *)(self + 0x62d4) = 0;
        *(u8 *)(self + 0x62f8) = 0;
        *(u8 *)(self + 0x62fb) = 0;
    }
    *(u8 *)(self + 0x62f9) = 0;
    *(u16 *)(self + 0x62ee) = 0;
    SetBg0Offset(0, 0);
}
}
#pragma pop

// @symbol func_ov006_020d8af8
extern "C" {
void func_ov006_020d8af8(char *self)
{
    int found;
    int j;
    u8 (*arr)[0x40] = (u8 (*)[0x40])self;

    func_ov006_020d836c(self);

    if (*(u16 *)(self + 0x62e8) != 0) {
        (*(u16 *)(self + 0x62e8))--;
        if (*(s16 *)(self + 0x62e8) > 0)
            return;
        *(u16 *)(self + 0x62e8) = 0;
        *(u16 *)(self + 0x62e0) = 0x10;
        if (func_ov006_020d8c88(self) != 0) {
            *(u8 *)(self + 0x62f4) = 1;
            Sound::PlayBank2_2D(0x1dd);
        }
        return;
    }

    if (*(u16 *)(self + 0x62e0) != 0) {
        (*(u16 *)(self + 0x62e0))--;
        return;
    }

    found = 0;
    j = 0;
    do {
        if (arr[j][0x4698] == 1 && arr[j][0x4697] == 6 && arr[j][0x469b] == 4)
            found = j + 1;
        j++;
    } while (j < 0x70);

    (*(u16 *)(self + 0x62ea))++;
    if (*(u16 *)(self + 0x62ea) < 4)
        return;

    if (found != 0) {
        arr[found - 1][0x4698] = 2;
        *(u16 *)(self + 0x62ea) = 0;
        (*(u16 *)(self + 0x62ee))++;
        Sound::PlayBank2_2D(0x1bc);
    } else {
        *(u16 *)(self + 0x62ea) = 0;
        *(u16 *)(self + 0x62e8) = 0x40;
        *(int *)(self + 0x62d4) = 3;
    }
}
}

// @symbol func_ov006_020d8c88
extern "C" {
int func_ov006_020d8c88(char *c)
{
    int i;
    unsigned char (*arr)[0x40];
    i = 0;
    arr = (unsigned char (*)[0x40])c;
    do {
        if (arr[i][0x4698] == 1 && arr[i][0x4697] == 6)
            return 1;
        i++;
    } while (i < 0x70);
    return 0;
}
}

// @symbol func_ov006_020d8cc4
#pragma push
#pragma opt_strength_reduction off
extern "C" {
void func_ov006_020d8cc4(char *r5)
{
    func_ov006_020d836c(r5);
    int c4 = 0;
    int c2 = 0;
    int i = 0;
    do {
        char *e = r5 + (i << 6);
        e = e + 0x4000;
        if (*(unsigned char *)(e + 0x698) != 0 && *(unsigned char *)(e + 0x697) == 6) {
            unsigned char v = *(unsigned char *)(e + 0x69b);
            if (v != 2) c2++;
            if (v != 4) c4++;
        }
        i++;
    } while (i < 0x70);
    if (c2 == 0 && *(unsigned char *)(r5 + 0x6000 + 0x2f4) == 0) {
        func_ov006_020d7604(r5);
    }
    if (c4 != 0) return;
    func_ov006_020d5e1c(r5);
    *(short *)(r5 + 0x6200 + 0xe8) = 0x10;
    *(int *)(r5 + 0x6000 + 0x2d4) = 2;
    *(unsigned char *)(r5 + 0x6000 + 0x2f9) = 1;
}
}
#pragma pop

// @symbol func_ov006_020d8d84
extern "C" {
void func_ov006_020d8d84(char *self)
{
    int flag;
    int count;
    int i;
    u8 (*arr)[0x40] = (u8 (*)[0x40])self;
    int (*iarr)[0x10] = (int (*)[0x10])self;

    func_ov006_020d836c(self);

    if (*(u16 *)(self + 0x62e8) != 0) {
        (*(u16 *)(self + 0x62e8))--;
        if (*(s16 *)(self + 0x62e8) < 0)
            *(s16 *)(self + 0x62e8) = 0;
        return;
    }

    if (*(u8 *)(self + 0x62f8) != 0) {
        flag = 0;
        count = 0;
        i = 0;
        do {
            if (arr[i][0x4698] != 0 && arr[i][0x4697] != 6) {
                arr[i][0x4697] = 6;
                arr[i][0x469b] = 0;
                iarr[i][0x119c] = 0x4000;
                if (arr[i][0x4696] != 0)
                    flag = 1;
                count++;
            }
            i++;
        } while (i < 0x70);
        if (count != 0) {
            Sound::PlayBank2_2D(0x1e6);
            if (*(u8 *)(self + 0x62f8) == 1) {
                func_ov006_020d634c(self, 0);
                func_ov006_020d634c(self, 1);
            } else {
                func_ov006_020d634c(self, flag);
            }
        }
    } else {
        i = 0;
        do {
            if (arr[i][0x4698] != 0 && arr[i][0x4697] == 5) {
                if (*(u8 *)(self + 0x62f5) == arr[i][0x4696]) {
                    arr[i][0x4697] = 6;
                    arr[i][0x469b] = 0;
                    iarr[i][0x119c] = 0x4000;
                }
            }
            i++;
        } while (i < 0x70);
        Sound::PlayBank2_2D(0x1e6);
        func_ov006_020d634c(self, *(u8 *)(self + 0x62f5));
    }
    *(int *)(self + 0x62d4) = 1;
}
}

// @symbol func_ov006_020d8f34
extern "C" {
void func_ov006_020d8f34(char *c)
{
    if (*(u16 *)(c + 0x62e8) == 0)
        return;
    *(u16 *)(c + 0x62e8) -= 1;
    if (*(s16 *)(c + 0x62e8) > 0)
        return;
    *(u16 *)(c + 0x62e8) = 0;
    func_ov004_020b0a54(0x10);
    *(u8 *)(c + 0xc3) = 0;
}
}

// @symbol func_ov006_020d8f98
/* Runs the +0x62d4 sub-state through the pointer-to-member table
 * data_ov006_021416a0, decoded by hand (a function word, then a
 * this-adjust whose low bit marks a virtual call), then the three
 * per-frame passes. */
extern "C" {
void func_ov006_020d8f98(unsigned char *c)
{
    int idx = *(int *)(c + 0x62d4);
    int *e = &data_ov006_021416a0[idx * 2];
    int off = e[1];
    void *obj = c + (off >> 1);
    void (*f)(void *);
    if (off & 1) f = (void (*)(void *))(*(int *)(*(int *)obj + e[0]));
    else f = (void (*)(void *))e[0];
    f(obj);
    func_ov006_020d65c8((C_65c8 *)c);
    func_ov006_020d6278((C_6278 *)c);
    func_ov006_020d5fec((C_5fec *)c);
}
}

// @symbol func_ov006_020d8ff4
extern "C" {
void func_ov006_020d8ff4(void *c)
{
    func_ov006_020d65c8((C_65c8 *)c);
    func_ov006_020d8408((char *)c);
    func_ov006_020d836c((char *)c);
    func_ov006_020d6784((char *)c);
}
}

// @symbol func_ov006_020d9020
extern "C" {
void func_ov006_020d9020(void *c)
{
    unsigned char *p = (unsigned char *)c;
    if (p[0xc4] == 0) { p[0xc3] = 1; p[0xc4] = 1; *(short *)(p + 0xc0) = 0; }
    *(int *)(p + 0x6000 + 0x2d0) = 2;
}
}

// @symbol func_ov006_020d904c
extern "C" {
void func_ov006_020d904c(void *c)
{
    func_ov006_020d6630(c);
    func_ov006_020d62e0(c);
    func_ov006_020d604c(c);
    *(int *)((char *)c + 0x6000 + 0x2d0) = 1;
}
}

// @symbol func_ov006_020d907c
/* Resets the round: clears every bomb record and the round counters
 * between +0x62d8 and +0x62fc, then runs the other reset helpers. */
extern "C" {
void func_ov006_020d907c(void *p)
{
    char *c = (char *)p;
    func_ov006_020d893c(c);
    *(short *)(c + 0x62e2) = 0;
    *(short *)(c + 0x62e4) = 0;
    *(short *)(c + 0x62e6) = 0;
    *(unsigned char *)(c + 0x62f6) = 0xff;
    *(unsigned char *)(c + 0x62f7) = 0;
    *(short *)(c + 0x62ea) = 0;
    *(unsigned char *)(c + 0x62f8) = 0;
    *(int *)(c + 0x62d8) = 0;
    *(short *)(c + 0x62f0) = 0;
    *(unsigned char *)(c + 0x62f9) = 0;
    *(unsigned char *)(c + 0x62fb) = 0;
    *(unsigned char *)(c + 0x62fc) = 0;
    func_ov004_020adb1c(0);
    func_ov006_020d669c(c);
    func_ov006_020d63ac(c);
    func_ov006_020d6084(c);
    func_ov006_020d5dd4(c);
    func_ov006_020d5c60(c);
    func_ov006_020d5b00(c);
}
}

/* Slot 18 override of dScMgBase_c::OnYoshiTryEat(int); the signature
 * repeats the base declaration exactly, or mwcc appends a slot instead of
 * overriding. */
// @symbol _ZN14dScMgBomroom_c13OnYoshiTryEatEi
void dScMgBomroom_c::OnYoshiTryEat(int /* arg */)
{
    unsigned char *c = (unsigned char *)this;

    func_ov006_020d907c(c);
    unsigned char *a = c + 0x6200;
    unsigned char *b = c + 0x6000;
    *(unsigned short *)(a + 0xee) = 0;
    *(int *)(b + 0x2d0) = 0;
    G2x::SetBlendAlpha((volatile u16 *)0x4000050, 1, 0x1c, 4, 3);
    SetBg0Offset(0, 0);
}

// @symbol _ZN14dScMgBomroom_c6RenderEv
s32 dScMgBomroom_c::Render()
{
    func_ov006_020d5dfc();
    func_ov006_020d6098(this);
    func_ov006_020d5e5c((char *)this);
    func_ov006_020d672c(this);
    func_ov006_020d7524(this);
    func_ov006_020d63d4(this);
    func_ov006_020d5c88(this);
    func_ov006_020d5ab0(this);
    return 1;
}

// @symbol _ZN14dScMgBomroom_c8BehaviorEv
/* Waits out the +0x62f0 delay, then runs the round state at +0x62d0 from
 * the pointer-to-member table data_ov006_021416e0 and steps the sprite
 * records and the BG2 offset. */
struct C_91b0;
typedef void (C_91b0::*PMF_91b0)();
extern "C" PMF_91b0 data_ov006_021416e0[];
s32 dScMgBomroom_c::Behavior()
{
    char *c = (char *)this;
    if (*(unsigned short *)(c + 0x6200 + 0xf0) != 0) {
        unsigned short *t = (unsigned short *)(((int)c + 0x62f0));
        *t = *t - 1;
        if (*(short *)(c + 0x6200 + 0xf0) <= 0)
            *(short *)(c + 0x6200 + 0xf0) = 0;
    } else {
        (((C_91b0 *)this)->*data_ov006_021416e0[*(int *)(c + 0x6000 + 0x2d0)])();
        func_ov006_020d5d08(c);
        func_ov006_020d5b10(c);
    }
    return 1;
}

// @symbol _ZN14dScMgBomroom_c13InitResourcesEv
/* Loads the main- and sub-screen backgrounds, palettes and object
 * graphics, sets the blend, then resets the round into state 1. */
s32 dScMgBomroom_c::InitResources()
{
    char *c = (char *)this;
    char *b;
    volatile u16 sp4;
    int f;
    int r5;

    data_0209d45c |= 8;
    *(volatile u16 *)0x400000e = (*(volatile u16 *)0x400000e & ~3) | 2;
    *(volatile u16 *)0x400000e = (*(volatile u16 *)0x400000e & 0x43) | 0x1210;

    f = LoadFile(0x23);
    DecompressLZ16((void *)f, (void *)(func_02054d88() + 0x4000));
    Deallocate((void *)f);

    f = LoadFile(0x24);
    _ZN2GX10LoadBGPlttEPKvjj((const void *)f, 0x180, 0x80);
    Deallocate((void *)f);

    f = LoadFile(0x25);
    func_02056314((void *)f, 0, 0x800);
    Deallocate((void *)f);

    data_0209d45c |= 4;
    *(volatile u16 *)0x400000c = (*(volatile u16 *)0x400000c & ~3) | 1;
    *(volatile u16 *)0x400000c = (*(volatile u16 *)0x400000c & 0x43) | 0x9410;

    f = LoadFile(3);
    b = (char *)_ZN2G212GetBG2ScrPtrEv();
    sp4 = 0xf23f;
    MultiStore16(sp4, b, 0x1000);
    func_020563d4((const void *)f, 0, 0x800);
    Deallocate((void *)f);

    data_0209d45c |= 1;
    *(volatile u16 *)0x4000008 = (*(volatile u16 *)0x4000008 & ~3);
    *(volatile u16 *)0x4000008 = (*(volatile u16 *)0x4000008 & 0x43) | 0x5610;

    f = LoadFile(2);
    func_02056554((const void *)f, 0, 0x800);
    Deallocate((void *)f);

    f = LoadFile(7);
    func_02056554((const void *)f, 0x800, 0x800);
    Deallocate((void *)f);

    G2x::SetBlendAlpha((volatile u16 *)0x4000050, 1, 0x1c, 4, 3);

    r5 = LoadFile(0xb5);
    f = LoadFile(0xb6);
    DecompressLZ16((void *)r5, (void *)0x6400000);
    _ZN2GX11LoadOBJPlttEPKvjj((const void *)f, 0, 0x100);

    data_0209d454 |= 8;
    *(volatile u16 *)0x400100e = (*(volatile u16 *)0x400100e & ~3) | 1;
    *(volatile u16 *)0x400100e = (*(volatile u16 *)0x400100e & 0x43) | 0x210;

    {
        int f7 = LoadFile(0x23);
        DecompressLZ16((void *)f7, (void *)(_ZN3G2S13GetBG3CharPtrEv() + 0x4000));
        Deallocate((void *)f7);

        f7 = LoadFile(0x24);
        _ZN3GXS10LoadBGPlttEPKvjj((const void *)f7, 0x180, 0x80);
        Deallocate((void *)f7);

        f7 = LoadFile(0x22);
        func_020562b4((const void *)f7, 0, 0x800);
        Deallocate((void *)f7);

        DecompressLZ16((void *)r5, (void *)0x6600000);
        _ZN3GXS11LoadOBJPlttEPKvjj((const void *)f, 0, 0x100);
        Deallocate((void *)r5);
        Deallocate((void *)f);
    }

    func_ov006_020d907c(c);
    func_ov006_020d6630(c);
    func_ov006_020d62e0(c);
    func_ov006_020d604c(c);
    *(int *)(c + 0x6000 + 0x2d0) = 1;
    *(u16 *)(c + 0x6200 + 0xee) = 0;
    func_ov004_020b04d0(0x20);
    func_ov004_020adb1c(0);
    return 1;
}
