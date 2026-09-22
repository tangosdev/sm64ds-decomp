//cpp
/* Reconstructed translation unit (PARTIAL -- 41 of the run's 80 functions).
 * ov006/dScMgBomroom_c   .text 0x020d5eb8 .. 0x020d7c4c
 *
 * FUNCTION ORDER IS ROM-ASCENDING, under `#pragma defer_codegen off`.
 *
 * Assembled from these legacy one-function sources (ROM address order):
 *   [14] 0x020d5eb8  src/func_ov006_020d5eb8.c
 *   [15] 0x020d5f28  src/func_ov006_020d5f28.c
 *   [16] 0x020d5f2c  src/func_ov006_020d5f2c.c
 *   [17] 0x020d5fd8  src/func_ov006_020d5fd8.c
 *   [18] 0x020d5fec  src/func_ov006_020d5fec.cpp
 *   [19] 0x020d604c  src/func_ov006_020d604c.c
 *   [20] 0x020d6084  src/func_ov006_020d6084.cpp
 *   [21] 0x020d6098  src/func_ov006_020d6098.c
 *   [22] 0x020d6100  src/func_ov006_020d6100.c
 *   [23] 0x020d6170  src/func_ov006_020d6170.c
 *   [24] 0x020d61dc  src/func_ov006_020d61dc.c
 *   [25] 0x020d6264  src/func_ov006_020d6264.c
 *   [26] 0x020d6278  src/func_ov006_020d6278.cpp
 *   [27] 0x020d62e0  src/func_ov006_020d62e0.c
 *   [28] 0x020d634c  src/func_ov006_020d634c.c
 *   [29] 0x020d63ac  src/func_ov006_020d63ac.c
 *   [30] 0x020d63d4  src/func_ov006_020d63d4.c
 *   [31] 0x020d6454  src/func_ov006_020d6454.c
 *   [32] 0x020d64c4  src/func_ov006_020d64c4.c
 *   [33] 0x020d64c8  src/func_ov006_020d64c8.c
 *   [34] 0x020d65b4  src/func_ov006_020d65b4.c
 *   [35] 0x020d65c8  src/func_ov006_020d65c8.cpp
 *   [36] 0x020d6630  src/func_ov006_020d6630.c
 *   [37] 0x020d669c  src/func_ov006_020d669c.c
 *   [38] 0x020d66c4  src/func_ov006_020d66c4.cpp
 *   [39] 0x020d672c  src/func_ov006_020d672c.c
 *   [40] 0x020d6784  src/func_ov006_020d6784.c
 *   [41] 0x020d68a8  src/func_ov006_020d68a8.c
 *   [42] 0x020d69b8  src/func_ov006_020d69b8.c
 *   [43] 0x020d6b88  src/func_ov006_020d6b88.c
 *   [44] 0x020d6c90  src/func_ov006_020d6c90.c
 *   [45] 0x020d6d7c  src/func_ov006_020d6d7c.c
 *   [46] 0x020d6e8c  src/func_ov006_020d6e8c.c
 *   [47] 0x020d7524  src/func_ov006_020d7524.c
 *   [48] 0x020d7604  src/func_ov006_020d7604.c
 *   [49] 0x020d7778  src/func_ov006_020d7778.c
 *   [50] 0x020d777c  src/func_ov006_020d777c.c
 *   [51] 0x020d7958  src/func_ov006_020d7958.c
 *   [52] 0x020d795c  src/func_ov006_020d795c.c
 *   [53] 0x020d7a84  src/func_ov006_020d7a84.c
 *   [54] 0x020d7c00  src/func_ov006_020d7c00.cpp
 */

/* Partial source reconstruction: these 41 entries remain C-linkage helpers.
 * The remaining 39 entries, including the class's lifecycle/key function,
 * lie outside this licensed slice. Existing bomb fields are used where they fit; the UI state
 * blocks, PMF receiver views and many scene fields still need reconstruction.
 * Keep defer_codegen and the measured positional optimization brackets. */

#include "dScMgBomroom_c.h"
#include "types.h"
#include "decl_common.h"

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

namespace Sound { unsigned int PlayBank2_2D(unsigned int); }

#pragma defer_codegen off


/* ---------------------------------------------------------------- */
/* ROM ordinal 14 -- func_ov006_020d5eb8, 0x020d5eb8, size 0x70 */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020d5eb8
extern "C" {
void func_ov006_020d5eb8(unsigned char* base, int idx) {
    unsigned char* p = base + (idx << 4);
    int o = idx << 4;
    if (*(unsigned char*)(p + 0x62ae) != 0) {
        *(unsigned short*)(base + 0x62a8 + o) =
            *(unsigned short*)(base + 0x62a8 + o) + 1;
        if (*(unsigned short*)(p + 0x62a8) < 3) return;
        *(unsigned short*)(p + 0x62a8) = 0;
        *(unsigned char*)(base + 0x62ae + o) =
            *(unsigned char*)(base + 0x62ae + o) - 1;
    } else {
        *(unsigned char*)(p + 0x62ac) = 0;
    }
}
}

/* ---------------------------------------------------------------- */
/* ROM ordinal 15 -- func_ov006_020d5f28, 0x020d5f28, size 0x4 */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020d5f28
extern "C" {
void func_ov006_020d5f28(void)
{
}
}

/* ---------------------------------------------------------------- */
/* ROM ordinal 16 -- func_ov006_020d5f2c, 0x020d5f2c, size 0xac */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020d5f2c
extern "C" {
void func_ov006_020d5f2c(char *sceneBytes, int slot)
{
  char *slotBase;
  char *stateBase;
  unsigned char level;
  slotBase = sceneBytes + (slot * 0x10);
  if (((unsigned char) (*((unsigned char *) ((slotBase + 0x6000) + 0x2ae)))) >= 4)
  {
    *((unsigned char *) ((slotBase + 0x6000) + 0x2ae)) = 4;
    *((unsigned char *) ((slotBase + 0x6000) + 0x2ac)) = 2;
    return;
  }
  {
    unsigned short *timer = (unsigned short *) ((sceneBytes + 0x62a8) + (slot * 0x10));
    *timer = (*timer) + 1;
  }
  if ((*((unsigned short *) (slotBase + 0x62a8))) < 3)
  {
    return;
  }
  *((unsigned short *) (slotBase + 0x62a8)) = 0;
  stateBase = slotBase + 0x6000;
  {
    unsigned char *levelField = (unsigned char *) ((sceneBytes + 0x62ae) + (slot * 0x10));
    *levelField = (*levelField) + 1;
  }
  level = (unsigned char) (*((unsigned char *) (stateBase + 0x2ae)));
  if (level >= 4)
  {
    *((unsigned char *) ((slotBase + 0x6000) + 0x2ae)) = 4;
    *((unsigned char *) (stateBase + 0x2ac)) = 2;
  }
}
}

