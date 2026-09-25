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
 * This TU holds 41 of the class's helpers (.text 0x020d5eb8..0x020d7c4c);
 * the class's methods live in their own files. Functions run in ROM order
 * under `#pragma defer_codegen off`; do not reorder. cstd::atan2 takes
 * Fix12 by value, so its call stays mangled.
 *
 * Leftover, measured: a state field (and &state) differs from
 * (u8 *)(bomb + 0x4697) in func_ov006_020d68a8, and that function's
 * idle-bomb timer store must stay raw + i*0x40 + 0x4690. &f3c differs
 * from (unsigned char *)(int)(bomb + 0x469c) in func_ov006_020d7604.
 */

#include "types.h"
#include "decl_common.h"
#include "Sound.h"

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
extern "C" {
class C_6278;
typedef void (C_6278::*PMF_6278)(int);
class C_6278 { public: int dummy; };
struct Row_6278 { u8 d[0x10]; };
extern "C" PMF_6278 data_ov006_021416c0[];
extern "C" void func_ov006_020d6278(C_6278 *self)
{
    Row_6278 *rows = (Row_6278 *)self;
    for (int i = 0; i < 2; i++) {
        if (rows[i].d[0x628d]) {
            u8 state = rows[i].d[0x628c];
            (self->*data_ov006_021416c0[state])(i);
        }
    }
}
}

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
extern "C" {
void func_ov006_020d63ac(char (*rows)[16]){
  int i;
  for(i=0;i<2;i++){ rows[i][0x628d]=0; rows[i][0x628e]=0; }
}
}

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
extern "C" {
class C_65c8;
typedef void (C_65c8::*PMF_65c8)(int);
class C_65c8 { public: int dummy; };
struct Row_65c8 { u8 d[0x10]; };
extern "C" PMF_65c8 data_ov006_02141680[];
extern "C" void func_ov006_020d65c8(C_65c8 *self)
{
    Row_65c8 *rows = (Row_65c8 *)self;
    for (int i = 0; i < 2; i++) {
        if (rows[i].d[0x626d]) {
            u8 state = rows[i].d[0x626c];
            (self->*data_ov006_02141680[state])(i);
        }
    }
}
}

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
extern "C" {
void func_ov006_020d669c(char (*rows)[16]){
  int i;
  for(i=0;i<2;i++){ rows[i][0x626d]=0; rows[i][0x626e]=0; }
}
}

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
extern u8 data_020a0e40;
extern Tab data_020a0de8[];

void func_ov006_020d6d7c(char *raw, int index) {
    u8 touch = data_020a0e40;
    int touching = 0;
    char *bomb;
    int dx, dy;
    if (data_020a0de8[touch].f0 != 0) {
        if (data_020a0de8[touch].f1 != 0) touching = 1;
    }
    if (touching == 0) return;
    if (*(u8*)(raw + 0x62f6) != 0xff) return;
    bomb = raw + index * 0x40;
    dx = data_020a0de8[touch].f2 - (((Bomb *)(bomb + 0x4660))->x >> 0xc);
    dy = data_020a0de8[touch].f3 - (((Bomb *)(bomb + 0x4660))->y >> 0xc);
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