/* ---------------------------------------------------------------- */
/* ROM ordinal 17 -- func_ov006_020d5fd8, 0x020d5fd8, size 0x14 */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020d5fd8
extern "C" {
typedef struct { char pad[0xa8]; short f; } BrEnt5fd8;
void func_ov006_020d5fd8(int c, int i){
  BrEnt5fd8 *p = (BrEnt5fd8*)(c + (i<<4) + 0x6200);
  p->f = 0;
}
}

/* ---------------------------------------------------------------- */
/* ROM ordinal 18 -- func_ov006_020d5fec, 0x020d5fec, size 0x60 */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020d5fec
extern "C" {
struct C_5fec; typedef void (C_5fec::*PMF_5fec)(int);
struct Entry_5fec { PMF_5fec pmf; };
extern "C" Entry_5fec data_ov006_02141660[];
struct C_5fec { char pad[0x62ac]; unsigned char idx; unsigned char guard; };
extern "C" void func_ov006_020d5fec(C_5fec* c) {
    if (!c->guard) return;
    (c->*(data_ov006_02141660[c->idx].pmf))(0);
}
}

/* ---------------------------------------------------------------- */
/* ROM ordinal 19 -- func_ov006_020d604c, 0x020d604c, size 0x38 */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020d604c
extern "C" {
void func_ov006_020d604c(void *p){
  char* sceneBytes = (char*)p;
  *(unsigned char*)(sceneBytes+0x62ad)=1;
  *(unsigned char*)(sceneBytes+0x62ac)=0;
  *(unsigned char*)(sceneBytes+0x62ae)=0;
  *(unsigned char*)(sceneBytes+0x62af)=1;
  *(short*)(sceneBytes+0x62a8)=0;
  *(int*)(sceneBytes+0x62a0)=0x80000;
  *(int*)(sceneBytes+0x62a4)=0x8000;
}
}

/* ---------------------------------------------------------------- */
/* ROM ordinal 20 -- func_ov006_020d6084, 0x020d6084, size 0x14 */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020d6084
extern "C" {
extern "C" void func_ov006_020d6084(char *self)
{
    *(char *)(self + 0x62ad) = 0;
    *(char *)(self + 0x62af) = 0;
}
}

/* ---------------------------------------------------------------- */
/* ROM ordinal 21 -- func_ov006_020d6098, 0x020d6098, size 0x68 */
/* ---------------------------------------------------------------- */
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
void func_ov006_020d6098(void *sceneData) {
    Obj_6098 *sceneView = (Obj_6098 *)sceneData;
    int slot;
    for (slot = 0; slot < 2; slot++) {
        int x = sceneView->recs[slot].x >> 12;
        int y = sceneView->recs[slot].y >> 12;
        int spriteIndex = sceneView->recs[slot].idx;
        if (slot != 0) spriteIndex += 5;
        Hud_RenderSprite((void*)data_ov006_021344ec[spriteIndex], x, y, -1, -1);
    }
}
}

/* ---------------------------------------------------------------- */
/* ROM ordinal 22 -- func_ov006_020d6100, 0x020d6100, size 0x70 */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020d6100
extern "C" {
void func_ov006_020d6100(unsigned char* base, int idx) {
    unsigned char* p = base + (idx << 4);
    int o = idx << 4;
    if (*(unsigned char*)(p + 0x628f) != 0) {
        *(unsigned short*)(base + 0x6288 + o) =
            *(unsigned short*)(base + 0x6288 + o) + 1;
        if (*(unsigned short*)(p + 0x6288) < 4) return;
        *(unsigned short*)(p + 0x6288) = 0;
        *(unsigned char*)(base + 0x628f + o) =
            *(unsigned char*)(base + 0x628f + o) - 1;
    } else {
        *(unsigned char*)(p + 0x628c) = 0;
    }
}
}

/* ---------------------------------------------------------------- */
/* ROM ordinal 23 -- func_ov006_020d6170, 0x020d6170, size 0x6c */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020d6170
extern "C" {
void func_ov006_020d6170(char* c, int idx)
{
    char* base = c + (idx << 4);
    unsigned short* t = (unsigned short*)(base + 0x6288);

    if (*t != 0) {
        unsigned short* p = (unsigned short*)(c + 0x6288 + (idx << 4));
        *p = (unsigned short)(*p - 1);
        if (*(short*)(base + 0x6288) < 0)
            *(short*)(base + 0x6288) = 0;
        return;
    }

    *(unsigned char*)(base + 0x628c) = 3;
}
}

/* ---------------------------------------------------------------- */
/* ROM ordinal 24 -- func_ov006_020d61dc, 0x020d61dc, size 0x88 */
/* ---------------------------------------------------------------- */
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
void func_ov006_020d61dc(char* c, int index)
{
    Work_61dc* w = (Work_61dc*)c;
    w->slots[index].timer++;
    if (w->slots[index].timer < 5)
        return;
    w->slots[index].timer = 0;
    w->slots[index].level++;
    if (w->slots[index].level >= 4) {
        w->slots[index].state = 2;
        w->slots[index].timer = 0x20;
    }
}
}

/* ---------------------------------------------------------------- */
/* ROM ordinal 25 -- func_ov006_020d6264, 0x020d6264, size 0x14 */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020d6264
extern "C" {
typedef struct { char pad[0x88]; short f; } BrEnt6264;
void func_ov006_020d6264(int c, int i){
  BrEnt6264 *p = (BrEnt6264*)(c + (i<<4) + 0x6200);
  p->f = 0;
}
}

/* ---------------------------------------------------------------- */
/* ROM ordinal 26 -- func_ov006_020d6278, 0x020d6278, size 0x68 */
/* ---------------------------------------------------------------- */
/* The two 0x10-stride UI entries are addressed within scene storage. Four
 * helpers (6278, 63ac, 65c8, 669c) previously indexed past a 16-byte subarray.
 * Their scoped byte-base forms need strength reduction off: otherwise the
 * dispatchers grow by 0x10 and the clearing loops by 8 bytes under 2004/b56.
 * These brackets preserve the existing linked code without asserting a full
 * UI-entry class layout. The PMF receiver classes remain opaque. */
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

/* ---------------------------------------------------------------- */
/* ROM ordinal 27 -- func_ov006_020d62e0, 0x020d62e0, size 0x6c */
/* ---------------------------------------------------------------- */
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

/* ---------------------------------------------------------------- */
/* ROM ordinal 28 -- func_ov006_020d634c, 0x020d634c, size 0x60 */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020d634c
extern "C" {
void func_ov006_020d634c(char *c, int i)
{
    char *p = c + i * 16;
    unsigned char st = *(unsigned char *)(p + 0x628c);
    if (st != 0) {
        if (st == 2) {
            *(short *)(p + 0x6288) = 0x20;
        } else {
            *(unsigned char *)(p + 0x628c) = 1;
        }
        return;
    }
    *(unsigned char *)(p + 0x628c) = 1;
    SetBg0Offset(0x100, 0);
}
}

/* ---------------------------------------------------------------- */
/* ROM ordinal 29 -- func_ov006_020d63ac, 0x020d63ac, size 0x28 */
/* ---------------------------------------------------------------- */
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

/* ---------------------------------------------------------------- */
/* ROM ordinal 30 -- func_ov006_020d63d4, 0x020d63d4, size 0x80 */
/* ---------------------------------------------------------------- */
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
void func_ov006_020d63d4(void *sceneData) {
    Obj_63d4 *sceneView = (Obj_63d4 *)sceneData;
    int slot;
    for (slot = 0; slot < 2; slot++) {
        if (sceneView->recs[slot].flag != 0) {
            int x = sceneView->recs[slot].x >> 12;
            int y = sceneView->recs[slot].y >> 12;
            int frame = sceneView->recs[slot].idx;
            int spriteHandle = (slot == 0) ? data_ov006_021343b0[frame] : data_ov006_021342bc[frame];
            Hud_RenderSprite((void*)spriteHandle, x, y, -1, 1);
        }
    }
}
}

/* ---------------------------------------------------------------- */
/* ROM ordinal 31 -- func_ov006_020d6454, 0x020d6454, size 0x70 */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020d6454
extern "C" {
void func_ov006_020d6454(unsigned char* base, int idx) {
    unsigned char* p = base + (idx << 4);
    if (*(unsigned char*)(p + 0x626f) != 0) {
        unsigned short* c = (unsigned short*)(base + 0x6268 + (idx << 4));
        *c = *c + 1;
        if (*(unsigned short*)(p + 0x6268) < 3) return;
        *(unsigned short*)(p + 0x6268) = 0;
        *(unsigned char*)(base + 0x626f + (idx << 4)) =
            *(unsigned char*)(base + 0x626f + (idx << 4)) - 1;
    } else {
        *(unsigned char*)(p + 0x626c) = 0;
    }
}
}

/* ---------------------------------------------------------------- */
/* ROM ordinal 32 -- func_ov006_020d64c4, 0x020d64c4, size 0x4 */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020d64c4
extern "C" {
void func_ov006_020d64c4(void)
{
}
}

/* ---------------------------------------------------------------- */
/* ROM ordinal 33 -- func_ov006_020d64c8, 0x020d64c8, size 0xec */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020d64c8
#pragma push
#pragma opt_common_subs off
#pragma opt_strength_reduction off
extern "C" {
void func_ov006_020d64c8(char *o, int i)
{
    *(u16 *)(o + 0x6268 + i * 16) += 1;
    if (*(u16 *)(o + i * 16 + 0x6268) < 5)
        return;
    *(u16 *)(o + i * 16 + 0x6268) = 0;
    *(u8 *)(o + 0x626f + i * 16) += 1;
    if (i == 0) {
        u8 v = *(u8 *)(o + i * 16 + 0x626f);
        if (v == 2 || v == 4) {
            int *w = (int *)(((int)o + 0x62b4));
            *w += 0x8000;
        }
    }
    if (*(u8 *)(o + i * 16 + 0x626f) == 3 && i == 1)
        func_ov006_020d5d90(o, i);
    if (*(u8 *)(o + i * 16 + 0x626f) >= 4)
        *(u8 *)(o + i * 16 + 0x626c) = 2;
}
}
#pragma pop

/* ---------------------------------------------------------------- */
/* ROM ordinal 34 -- func_ov006_020d65b4, 0x020d65b4, size 0x14 */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020d65b4
extern "C" {
typedef struct { char pad[0x68]; short f; } BrEnt65b4;
void func_ov006_020d65b4(int c, int i){
  BrEnt65b4 *p = (BrEnt65b4*)(c + (i<<4) + 0x6200);
  p->f = 0;
}
}

/* ---------------------------------------------------------------- */
/* ROM ordinal 35 -- func_ov006_020d65c8, 0x020d65c8, size 0x68 */
/* ---------------------------------------------------------------- */
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

/* ---------------------------------------------------------------- */
/* ROM ordinal 36 -- func_ov006_020d6630, 0x020d6630, size 0x6c */
/* ---------------------------------------------------------------- */
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

/* ---------------------------------------------------------------- */
/* ROM ordinal 37 -- func_ov006_020d669c, 0x020d669c, size 0x28 */
/* ---------------------------------------------------------------- */
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

/* ---------------------------------------------------------------- */
/* ROM ordinal 38 -- func_ov006_020d66c4, 0x020d66c4, size 0x68 */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020d66c4
extern "C" {
extern "C" void func_ov006_020d66c4(char *base, int idx) {
    char *ip = base + idx * 16;
    if (*(unsigned char*)(ip + 0x6000 + 0x26c) != 0) return;
    *(unsigned char*)(ip + 0x6000 + 0x26c) = 1;
    Sound::PlayBank2_2D(0x1e2);
    if (idx != 0) return;
    func_ov006_020d5d90(base, idx);
}
}

/* ---------------------------------------------------------------- */
/* ROM ordinal 39 -- func_ov006_020d672c, 0x020d672c, size 0x58 */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020d672c
extern "C" {
typedef struct { char pad[0x2f9]; unsigned char on; } BrFlag672c;
typedef struct { char pad[0xee]; unsigned short v; } BrEnt672c;
void func_ov006_020d672c(void *p){
  int c = (int)p;
  if(((BrFlag672c*)(c+0x6000))->on==0) return;
  func_ov004_020b2444(0x80,0xc,((BrEnt672c*)(c+0x6200))->v,1,-1,0,0);
}
}

/* ---------------------------------------------------------------- */
/* ROM ordinal 40 -- func_ov006_020d6784, 0x020d6784, size 0x124 */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020d6784
#pragma push
#pragma opt_strength_reduction off
extern "C" {
void func_ov006_020d6784(char* sceneBytes)
{
    dScMgBomroom_c* scene = (dScMgBomroom_c*)sceneBytes;
    int leftRoomCount, rightRoomCount, bombIndex;
    rightRoomCount = 0;
    leftRoomCount = 0;
    for (bombIndex = 0; bombIndex < 0x70; bombIndex++) {
        if (scene->mBombs[bombIndex].unk_38 == 0) continue;
        if (scene->mBombs[bombIndex].unk_37 != 5) continue;
        if (scene->mBombs[bombIndex].unk_36 != 0) rightRoomCount++;
        else leftRoomCount++;
    }
    if (leftRoomCount >= 0x28) {
        scene->unk_62d0 = 3;
        *(int *)(sceneBytes + 0x62d4) = 0;
        *(unsigned char *)(sceneBytes + 0x62f8) = 0;
        *(unsigned char *)(sceneBytes + 0x62f5) = 0;
        *(volatile u32*)&scene->unk_0bc = scene->unk_0bc + 1;
        if (scene->unk_0bc > 0x270e) scene->unk_0bc = 0x270e;
        *(unsigned char *)(sceneBytes + 0x62fb) = 1;
        Sound::PlayBank2_2D(0x1e6);
    } else if (rightRoomCount >= 0x28) {
        scene->unk_62d0 = 3;
        *(int *)(sceneBytes + 0x62d4) = 0;
        *(unsigned char *)(sceneBytes + 0x62f8) = 0;
        *(unsigned char *)(sceneBytes + 0x62f5) = 1;
        *(volatile u32*)&scene->unk_0bc = scene->unk_0bc + 1;
        if (scene->unk_0bc > 0x270e) scene->unk_0bc = 0x270e;
        *(unsigned char *)(sceneBytes + 0x62fb) = 1;
        Sound::PlayBank2_2D(0x1e6);
    }
}
}
#pragma pop

/* ---------------------------------------------------------------- */
/* ROM ordinal 41 -- func_ov006_020d68a8, 0x020d68a8, size 0x110 */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020d68a8
#pragma push
#pragma opt_strength_reduction off
#pragma opt_common_subs off
extern "C" {
void func_ov006_020d68a8(char *o, int a1)
{
    int i;
    u8 *cur = (u8 *)(((int)(o + a1 * 0x40) + 0x469a));
    for (i = 0; i < 0x70; i++) {
        char *q = o + i * 0x40;
        if (*(u8 *)(q + 0x4698) != 0) {
            if (i != a1) {
                u8 n = *cur;
                if (n == 0) {
                    u8 *st = (u8 *)(((int)q + 0x4697));
                    if (*st != 5) {
                        *st = 3;
                        *(u16 *)(q + 0x4690) = 0x20;
                    }
                } else {
                    u8 *st = (u8 *)(((int)q + 0x4697));
                    if (*st == 5) {
                        if (*(u8 *)(q + 0x4696) + 1 == n) {
                            *st = 3;
                            *(u16 *)(q + 0x4690) = 0x20;
                        }
                    }
                    if (*st <= 1) {
                        *st = 3;
                        *(u16 *)(o + i * 0x40 + 0x4690) = 0x20;
                    }
                }
            }
        }
    }
    if (*(u8 *)(o + 0x62f8) != 0)
        return;
    *(u8 *)(o + 0x62f8) = *(u8 *)(o + a1 * 0x40 + 0x469a) + 1;
    *(u16 *)(o + 0x62e8) = 0x60;
    *(int *)(o + 0x62d0) = 3;
    *(int *)(o + 0x62d4) = 0;
}
}
#pragma pop

/* ---------------------------------------------------------------- */
/* ROM ordinal 42 -- func_ov006_020d69b8, 0x020d69b8, size 0x1d0 */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020d69b8
extern "C" {
struct Ent_69b8 {
    int f660;
    char pad1[0x20];
    int f684;
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
struct Gbl_69b8 {
    char pad0[0x2d0];
    int f2d0;
};
#define E_69b8 (((struct Ent_69b8 *)(a + 0x4000 + 0x660))[i])
#define G_69b8 ((struct Gbl_69b8 *)(a + 0x6000))
void func_ov006_020d69b8(char *a, int i)
{
    int type;
    int frame;
    int max;

    *(unsigned short *)(a + 0x468e + (i << 6)) += 1;
    type = E_69b8.f694;
    frame = E_69b8.f695;

    if (type != 3 && type != 0 && G_69b8->f2d0 != 3) {
        E_69b8.f684 = func_02012468(E_69b8.f684, 2, 0x1db, 4, 0, 0,
                               func_020126e8(E_69b8.f660), 0);
    }

    if (E_69b8.f68e >= data_ov006_0213bb08[type][frame]) {
        frame++;
        E_69b8.f695 = frame;
        E_69b8.f68e = 0;
        if (type != 3 && E_69b8.f697 != 2 && (type != 0 || E_69b8.f697 != 5)) {
            if (E_69b8.f698 == 1 && G_69b8->f2d0 != 3) {
                if (E_69b8.f696 != 0) {
                    func_02012718((int)0x1d9, E_69b8.f660);
                } else {
                    func_02012718((int)0x1da, E_69b8.f660);
                }
            }
        }
        if (type == 3 && frame == 1) {
            func_02012718((int)0x1e1, E_69b8.f660);
        }
    }

    max = data_ov006_0213b9bc[type];
    if (frame < max) {
        return;
    }
    if (type == 3) {
        E_69b8.f698 = 0;
        E_69b8.f699 = 0;
    } else {
        max = 0;
    }
    E_69b8.f695 = max;
}
}

/* ---------------------------------------------------------------- */
/* ROM ordinal 43 -- func_ov006_020d6b88, 0x020d6b88, size 0x108 */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020d6b88
extern "C" {

void func_ov006_020d6b88(char *sceneBytes, int bombIndex)
{
    char *indexedScene = sceneBytes + bombIndex * 0x40;
    u8 room = *(u8*)(indexedScene + 0x4696);
    int x = *(int*)(indexedScene + 0x4660) >> 0xc;
    int y = *(int*)(indexedScene + 0x4664) >> 0xc;
    if (room != 0) {
        if (x <= 0xc0) return;
        if (y <= 0x40) return;
        if (y >= 0x80) return;
        *(u8*)(indexedScene + 0x4697) = 5;
        *(u8*)(indexedScene + 0x4694) = 0;
        *(u8*)(indexedScene + 0x4695) = 0;
        *(u16*)(indexedScene + 0x468e) = 0;
        *(int*)(indexedScene + 0x4670) = 0x999;
        func_02012718((int)0x1dc, *(int*)(indexedScene + 0x4660));
    } else {
        if (x >= 0x40) return;
        if (y <= 0x40) return;
        if (y >= 0x80) return;
        *(u8*)(indexedScene + 0x4697) = 5;
        *(u8*)(indexedScene + 0x4694) = 0;
        *(u8*)(indexedScene + 0x4695) = 0;
        *(u16*)(indexedScene + 0x468e) = 0;
        *(int*)(indexedScene + 0x4670) = 0x999;
        func_02012718((int)0x1dc, *(int*)(indexedScene + 0x4660));
    }
}
}

/* ---------------------------------------------------------------- */
/* ROM ordinal 44 -- func_ov006_020d6c90, 0x020d6c90, size 0xec */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020d6c90
extern "C" {
void func_ov006_020d6c90(char *c, int idx)
{
    char *slot = c + (idx << 6);
    unsigned char flag = *(unsigned char *)(slot + 0x4696);
    int x = *(int *)(slot + 0x4660) >> 12;
    int y = *(int *)(slot + 0x4664) >> 12;

    if (flag != 0) {
        if (x >= 0x40)
            return;
        if (y <= 0x40)
            return;
        if (y >= 0x80)
            return;
        *(short *)(slot + 0x4692) = 0;
        *(unsigned char *)(slot + 0x4697) = 4;
        *(unsigned char *)(slot + 0x4694) = 3;
        *(unsigned char *)(slot + 0x4695) = 0;
        *(short *)(slot + 0x468e) = 0;
        *(unsigned char *)(slot + 0x469a) = 1;
        *(unsigned char *)(slot + 0x469d) = 1;
        func_ov006_020d68a8(c, idx);
    } else {
        if (x <= 0xc0)
            return;
        if (y <= 0x40)
            return;
        if (y >= 0x80)
            return;
        *(short *)(slot + 0x4692) = 0;
        *(unsigned char *)(slot + 0x4697) = 4;
        *(unsigned char *)(slot + 0x4694) = 3;
        *(unsigned char *)(slot + 0x4695) = 0;
        *(short *)(slot + 0x468e) = 0;
        *(unsigned char *)(slot + 0x469a) = 2;
        *(unsigned char *)(slot + 0x469d) = 1;
        func_ov006_020d68a8(c, idx);
    }
}
}

/* ---------------------------------------------------------------- */
/* ROM ordinal 45 -- func_ov006_020d6d7c, 0x020d6d7c, size 0x110 */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020d6d7c
#pragma push
#pragma opt_propagation off
extern "C" {

typedef struct { u8 f0, f1, f2, f3; } Tab;
extern u8 data_020a0e40;
extern Tab data_020a0de8[];

void func_ov006_020d6d7c(char *self_, int idx) {
    u8 m = data_020a0e40;
    int hit = 0;
    char *b;
    int dx, dy;
    if (data_020a0de8[m].f0 != 0) {
        if (data_020a0de8[m].f1 != 0) hit = 1;
    }
    if (hit == 0) return;
    if (*(u8*)(self_ + 0x62f6) != 0xff) return;
    b = self_ + idx * 0x40;
    dx = data_020a0de8[m].f2 - (*(int*)(b + 0x4660) >> 0xc);
    dy = data_020a0de8[m].f3 - (*(int*)(b + 0x4664) >> 0xc);
    if (dx > 0xc) return;
    if (dx < -0xc) return;
    if (dy > 0xf) return;
    if (dy < -0xf) return;
    *(u8*)(self_ + 0x62f6) = *(u8*)(b + 0x4696);
    *(u8*)(b + 0x4694) = 1;
    *(u8*)(b + 0x4697) = 2;
    *(int*)(b + 0x4668) = dx << 0xc;
    *(int*)(b + 0x466c) = dy << 0xc;
    *(u8*)(b + 0x469e) = 0;
    func_02012718((int)0x1d2, *(int*)(b + 0x4660));
}
}
#pragma pop

/* ---------------------------------------------------------------- */
/* ROM ordinal 46 -- func_ov006_020d6e8c, 0x020d6e8c, size 0x698 */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020d6e8c
#pragma push
#pragma opt_common_subs off
extern "C" {
void func_ov006_020d6e8c(char* self, int idx)
{
    int x, y, py, dx, dy1, ang, i2, a, dy2, sy, px;

    x = *(int*)(self + (idx << 6) + 0x4660) >> 12;
    y = *(int*)(self + (idx << 6) + 0x4664) >> 12;

    if (x + 12 > 0x100) {
        *(u16*)(self + (idx << 6) + 0x468c) = 0x8000 - *(u16*)(self + (idx << 6) + 0x468c);
        *(int*)(self + (idx << 6) + 0x4660) = 0xF4000;
    } else if (x - 12 < 0) {
        *(u16*)(self + (idx << 6) + 0x468c) = 0x8000 - *(u16*)(self + (idx << 6) + 0x468c);
        *(int*)(self + (idx << 6) + 0x4660) = 0xC000;
    }

    if (y + 12 > 0xB8) {
        *(u16*)(self + (idx << 6) + 0x468c) = 0 - *(u16*)(self + (idx << 6) + 0x468c);
        *(int*)(self + (idx << 6) + 0x4664) = 0xAC000;
    } else if (y - 12 < 0) {
        *(u16*)(self + (idx << 6) + 0x468c) = 0 - *(u16*)(self + (idx << 6) + 0x468c);
        *(int*)(self + (idx << 6) + 0x4664) = 0xC000;
    }

    px = *(int*)(self + (idx << 6) + 0x4660) >> 12;
    py = *(int*)(self + (idx << 6) + 0x4664) >> 12;

    if (px + 12 > 0xC0 && py + 12 > 0x40 && py - 12 < 0x80) {
        dx = px - 0xC0;
        dy1 = py - 0x40;
        dy2 = 0x80 - py;
        if (px <= 0xC0 && py >= 0x40 && py <= 0x80) {
            *(u16*)(self + (idx << 6) + 0x468c) = 0x8000 - *(u16*)(self + (idx << 6) + 0x468c);
            *(int*)(self + (idx << 6) + 0x4660) = 0xB4000;
        } else if (px > 0xC0 && py < 0x40) {
            *(u16*)(self + (idx << 6) + 0x468c) = 0 - *(u16*)(self + (idx << 6) + 0x468c);
            *(int*)(self + (idx << 6) + 0x4664) = 0x34000;
        } else if (px > 0xC0 && py > 0x80) {
            *(u16*)(self + (idx << 6) + 0x468c) = 0 - *(u16*)(self + (idx << 6) + 0x468c);
            *(int*)(self + (idx << 6) + 0x4664) = 0x8C000;
        } else if (px > 0xC0 && py > 0x40 && py < 0x80) {
            if (dy1 < dy2) {
                if (dx < dy1) {
                    *(u16*)(self + (idx << 6) + 0x468c) = 0x8000 - *(u16*)(self + (idx << 6) + 0x468c);
                    *(int*)(self + (idx << 6) + 0x4660) = 0xB4000;
                } else {
                    *(u16*)(self + (idx << 6) + 0x468c) = 0 - *(u16*)(self + (idx << 6) + 0x468c);
                    *(int*)(self + (idx << 6) + 0x4664) = 0x34000;
                }
            } else {
                if (dx < dy2) {
                    *(u16*)(self + (idx << 6) + 0x468c) = 0x8000 - *(u16*)(self + (idx << 6) + 0x468c);
                    *(int*)(self + (idx << 6) + 0x4660) = 0xB4000;
                } else {
                    *(u16*)(self + (idx << 6) + 0x468c) = 0 - *(u16*)(self + (idx << 6) + 0x468c);
                    *(int*)(self + (idx << 6) + 0x4664) = 0x8C000;
                }
            }
        } else {
            if (py >= 0x60) {
                sy = 0xC0 - px;
            } else {
                dy2 = 0x40 - py;
                sy = 0xC0 - px;
            }
            _ZN4cstd4sqrtEy((s64)(sy * sy + dy2 * dy2));
            ang = (u16)_ZN4cstd5atan2E5Fix12IiES1_(dy2, sy);
            i2 = (ang >> 4) * 2;
            *(int*)(self + (idx << 6) + 0x4660) =
                (0xC0 - (int)(((s64)data_02082214[i2 + 1] * 0xF + 0x800) >> 12)) << 12;
            if (py >= 0x60) {
                *(int*)(self + (idx << 6) + 0x4664) =
                    (0x80 - (int)(((s64)data_02082214[i2] * 0xF + 0x800) >> 12)) << 12;
            } else {
                *(int*)(self + (idx << 6) + 0x4664) =
                    (0x40 - (int)(((s64)data_02082214[i2] * 0xF + 0x800) >> 12)) << 12;
            }
            a = *(u16*)(self + (idx << 6) + 0x468c);
            if (data_02082214[(a >> 4) * 2 + 1] > 0) {
                *(u16*)(self + (idx << 6) + 0x468c) = 0x8000 - a;
            } else {
                *(u16*)(self + (idx << 6) + 0x468c) = 0 - a;
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
        *(u16*)(self + (idx << 6) + 0x468c) = 0x8000 - *(u16*)(self + (idx << 6) + 0x468c);
        *(int*)(self + (idx << 6) + 0x4660) = 0x4C000;
        return;
    }
    if (px < 0x40 && py < 0x40) {
        *(u16*)(self + (idx << 6) + 0x468c) = 0 - *(u16*)(self + (idx << 6) + 0x468c);
        *(int*)(self + (idx << 6) + 0x4664) = 0x34000;
        return;
    }
    if (px < 0x40 && py > 0x80) {
        *(u16*)(self + (idx << 6) + 0x468c) = 0 - *(u16*)(self + (idx << 6) + 0x468c);
        *(int*)(self + (idx << 6) + 0x4664) = 0x8C000;
        return;
    }
    if (px < 0x40 && py > 0x40 && py < 0x80) {
        if (dy1 < sy) {
            if (dy2 < dy1) {
                *(u16*)(self + (idx << 6) + 0x468c) = 0x8000 - *(u16*)(self + (idx << 6) + 0x468c);
                *(int*)(self + (idx << 6) + 0x4660) = 0x4C000;
                return;
            }
            *(u16*)(self + (idx << 6) + 0x468c) = 0 - *(u16*)(self + (idx << 6) + 0x468c);
            *(int*)(self + (idx << 6) + 0x4664) = 0x34000;
            return;
        }
        if (dy2 < sy) {
            *(u16*)(self + (idx << 6) + 0x468c) = 0x8000 - *(u16*)(self + (idx << 6) + 0x468c);
            *(int*)(self + (idx << 6) + 0x4660) = 0x4C000;
            return;
        }
        *(u16*)(self + (idx << 6) + 0x468c) = 0 - *(u16*)(self + (idx << 6) + 0x468c);
        *(int*)(self + (idx << 6) + 0x4664) = 0x8C000;
        return;
    }

    if (py < 0x60) {
        sy = 0x40 - py;
    }
    _ZN4cstd4sqrtEy((s64)(dy2 * dy2 + sy * sy));
    ang = (u16)_ZN4cstd5atan2E5Fix12IiES1_(sy, dy2);
    i2 = (ang >> 4) * 2;
    *(int*)(self + (idx << 6) + 0x4660) =
        (0x40 - (int)(((s64)data_02082214[i2 + 1] * 0xF + 0x800) >> 12)) << 12;
    if (py >= 0x60) {
        *(int*)(self + (idx << 6) + 0x4664) =
            (0x80 - (int)(((s64)data_02082214[i2] * 0xF + 0x800) >> 12)) << 12;
    } else {
        *(int*)(self + (idx << 6) + 0x4664) =
            (0x40 - (int)(((s64)data_02082214[i2] * 0xF + 0x800) >> 12)) << 12;
    }
    a = *(u16*)(self + (idx << 6) + 0x468c);
    if (data_02082214[(a >> 4) * 2 + 1] < 0) {
        *(u16*)(self + (idx << 6) + 0x468c) = 0x8000 - a;
    } else {
        *(u16*)(self + (idx << 6) + 0x468c) = 0 - a;
    }
}
}
#pragma pop

/* ---------------------------------------------------------------- */
/* ROM ordinal 47 -- func_ov006_020d7524, 0x020d7524, size 0xe0 */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020d7524
extern "C" {
typedef struct { int f660; int f664; char pad08[0x2c]; unsigned char b694; unsigned char b695; unsigned char b696; unsigned char b697; unsigned char pad38; unsigned char b699; unsigned char pad3a; unsigned char b69b; char pad3c[4]; } E_7524;
typedef struct { char pad0[0x4660]; E_7524 e[112]; char pad1[0x7c]; int f62dc; } Self_7524;
void func_ov006_020d7524(void *p) {
    Self_7524 *self = (Self_7524 *)p;
    int i;
    for (i = 0; i < 0x70; i++) {
        if (self->e[i].b699 == 0) continue;
        {
            int b694 = self->e[i].b694;
            int a4 = 1;
            int b695, b696;
            unsigned char idx;
            int b697;
            int f664;
            int a1, a2;
            if (b694 == 3) a4 = 0;
            b696 = self->e[i].b696;
            b695 = self->e[i].b695;
            if (b696 != 0) { idx = data_ov006_0213bb28[b694][b695]; }
            else { idx = data_ov006_0213bb18[b694][b695]; }
            b697 = self->e[i].b697;
            a1 = self->e[i].f660 >> 12;
            f664 = self->e[i].f664;
            a2 = f664 >> 12;
            if (b697 == 6) {
                if (self->e[i].b69b == 4) { a2 = (f664 - self->f62dc) >> 12; }
            }
            func_ov004_020afdd0(data_ov006_0213bb4c[idx], a1, a2, -1, a4);
        }
    }
}
}

/* ---------------------------------------------------------------- */
/* ROM ordinal 48 -- func_ov006_020d7604, 0x020d7604, size 0x174 */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020d7604
#pragma push
#pragma opt_strength_reduction off
#pragma opt_common_subs off
extern "C" {
void func_ov006_020d7604(void *sceneData)
{
    unsigned char *base = (unsigned char *)sceneData;
    int rightRoomCount = 0;
    int leftRoomCount = 0;
    int displaySlot = 0;
    int bombIndex;

    for (bombIndex = 0; bombIndex < 0x70; bombIndex++) {
        unsigned char *p = base + bombIndex * 64;
        if (p[0x4698] != 0) {
            if (base[0x62f8] == 1) {
                if (p[0x4696] != 0) {
                    int column;
                    int row;
                    p[0x469b] = 3;
                    column = rightRoomCount;
                    row = 0;
                    *(unsigned short *)(p + 0x4690) = (unsigned short)(displaySlot * 8);
                    while (column >= 5) { column -= 5; row++; }
                    ((unsigned char *)(int)(base + bombIndex * 64))[0x469c] = (unsigned char)(row * 10 + column);
                    rightRoomCount++;
                    displaySlot++;
                } else {
                    int column;
                    int row;
                    unsigned char *slotField = (unsigned char *)(int)(p + 0x469c);
                    *slotField = (unsigned char)leftRoomCount;
                    p[0x469b] = 3;
                    column = leftRoomCount;
                    *(unsigned short *)(p + 0x4690) = (unsigned short)(displaySlot * 8);
                    row = 0;
                    while (column >= 5) { column -= 5; row++; }
                    *slotField = (unsigned char)(row * 10 + column + 5);
                    leftRoomCount++;
                    displaySlot++;
                }
            } else if (base[0x62f8] != 0) {
                p[0x469c] = (unsigned char)displaySlot;
                p[0x469b] = 3;
                *(unsigned short *)(p + 0x4690) = (unsigned short)(displaySlot * 8);
                displaySlot++;
            } else {
                if (base[0x62f5] == p[0x4696] && p[0x4697] == 6) {
                    p[0x469c] = (unsigned char)displaySlot;
                    p[0x469b] = 3;
                    *(unsigned short *)(p + 0x4690) = (unsigned short)(displaySlot * 8);
                    displaySlot++;
                }
            }
        }
    }
    func_ov006_020d5e3c(sceneData);
}
}
#pragma pop

/* ---------------------------------------------------------------- */
/* ROM ordinal 49 -- func_ov006_020d7778, 0x020d7778, size 0x4 */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020d7778
extern "C" {
void func_ov006_020d7778(void)
{
}
}

/* ---------------------------------------------------------------- */
/* ROM ordinal 50 -- func_ov006_020d777c, 0x020d777c, size 0x1dc */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020d777c
#pragma push
#pragma opt_common_subs off
extern "C" {

void func_ov006_020d777c(char *sceneBytes, int bombIndex)
{
    int offset = bombIndex << 6;
    if (*(u16*)(sceneBytes + (bombIndex << 6) + 0x4600 + 0x90) != 0) {
        *(u16*)(sceneBytes + 0x4690 + offset) -= 1;
        if (*(s16*)(sceneBytes + (bombIndex << 6) + 0x4600 + 0x90) < 0)
            *(u16*)(sceneBytes + (bombIndex << 6) + 0x4600 + 0x90) = 0;
        return;
    }
    {
        int column = *(u8*)(sceneBytes + (bombIndex << 6) + 0x4000 + 0x69c);
        int row = 0;
        int curX, curY, targetY, targetX;
        while (column >= 10) { column -= 10; row++; }
        offset = row * 20;
        targetX = column * 16 + 0x38;
        targetY = offset - 0xC0;
        curX = *(int*)(sceneBytes + (bombIndex << 6) + 0x4000 + 0x660) >> 12;
        curY = *(int*)(sceneBytes + (bombIndex << 6) + 0x4000 + 0x664) >> 12;
        if (targetX == curX && targetY == curY) {
            *(u8*)(sceneBytes + (bombIndex << 6) + 0x4000 + 0x69b) = 4;
            if (*(u8*)(sceneBytes + (bombIndex << 6) + 0x4000 + 0x696) != 0)
                func_02012718((int)0x1d9, *(int*)(sceneBytes + (bombIndex << 6) + 0x4000 + 0x660));
            else
                func_02012718((int)0x1da, *(int*)(sceneBytes + (bombIndex << 6) + 0x4000 + 0x660));
            return;
        }
        if (targetY != curY) {
            *(int*)(sceneBytes + 0x4664 + (bombIndex << 6)) += *(int*)(sceneBytes + (bombIndex << 6) + 0x4000 + 0x670);
            if ((*(int*)(sceneBytes + (bombIndex << 6) + 0x4000 + 0x664) >> 12) >= targetY)
                *(int*)(sceneBytes + (bombIndex << 6) + 0x4000 + 0x664) = targetY << 12;
            return;
        }
        if (targetX > curX) {
            *(int*)(sceneBytes + 0x4660 + (bombIndex << 6)) += *(int*)(sceneBytes + (bombIndex << 6) + 0x4000 + 0x670);
            if (targetX <= (*(int*)(sceneBytes + (bombIndex << 6) + 0x4000 + 0x660) >> 12))
                *(int*)(sceneBytes + (bombIndex << 6) + 0x4000 + 0x660) = targetX << 12;
        } else if (targetX < curX) {
            *(int*)(sceneBytes + 0x4660 + (bombIndex << 6)) -= *(int*)(sceneBytes + (bombIndex << 6) + 0x4000 + 0x670);
            if (targetX >= curX)
                *(int*)(sceneBytes + (bombIndex << 6) + 0x4000 + 0x660) = targetX << 12;
        }
    }
}
}
#pragma pop

/* ---------------------------------------------------------------- */
/* ROM ordinal 51 -- func_ov006_020d7958, 0x020d7958, size 0x4 */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020d7958
extern "C" {
void func_ov006_020d7958(void)
{
}
}

/* ---------------------------------------------------------------- */
/* ROM ordinal 52 -- func_ov006_020d795c, 0x020d795c, size 0x128 */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020d795c
#pragma push
#pragma opt_strength_reduction off
#pragma opt_common_subs off
extern "C" {
void func_ov006_020d795c(char *sceneBytes, int bombIndex)
{
    int room, x;
    {
        s16 trigValue = data_02082214[((*(u16 *)(sceneBytes + bombIndex * 0x40 + 0x468c)) >> 4) * 2 + 1];
        *(int *)((char *)(((int)sceneBytes + 0x4660)) + bombIndex * 0x40) +=
            (int)(((s64)trigValue * *(int *)(sceneBytes + bombIndex * 0x40 + 0x4670) + 0x800) >> 12);
    }
    {
        s16 trigValue = data_02082214[((*(u16 *)(sceneBytes + bombIndex * 0x40 + 0x468c)) >> 4) * 2];
        *(int *)((char *)(((int)sceneBytes + 0x4664)) + bombIndex * 0x40) +=
            (int)(((s64)trigValue * *(int *)(sceneBytes + bombIndex * 0x40 + 0x4670) + 0x800) >> 12);
    }
    room = *(u8 *)(sceneBytes + bombIndex * 0x40 + 0x4696);
    x = *(int *)(sceneBytes + bombIndex * 0x40 + 0x4660) >> 12;
    if (room != 0) {
        if (x >= 0x110) {
            *(u8 *)(sceneBytes + bombIndex * 0x40 + 0x469b) = 2;
            *(int *)(sceneBytes + bombIndex * 0x40 + 0x4660) = 0x80000;
            *(int *)(sceneBytes + bombIndex * 0x40 + 0x4664) = -0xf0000;
        }
    } else {
        if (x <= -0x10) {
            *(u8 *)(sceneBytes + bombIndex * 0x40 + 0x469b) = 2;
            *(int *)(sceneBytes + bombIndex * 0x40 + 0x4660) = 0x80000;
            *(int *)(sceneBytes + bombIndex * 0x40 + 0x4664) = -0xf0000;
        }
    }
    func_ov006_020d634c(sceneBytes, *(u8 *)(sceneBytes + bombIndex * 0x40 + 0x4696));
}
}
#pragma pop

/* ---------------------------------------------------------------- */
/* ROM ordinal 53 -- func_ov006_020d7a84, 0x020d7a84, size 0x17c */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020d7a84
#pragma push
#pragma opt_strength_reduction off
#pragma opt_common_subs off
extern "C" {
void func_ov006_020d7a84(char *sceneBytes, int bombIndex)
{
    int room, x, y, tx, ty;

    tx = (*(u8 *)(sceneBytes + bombIndex * 0x40 + 0x4696) != 0) ? 0x100 : 0;
    ty = 0x60;
    tx -= *(int *)(sceneBytes + bombIndex * 0x40 + 0x4660) >> 12;
    ty -= *(int *)(sceneBytes + bombIndex * 0x40 + 0x4664) >> 12;

    *(s16 *)(sceneBytes + bombIndex * 0x40 + 0x468c) =
        _ZN4cstd5atan2E5Fix12IiES1_(ty, tx);

    {
        s16 trigValue = data_02082214[
            ((*(u16 *)(sceneBytes + bombIndex * 0x40 + 0x468c) >> 4) * 2) + 1];

        *(int *)((char *)(((int)sceneBytes + 0x4660)) +
                  bombIndex * 0x40) +=
            (int)(((s64)trigValue *
                   *(int *)(sceneBytes + bombIndex * 0x40 + 0x4670) + 0x800) >> 12);
    }

    {
        s16 trigValue = data_02082214[
            (*(u16 *)(sceneBytes + bombIndex * 0x40 + 0x468c) >> 4) * 2];

        *(int *)((char *)(((int)sceneBytes + 0x4664)) +
                  bombIndex * 0x40) +=
            (int)(((s64)trigValue *
                   *(int *)(sceneBytes + bombIndex * 0x40 + 0x4670) + 0x800) >> 12);
    }

    room = *(u8 *)(sceneBytes + bombIndex * 0x40 + 0x4696);
    x = *(int *)(sceneBytes + bombIndex * 0x40 + 0x4660) >> 12;
    y = *(int *)(sceneBytes + bombIndex * 0x40 + 0x4664) >> 12;
    tx = room ? 0x100 : 0;

    if (x > tx + 2)
        goto done;
    if (x < tx - 2)
        goto done;
    if (y > 0x62)
        goto done;
    if (y < 0x5e)
        goto done;

    *(u8 *)(sceneBytes + bombIndex * 0x40 + 0x469b) = 1;
    if (*(u8 *)(sceneBytes + bombIndex * 0x40 + 0x4696) != 0)
        *(s16 *)(sceneBytes + bombIndex * 0x40 + 0x468c) = 0;
    else
        *(u16 *)(sceneBytes + bombIndex * 0x40 + 0x468c) = 0x8000;

done:
    func_ov006_020d634c(sceneBytes, *(u8 *)(sceneBytes + bombIndex * 0x40 + 0x4696));
}
}
#pragma pop

/* ---------------------------------------------------------------- */
/* ROM ordinal 54 -- func_ov006_020d7c00, 0x020d7c00, size 0x4c */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020d7c00
extern "C" {
struct C_7c00;
typedef void (C_7c00::*PMF_7c00)(int);
struct Entry_7c00 { PMF_7c00 pmf; };
struct Elem_7c00 { char pad[0x40]; };
struct C_7c00 {};
extern "C" Entry_7c00 data_ov006_02141708[];
extern "C" void func_ov006_020d7c00(C_7c00* c, int i){
  Elem_7c00* e = (Elem_7c00*)c + i;
  unsigned char idx = *((unsigned char*)e + 0x469b);
  (c->*(data_ov006_02141708[idx].pmf))(i);
}
}
