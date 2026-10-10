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
 * This TU is the class's whole linker unit: 81 functions, .text
 * 0x020d5a54..0x020d95a4. It opens with the destructor, which the header
 * declares first and out of line, so this file is the key function and
 * emits the vtable and the RTTI chain. Then come the unnamed helpers and
 * round states, OnYoshiTryEat, Render, Behavior and InitResources, and it
 * closes with the registry factory dScMgBomroom_c_classInit. Functions run
 * in ROM order under
 * `#pragma defer_codegen off`; do not reorder. cstd::atan2 takes Fix12 by
 * value, so its call stays mangled.
 *
 * Each pragma bracket below carries the file-global pragma of the shard
 * that function came from; every bracket moves bytes, and they bind only
 * under defer_codegen off.
 *
 * comment leftovers:
 * func_ov006_020d68a8: a state field (and &state) differs from
 * (u8 *)(bomb + 0x4697), the idle-bomb timer store stays
 * raw + i*0x40 + 0x4690, and the color read goes through
 * (dScMgBomroom_Bomb *)(bomb + 0x4660) -- mBombs[i].color swaps the
 * compare's register pairing by one word.
 * func_ov006_020d7604: &f3c differs from
 * (unsigned char *)(int)(bomb + 0x469c).
 * func_ov006_020d7c4c: the sine-table value stays the first factor of
 * each product (s16 sinv/cosv); inline table reads flip smull operand
 * order. The spawner keeps its raw (c + (j << 6)) + 0x46NN stores.
 * func_ov006_020d5eb8, _020d5f2c, _020d6100, _020d6170, _020d6454:
 * the rec = this + index*0x10 temp and its *(ty *)(rec + 0x62NN) puns
 * are load-bearing -- mSlots* member access reschedules the address
 * math. func_ov006_020d5fd8 keeps its BrEnt5fd8 pad overlay for the
 * same reason (mSlotsB[index + 2].timer emits an extra add).
 * func_ov006_020d7edc, _020d8324: slot/h pointer temps likewise.
 * iarr[i][0x119c] in func_ov006_020d89c4 and _020d8af8 is a parallel
 * 0x40-stride int array outside the bomb records; nothing else names
 * that region.
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
#pragma defer_codegen off

/* No member needs explicit destruction: the empty body is the compiler's
 * own vtable store and base-destructor call. The deleting variant reaches
 * dScMgBase_c's operator delete, its immediate base's. */
// @symbol _ZN14dScMgBomroom_cD1Ev
// @symbol _ZN14dScMgBomroom_cD0Ev
dScMgBomroom_c::~dScMgBomroom_c()
{
}

// @symbol _ZN14dScMgBomroom_c19func_ov006_020d5ab0Ev
/* Draws the round-over sprite once +0x62fa is set. */

void dScMgBomroom_c::func_ov006_020d5ab0() {
    void * p = (void *)this;
    char *c = (char *)p;
    if (*(unsigned char *)(c + 0x6000 + 0x2fa) == 0) return;
    RenderOamMainScreen((int)data_ov006_02134f30, 0x80, 0xc0, -1, 1);
}


// @symbol _ZN14dScMgBomroom_c19func_ov006_020d5b00Ev

void dScMgBomroom_c::func_ov006_020d5b00() {
    char * p = (char *)this;
    mRoundOver = 1;
}


// @symbol _ZN14dScMgBomroom_c19func_ov006_020d5b10Ev
/* Steps the BG2 y offset (Fix12 at +0x62dc): state 1 raises it by 3 a
 * frame to 0xc0, state 2 waits out the +0x62f2 count, then lowers it by 3
 * a frame back to 0. */

void dScMgBomroom_c::func_ov006_020d5b10() {
    char * c = (char *)this;
    u8 state = mBg2State;

    if (state == 0) {
        return;
    }

    if (state == 1) {
        s32 v;
        s32 shifted;
        mBg2Y += 0x3000;
        v = mBg2Y;
        shifted = v >> 0xc;
        if (shifted >= 0xc0) {
            mBg2Y = 0xc0000;
            mBg2State = 2;
            shifted = 0xc0;
            mBg2Hold = shifted;
            Sound::PlayBank2_2D(0x1de);
            func_ov006_020d8904();
        }
        SetBg2Offset(0, shifted);
        return;
    }

    if (state != 2) {
        return;
    }

    if (mBg2Hold != 0) {
        mBg2Hold -= 1;
        if (mBg2Hold != 0) {
            return;
        }
        Sound::PlayBank2_2D(0x1df);
        return;
    }

    {
        s32 v;
        s32 shifted;
        mBg2Y -= 0x3000;
        v = mBg2Y;
        shifted = v >> 0xc;
        if (shifted <= 0) {
            shifted = 0;
            mBg2Y = 0;
            mBg2State = 0;
            Sound::PlayBank2_2D(0x1de);
        }
        SetBg2Offset(0, shifted);
    }
}


// @symbol _ZN14dScMgBomroom_c19func_ov006_020d5c60Ev

void dScMgBomroom_c::func_ov006_020d5c60() {
    char * p = (char *)this;
    int zero = 0;
    mBg2Y = zero;
    mBg2State = zero;
    mBg2Hold = zero;
    SetBg2Offset(zero, zero);
}


// @symbol _ZN14dScMgBomroom_c19func_ov006_020d5c88Ev
/* Draws the two 0x10-byte sprite records at +0x62b0 that are active. */

void dScMgBomroom_c::func_ov006_020d5c88() {
    int i;
    for (i = 0; i < 2; i++) {
        if (mMarkers[i].gate != 0) {
            int xv = mMarkers[i].x;
            int yv = mMarkers[i].y;
            int idx = mMarkers[i].idx;
            int a1 = xv >> 12;
            int a2 = yv >> 12;
            Hud_RenderSprite((i != 0) ? data_ov006_02133a70[idx]
                                      : data_ov006_02133ae0[idx],
                             a1, a2, -1, 1);
        }
    }
}


// @symbol _ZN14dScMgBomroom_c19func_ov006_020d5d08Ev
#pragma push
#pragma opt_strength_reduction off

void dScMgBomroom_c::func_ov006_020d5d08() {
    int i;
    for (i = 0; i < 2; i++) {
        if (mMarkers[i].state) {
            mMarkers[i].timer = mMarkers[i].timer + 1;
            if (mMarkers[i].timer >= 4) {
                mMarkers[i].timer = 0;
                mMarkers[i].idx = mMarkers[i].idx + 1;
                mMarkers[i].idx = mMarkers[i].idx & 3;
            }
        }
    }
}

#pragma pop

// @symbol _ZN14dScMgBomroom_c19func_ov006_020d5d90Ei

void dScMgBomroom_c::func_ov006_020d5d90(int idx) {
    mMarkers[idx].state = 1;
    mMarkers[idx].gate = 1;
    mMarkers[idx].x = 0x80000;
    mMarkers[idx].y = data_ov006_0212e2c0[idx] << 12;
    mMarkers[idx].timer = 0;
    mMarkers[idx].idx = 0;
}


/* The scene keeps its per-slot UI state as 0x10-stride entries at
 * +0x6260, +0x6280 and +0x62b0, two slots each. The loop helpers
 * (5dd4, 6278, 63ac, 65c8, 669c) address an entry as slot * 0x10 bytes into
 * scene storage and do not model it as a 16-byte array, which they would
 * index far past its end. That byte-base form matches only with strength
 * reduction off, so each of the five carries its own bracket. */
// @symbol _ZN14dScMgBomroom_c19func_ov006_020d5dd4Ev
#pragma push
#pragma opt_strength_reduction off

void dScMgBomroom_c::func_ov006_020d5dd4() {
    int slot;
    for (slot = 0; slot < 2; slot++) {
        mMarkers[slot].state = 0;
        mMarkers[slot].gate = 0;
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

// @symbol _ZN14dScMgBomroom_c19func_ov006_020d5e1cEv

void dScMgBomroom_c::func_ov006_020d5e1c() {
    mSlotsB[2].state = 3;
    Sound::PlayBank2_2D(0x1e3);
}


// @symbol _ZN14dScMgBomroom_c19func_ov006_020d5e3cEv

void dScMgBomroom_c::func_ov006_020d5e3c() {
    mSlotsB[2].state = 1;
    Sound::PlayBank2_2D(0x1e2);
}


// @symbol _ZN14dScMgBomroom_c19func_ov006_020d5e5cEv

void dScMgBomroom_c::func_ov006_020d5e5c() {
    char * c = (char *)this;
    c += 0x6000;
    if (*(unsigned char *)(c + 0x2af) == 0) return;
    RenderOamMainScreen(data_ov006_021343b0[*(unsigned char *)(c + 0x2ae)],
                        *(int *)(c + 0x2a0) >> 0xc, *(int *)(c + 0x2a4) >> 0xc, -1, 2);
}


// @symbol _ZN14dScMgBomroom_c19func_ov006_020d5eb8Ei

void dScMgBomroom_c::func_ov006_020d5eb8(int index) {
    unsigned char* raw = (unsigned char*)this;

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


// @symbol _ZN14dScMgBomroom_c19func_ov006_020d5f28Ev

void dScMgBomroom_c::func_ov006_020d5f28() {
}


// @symbol _ZN14dScMgBomroom_c19func_ov006_020d5f2cEi

void dScMgBomroom_c::func_ov006_020d5f2c(int index) {
    char *raw = (char *)this;

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


// @symbol _ZN14dScMgBomroom_c19func_ov006_020d5fd8Ei

typedef struct { char pad[0xa8]; short f; } BrEnt5fd8;
void dScMgBomroom_c::func_ov006_020d5fd8(int index) {
    int raw = (int)this;

  BrEnt5fd8 *rec = (BrEnt5fd8*)(raw + (index<<4) + 0x6200);
  rec->f = 0;
}


// @symbol _ZN14dScMgBomroom_c19func_ov006_020d5fecEv

typedef void (dScMgBomroom_c::*PMF)(int);
struct PMFEntry { PMF pmf; };
extern "C" PMFEntry data_ov006_02141660[];
void dScMgBomroom_c::func_ov006_020d5fec() {
    if (!mSlotsB[2].gate) return;
    (this->*(data_ov006_02141660[mSlotsB[2].state].pmf))(0);
}


// @symbol _ZN14dScMgBomroom_c19func_ov006_020d604cEv

void dScMgBomroom_c::func_ov006_020d604c() {
    mSlotsB[2].gate = 1;
    mSlotsB[2].state = 0;
    mSlotsB[2].idx = 0;
    mSlotsB[2].level = 1;
    mSlotsB[2].timer = 0;
    mSlotsB[2].x = 0x80000;
    mSlotsB[2].y = 0x8000;
}


// @symbol _ZN14dScMgBomroom_c19func_ov006_020d6084Ev

void dScMgBomroom_c::func_ov006_020d6084() {
    mSlotsB[2].gate = 0;
    mSlotsB[2].level = 0;
}


// @symbol _ZN14dScMgBomroom_c19func_ov006_020d6098Ev

void dScMgBomroom_c::func_ov006_020d6098() {
    int i;
    for (i = 0; i < 2; i++) {
        int xv = mSlotsB[i].x >> 12;
        int yv = mSlotsB[i].y >> 12;
        int k = mSlotsB[i].level;
        if (i != 0) k += 5;
        Hud_RenderSprite((void*)data_ov006_021344ec[k], xv, yv, -1, -1);
    }
}


// @symbol _ZN14dScMgBomroom_c19func_ov006_020d6100Ei

void dScMgBomroom_c::func_ov006_020d6100(int index) {
    unsigned char* raw = (unsigned char*)this;

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


// @symbol _ZN14dScMgBomroom_c19func_ov006_020d6170Ei

void dScMgBomroom_c::func_ov006_020d6170(int index) {
    char* raw = (char*)this;

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


// @symbol _ZN14dScMgBomroom_c19func_ov006_020d61dcEi

void dScMgBomroom_c::func_ov006_020d61dc(int index) {
    mSlotsB[index].timer++;
    if (mSlotsB[index].timer < 5)
        return;
    mSlotsB[index].timer = 0;
    mSlotsB[index].level++;
    if (mSlotsB[index].level >= 4) {
        mSlotsB[index].state = 2;
        mSlotsB[index].timer = 0x20;
    }
}


// @symbol _ZN14dScMgBomroom_c19func_ov006_020d6264Ei

void dScMgBomroom_c::func_ov006_020d6264(int index) {
  mSlotsB[index].timer = 0;
}


// @symbol _ZN14dScMgBomroom_c19func_ov006_020d6278Ev
#pragma push
#pragma opt_strength_reduction off

typedef void (dScMgBomroom_c::*PMF)(int);
extern "C" PMF data_ov006_021416c0[];
void dScMgBomroom_c::func_ov006_020d6278() {
    for (int slot = 0; slot < 2; slot++) {
        if (mSlotsB[slot].gate) {
            u8 state = mSlotsB[slot].state;
            (this->*data_ov006_021416c0[state])(slot);
        }
    }
}

#pragma pop

// @symbol _ZN14dScMgBomroom_c19func_ov006_020d62e0Ev
#pragma push
#pragma opt_strength_reduction off

void dScMgBomroom_c::func_ov006_020d62e0() {
    void * p = (void *)this;
    int i;
    for (i = 0; i < 2; i++) {
        mSlotsB[i].gate = 1;
        mSlotsB[i].state = 0;
        mSlotsB[i].idx = 1;
        mSlotsB[i].x = data_ov006_0212e2c8[i] << 0xc;
        mSlotsB[i].y = data_ov006_0212e2e0[i] << 0xc;
        mSlotsB[i].level = 0;
        mSlotsB[i].timer = 0;
    }
}

#pragma pop

// @symbol _ZN14dScMgBomroom_c19func_ov006_020d634cEi

void dScMgBomroom_c::func_ov006_020d634c(int index) {
    char * raw = (char *)this;
    unsigned char st = mSlotsB[index].state;
    if (st != 0) {
        if (st == 2) {
            mSlotsB[index].timer = 0x20;
        } else {
            mSlotsB[index].state = 1;
        }
        return;
    }
    mSlotsB[index].state = 1;
    SetBg0Offset(0x100, 0);
}


// @symbol _ZN14dScMgBomroom_c19func_ov006_020d63acEv
#pragma push
#pragma opt_strength_reduction off

void dScMgBomroom_c::func_ov006_020d63ac() {
    int slot;
    for (slot = 0; slot < 2; slot++) {
        mSlotsB[slot].gate = 0;
        mSlotsB[slot].idx = 0;
    }
}

#pragma pop

// @symbol _ZN14dScMgBomroom_c19func_ov006_020d63d4Ev

void dScMgBomroom_c::func_ov006_020d63d4() {
    int i;
    for (i = 0; i < 2; i++) {
        if (mSlotsA[i].idx != 0) {
            int xv = mSlotsA[i].x >> 12;
            int yv = mSlotsA[i].y >> 12;
            int k = mSlotsA[i].level;
            int v = (i == 0) ? data_ov006_021343b0[k] : data_ov006_021342bc[k];
            Hud_RenderSprite((void*)v, xv, yv, -1, 1);
        }
    }
}


// @symbol _ZN14dScMgBomroom_c19func_ov006_020d6454Ei

void dScMgBomroom_c::func_ov006_020d6454(int index) {
    unsigned char* raw = (unsigned char*)this;

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


// @symbol _ZN14dScMgBomroom_c19func_ov006_020d64c4Ev

void dScMgBomroom_c::func_ov006_020d64c4() {
}


// @symbol _ZN14dScMgBomroom_c19func_ov006_020d64c8Ei
#pragma push
#pragma opt_common_subs off
#pragma opt_strength_reduction off

void dScMgBomroom_c::func_ov006_020d64c8(int index) {
    char * raw = (char *)this;
    mSlotsA[index].timer += 1;
    if (mSlotsA[index].timer < 5)
        return;
    mSlotsA[index].timer = 0;
    mSlotsA[index].level += 1;
    if (index == 0) {
        u8 level = mSlotsA[index].level;
        if (level == 2 || level == 4) {
            int *offsetY = &mMarkers[0].y;
            *offsetY += 0x8000;
        }
    }
    if (mSlotsA[index].level == 3 && index == 1)
        func_ov006_020d5d90(index);
    if (mSlotsA[index].level >= 4)
        mSlotsA[index].state = 2;
}

#pragma pop

// @symbol _ZN14dScMgBomroom_c19func_ov006_020d65b4Ei

void dScMgBomroom_c::func_ov006_020d65b4(int index) {
  mSlotsA[index].timer = 0;
}


// @symbol _ZN14dScMgBomroom_c19func_ov006_020d65c8Ev
#pragma push
#pragma opt_strength_reduction off

typedef void (dScMgBomroom_c::*PMF)(int);
extern "C" PMF data_ov006_02141680[];
void dScMgBomroom_c::func_ov006_020d65c8() {
    for (int slot = 0; slot < 2; slot++) {
        if (mSlotsA[slot].gate) {
            u8 state = mSlotsA[slot].state;
            (this->*data_ov006_02141680[state])(slot);
        }
    }
}

#pragma pop

// @symbol _ZN14dScMgBomroom_c19func_ov006_020d6630Ev
#pragma push
#pragma opt_strength_reduction off

void dScMgBomroom_c::func_ov006_020d6630() {
    void * p = (void *)this;
    int i;
    for (i = 0; i < 2; i++) {
        mSlotsA[i].gate = 1;
        mSlotsA[i].state = 0;
        mSlotsA[i].x = data_ov006_0212e2d8[i] << 0xc;
        mSlotsA[i].y = data_ov006_0212e2d0[i] << 0xc;
        mSlotsA[i].level = 0;
        mSlotsA[i].timer = 0;
        mSlotsA[i].idx = 1;
    }
}

#pragma pop

// @symbol _ZN14dScMgBomroom_c19func_ov006_020d669cEv
#pragma push
#pragma opt_strength_reduction off

void dScMgBomroom_c::func_ov006_020d669c() {
    int slot;
    for (slot = 0; slot < 2; slot++) {
        mSlotsA[slot].gate = 0;
        mSlotsA[slot].idx = 0;
    }
}

#pragma pop

// @symbol _ZN14dScMgBomroom_c19func_ov006_020d66c4Ei

void dScMgBomroom_c::func_ov006_020d66c4(int index) {
    char * raw = (char *)this;
    if (mSlotsA[index].state != 0) return;
    mSlotsA[index].state = 1;
    Sound::PlayBank2_2D(0x1e2);
    if (index != 0) return;
    func_ov006_020d5d90(index);
}


// @symbol _ZN14dScMgBomroom_c19func_ov006_020d672cEv

void dScMgBomroom_c::func_ov006_020d672c() {
  if(unk_62f9==0) return;
  func_ov004_020b2444(0x80,0xc,unk_62ee,1,-1,0,0);
}


// @symbol _ZN14dScMgBomroom_c19func_ov006_020d6784Ev
#pragma push
#pragma opt_strength_reduction off

void dScMgBomroom_c::func_ov006_020d6784() {
    char * raw = (char *)this;
    int plain, colored, i;
    colored = 0;
    plain = 0;
    for (i = 0; i < 0x70; i++) {
        if (mBombs[i].active == 0) continue;
        if (mBombs[i].state != 5) continue;
        if (mBombs[i].color != 0) colored++;
        else plain++;
    }
    if (plain >= 0x28) {
        mSceneState = 3;
        mSubState = 0;
        mBlastPen = 0;
        mWinColor = 0;
        *(volatile unsigned int *)(raw + 0xbc) = *(unsigned int *)(raw + 0xbc) + 1;
        if (*(unsigned int *)(raw + 0xbc) > 0x270e) *(unsigned int *)(raw + 0xbc) = 0x270e;
        unk_62fb = 1;
        Sound::PlayBank2_2D(0x1e6);
    } else if (colored >= 0x28) {
        mSceneState = 3;
        mSubState = 0;
        mBlastPen = 0;
        mWinColor = 1;
        *(volatile unsigned int *)(raw + 0xbc) = *(unsigned int *)(raw + 0xbc) + 1;
        if (*(unsigned int *)(raw + 0xbc) > 0x270e) *(unsigned int *)(raw + 0xbc) = 0x270e;
        unk_62fb = 1;
        Sound::PlayBank2_2D(0x1e6);
    }
}

#pragma pop

// @symbol _ZN14dScMgBomroom_c19func_ov006_020d68a8Ei
/* Bomb `picked` has gone off. Pen byte +0x3a of that bomb is 0 when it
 * was loose: every other live bomb not yet sorted (state 5) is knocked into
 * state 3 for 0x20 frames. Otherwise only the bombs of that pen's color and
 * the idle ones are. The first blast also records its pen at 0x62f8 and sets
 * the scene state at 0x62d0 to 3. */
#pragma push
#pragma opt_strength_reduction off
#pragma opt_common_subs off

void dScMgBomroom_c::func_ov006_020d68a8(int picked) {
    char * raw = (char *)this;
    int i;
    u8 *cur = (u8 *)(((int)(raw + picked * 0x40) + 0x469a));
    for (i = 0; i < 0x70; i++) {
        char *bomb = raw + i * 0x40;
        if (mBombs[i].active != 0) {
            if (i != picked) {
                u8 pen = *cur;
                if (pen == 0) {
                    u8 *state = (u8 *)(bomb + 0x4697);
                    if (*state != 5) {
                        *state = 3;
                        mBombs[i].unk_30 = 0x20;
                    }
                } else {
                    u8 *state = (u8 *)(bomb + 0x4697);
                    if (*state == 5) {
                        if (((dScMgBomroom_Bomb *)(bomb + 0x4660))->color + 1 == pen) {
                            *state = 3;
                            mBombs[i].unk_30 = 0x20;
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
    if (mBlastPen != 0)
        return;
    mBlastPen = mBombs[picked].pen + 1;
    mStateTimer = 0x60;
    mSceneState = 3;
    mSubState = 0;
}

#pragma pop

// @symbol _ZN14dScMgBomroom_c19func_ov006_020d69b8Ei
/* Steps one bomb's animation: the frame advances when the counter at +0x2e
 * reaches the per-type frame time, playing its sound effects, and
 * wraps (or holds, for type 3) at the per-type frame count. */


void dScMgBomroom_c::func_ov006_020d69b8(int index) {
    int type;
    int frame;
    int max;

    mBombs[index].counter += 1;
    type = mBombs[index].type;
    frame = mBombs[index].frame;

    if (type != 3 && type != 0 && mSceneState != 3) {
        mBombs[index].sound = func_02012468(mBombs[index].sound, 2, 0x1db, 4, 0, 0,
                               func_020126e8(mBombs[index].x), 0);
    }

    if (mBombs[index].counter >= data_ov006_0213bb08[type][frame]) {
        frame++;
        mBombs[index].frame = frame;
        mBombs[index].counter = 0;
        if (type != 3 && mBombs[index].state != 2 && (type != 0 || mBombs[index].state != 5)) {
            if (mBombs[index].active == 1 && mSceneState != 3) {
                if (mBombs[index].color != 0) {
                    func_02012718((int)0x1d9, mBombs[index].x);
                } else {
                    func_02012718((int)0x1da, mBombs[index].x);
                }
            }
        }
        if (type == 3 && frame == 1) {
            func_02012718((int)0x1e1, mBombs[index].x);
        }
    }

    max = data_ov006_0213b9bc[type];
    if (frame < max) {
        return;
    }
    if (type == 3) {
        mBombs[index].active = 0;
        mBombs[index].unk_39 = 0;
    } else {
        max = 0;
    }
    mBombs[index].frame = max;
}


// @symbol _ZN14dScMgBomroom_c19func_ov006_020d6b88Ei


void dScMgBomroom_c::func_ov006_020d6b88(int index) {
    char * raw = (char *)this;
    char *bomb = raw + index * 0x40;
    u8 color = mBombs[index].color;
    int x = mBombs[index].x >> 0xc;
    int y = mBombs[index].y >> 0xc;
    if (color != 0) {
        if (x <= 0xc0) return;
        if (y <= 0x40) return;
        if (y >= 0x80) return;
        mBombs[index].state = 5;
        mBombs[index].type = 0;
        mBombs[index].frame = 0;
        mBombs[index].counter = 0;
        mBombs[index].speed = 0x999;
        func_02012718((int)0x1dc, mBombs[index].x);
    } else {
        if (x >= 0x40) return;
        if (y <= 0x40) return;
        if (y >= 0x80) return;
        mBombs[index].state = 5;
        mBombs[index].type = 0;
        mBombs[index].frame = 0;
        mBombs[index].counter = 0;
        mBombs[index].speed = 0x999;
        func_02012718((int)0x1dc, mBombs[index].x);
    }
}


// @symbol _ZN14dScMgBomroom_c19func_ov006_020d6c90Ei

void dScMgBomroom_c::func_ov006_020d6c90(int index) {
    char * raw = (char *)this;
    char *bomb = raw + (index << 6);
    unsigned char color = mBombs[index].color;
    int x = mBombs[index].x >> 12;
    int y = mBombs[index].y >> 12;

    if (color != 0) {
        if (x >= 0x40)
            return;
        if (y <= 0x40)
            return;
        if (y >= 0x80)
            return;
        mBombs[index].unk_32 = 0;
        mBombs[index].state = 4;
        mBombs[index].type = 3;
        mBombs[index].frame = 0;
        mBombs[index].counter = 0;
        mBombs[index].pen = 1;
        mBombs[index].unk_3d = 1;
        func_ov006_020d68a8(index);
    } else {
        if (x <= 0xc0)
            return;
        if (y <= 0x40)
            return;
        if (y >= 0x80)
            return;
        mBombs[index].unk_32 = 0;
        mBombs[index].state = 4;
        mBombs[index].type = 3;
        mBombs[index].frame = 0;
        mBombs[index].counter = 0;
        mBombs[index].pen = 2;
        mBombs[index].unk_3d = 1;
        func_ov006_020d68a8(index);
    }
}


// @symbol _ZN14dScMgBomroom_c19func_ov006_020d6d7cEi
/* Picks up a bomb under the stylus: a touch within 12 units across and 15
 * down of it, while nothing is held (0x62f6 is 0xff). grabX/grabY keep the
 * stylus-minus-bomb offset. */
#pragma push
#pragma opt_propagation off


typedef struct { u8 f0, f1, f2, f3; } Tab;
// local extern: see above.
extern u8 gActivePlayerSlot;
// local extern: see above.
extern Tab gTouchHeld[];

void dScMgBomroom_c::func_ov006_020d6d7c(int index) {
    char * raw = (char *)this;
    u8 touch = gActivePlayerSlot;
    int touching = 0;
    int dx, dy;
    if (gTouchHeld[touch].f0 != 0) {
        if (gTouchHeld[touch].f1 != 0) touching = 1;
    }
    if (touching == 0) return;
    if (mHeldColor != 0xff) return;
    dx = gTouchHeld[touch].f2 - (mBombs[index].x >> 0xc);
    dy = gTouchHeld[touch].f3 - (mBombs[index].y >> 0xc);
    if (dx > 0xc) return;
    if (dx < -0xc) return;
    if (dy > 0xf) return;
    if (dy < -0xf) return;
    mHeldColor = mBombs[index].color;
    mBombs[index].type = 1;
    mBombs[index].state = 2;
    mBombs[index].grabX = dx << 0xc;
    mBombs[index].grabY = dy << 0xc;
    mBombs[index].unk_3e = 0;
    func_02012718((int)0x1d2, mBombs[index].x);
}

#pragma pop

// @symbol _ZN14dScMgBomroom_c19func_ov006_020d6e8cEi
#pragma push
#pragma opt_common_subs off

void dScMgBomroom_c::func_ov006_020d6e8c(int index) {
    char* raw = (char*)this;
    int x, y, py, dx, dy1, ang, i2, a, dy2, sy, px;

    x = mBombs[index].x >> 12;
    y = mBombs[index].y >> 12;

    if (x + 12 > 0x100) {
        mBombs[index].angle = 0x8000 - mBombs[index].angle;
        mBombs[index].x = 0xF4000;
    } else if (x - 12 < 0) {
        mBombs[index].angle = 0x8000 - mBombs[index].angle;
        mBombs[index].x = 0xC000;
    }

    if (y + 12 > 0xB8) {
        mBombs[index].angle = 0 - mBombs[index].angle;
        mBombs[index].y = 0xAC000;
    } else if (y - 12 < 0) {
        mBombs[index].angle = 0 - mBombs[index].angle;
        mBombs[index].y = 0xC000;
    }

    px = mBombs[index].x >> 12;
    py = mBombs[index].y >> 12;

    if (px + 12 > 0xC0 && py + 12 > 0x40 && py - 12 < 0x80) {
        dx = px - 0xC0;
        dy1 = py - 0x40;
        dy2 = 0x80 - py;
        if (px <= 0xC0 && py >= 0x40 && py <= 0x80) {
            mBombs[index].angle = 0x8000 - mBombs[index].angle;
            mBombs[index].x = 0xB4000;
        } else if (px > 0xC0 && py < 0x40) {
            mBombs[index].angle = 0 - mBombs[index].angle;
            mBombs[index].y = 0x34000;
        } else if (px > 0xC0 && py > 0x80) {
            mBombs[index].angle = 0 - mBombs[index].angle;
            mBombs[index].y = 0x8C000;
        } else if (px > 0xC0 && py > 0x40 && py < 0x80) {
            if (dy1 < dy2) {
                if (dx < dy1) {
                    mBombs[index].angle = 0x8000 - mBombs[index].angle;
                    mBombs[index].x = 0xB4000;
                } else {
                    mBombs[index].angle = 0 - mBombs[index].angle;
                    mBombs[index].y = 0x34000;
                }
            } else {
                if (dx < dy2) {
                    mBombs[index].angle = 0x8000 - mBombs[index].angle;
                    mBombs[index].x = 0xB4000;
                } else {
                    mBombs[index].angle = 0 - mBombs[index].angle;
                    mBombs[index].y = 0x8C000;
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
            mBombs[index].x =
                (0xC0 - (int)(((s64)data_02082214[i2 + 1] * 0xF + 0x800) >> 12)) << 12;
            if (py >= 0x60) {
                mBombs[index].y =
                    (0x80 - (int)(((s64)data_02082214[i2] * 0xF + 0x800) >> 12)) << 12;
            } else {
                mBombs[index].y =
                    (0x40 - (int)(((s64)data_02082214[i2] * 0xF + 0x800) >> 12)) << 12;
            }
            a = mBombs[index].angle;
            if (data_02082214[(a >> 4) * 2 + 1] > 0) {
                mBombs[index].angle = 0x8000 - a;
            } else {
                mBombs[index].angle = 0 - a;
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
        mBombs[index].angle = 0x8000 - mBombs[index].angle;
        mBombs[index].x = 0x4C000;
        return;
    }
    if (px < 0x40 && py < 0x40) {
        mBombs[index].angle = 0 - mBombs[index].angle;
        mBombs[index].y = 0x34000;
        return;
    }
    if (px < 0x40 && py > 0x80) {
        mBombs[index].angle = 0 - mBombs[index].angle;
        mBombs[index].y = 0x8C000;
        return;
    }
    if (px < 0x40 && py > 0x40 && py < 0x80) {
        if (dy1 < sy) {
            if (dy2 < dy1) {
                mBombs[index].angle = 0x8000 - mBombs[index].angle;
                mBombs[index].x = 0x4C000;
                return;
            }
            mBombs[index].angle = 0 - mBombs[index].angle;
            mBombs[index].y = 0x34000;
            return;
        }
        if (dy2 < sy) {
            mBombs[index].angle = 0x8000 - mBombs[index].angle;
            mBombs[index].x = 0x4C000;
            return;
        }
        mBombs[index].angle = 0 - mBombs[index].angle;
        mBombs[index].y = 0x8C000;
        return;
    }

    if (py < 0x60) {
        sy = 0x40 - py;
    }
    cstd::sqrt((s64)(dy2 * dy2 + sy * sy));
    ang = (u16)_ZN4cstd5atan2E5Fix12IiES1_(sy, dy2);
    i2 = (ang >> 4) * 2;
    mBombs[index].x =
        (0x40 - (int)(((s64)data_02082214[i2 + 1] * 0xF + 0x800) >> 12)) << 12;
    if (py >= 0x60) {
        mBombs[index].y =
            (0x80 - (int)(((s64)data_02082214[i2] * 0xF + 0x800) >> 12)) << 12;
    } else {
        mBombs[index].y =
            (0x40 - (int)(((s64)data_02082214[i2] * 0xF + 0x800) >> 12)) << 12;
    }
    a = mBombs[index].angle;
    if (data_02082214[(a >> 4) * 2 + 1] < 0) {
        mBombs[index].angle = 0x8000 - a;
    } else {
        mBombs[index].angle = 0 - a;
    }
}

#pragma pop

// @symbol _ZN14dScMgBomroom_c19func_ov006_020d7524Ev

void dScMgBomroom_c::func_ov006_020d7524() {
    int i;
    for (i = 0; i < 0x70; i++) {
        if (mBombs[i].unk_39 == 0) continue;
        {
            int b694 = mBombs[i].type;
            int palette = 1;
            int b695, b696;
            unsigned char sprite;
            int b697;
            int f664;
            int x, y;
            if (b694 == 3) palette = 0;
            b696 = mBombs[i].color;
            b695 = mBombs[i].frame;
            if (b696 != 0) { sprite = data_ov006_0213bb28[b694][b695]; }
            else { sprite = data_ov006_0213bb18[b694][b695]; }
            b697 = mBombs[i].state;
            x = mBombs[i].x >> 12;
            f664 = mBombs[i].y;
            y = f664 >> 12;
            if (b697 == 6) {
                if (mBombs[i].substate == 4) { y = (f664 - mBg2Y) >> 12; }
            }
            func_ov004_020afdd0(data_ov006_0213bb4c[sprite], x, y, -1, palette);
        }
    }
}


// @symbol _ZN14dScMgBomroom_c19func_ov006_020d7604Ev
#pragma push
#pragma opt_strength_reduction off
#pragma opt_common_subs off

void dScMgBomroom_c::func_ov006_020d7604() {
    void * raw = (void *)this;
    unsigned char *bytes = (unsigned char *)raw;
    int ca = 0;
    int cb = 0;
    int cc = 0;
    int i;

    for (i = 0; i < 0x70; i++) {
        unsigned char *bomb = bytes + i * 64;
        if (mBombs[i].active != 0) {
            if (mBlastPen == 1) {
                if (mBombs[i].color != 0) {
                    int v;
                    int q;
                    mBombs[i].substate = 3;
                    v = ca;
                    q = 0;
                    mBombs[i].unk_30 = (unsigned short)(cc * 8);
                    while (v >= 5) { v -= 5; q++; }
                    ((unsigned char *)(int)(bytes + i * 64))[0x469c] = (unsigned char)(q * 10 + v);
                    ca++;
                    cc++;
                } else {
                    int v;
                    int q;
                    unsigned char *d = (unsigned char *)(int)(bomb + 0x469c);
                    *d = (unsigned char)cb;
                    mBombs[i].substate = 3;
                    v = cb;
                    mBombs[i].unk_30 = (unsigned short)(cc * 8);
                    q = 0;
                    while (v >= 5) { v -= 5; q++; }
                    *d = (unsigned char)(q * 10 + v + 5);
                    cb++;
                    cc++;
                }
            } else if (mBlastPen != 0) {
                mBombs[i].unk_3c = (unsigned char)cc;
                mBombs[i].substate = 3;
                mBombs[i].unk_30 = (unsigned short)(cc * 8);
                cc++;
            } else {
                if (mWinColor == mBombs[i].color && mBombs[i].state == 6) {
                    mBombs[i].unk_3c = (unsigned char)cc;
                    mBombs[i].substate = 3;
                    mBombs[i].unk_30 = (unsigned short)(cc * 8);
                    cc++;
                }
            }
        }
    }
    func_ov006_020d5e3c();
}

#pragma pop

// @symbol _ZN14dScMgBomroom_c19func_ov006_020d7778Ev

void dScMgBomroom_c::func_ov006_020d7778() {
}


// @symbol _ZN14dScMgBomroom_c19func_ov006_020d777cEi
#pragma push
#pragma opt_common_subs off


void dScMgBomroom_c::func_ov006_020d777c(int index) {
    char * raw = (char *)this;
    int off = index << 6;
    if (mBombs[index].unk_30 != 0) {
        mBombs[index].unk_30 -= 1;
        if ((s16)mBombs[index].unk_30 < 0)
            mBombs[index].unk_30 = 0;
        return;
    }
    {
        int slot = mBombs[index].unk_3c;
        int row = 0;
        int curX, curY, targetY, targetX;
        while (slot >= 10) { slot -= 10; row++; }
        off = row * 20;
        targetX = slot * 16 + 0x38;
        targetY = off - 0xC0;
        curX = mBombs[index].x >> 12;
        curY = mBombs[index].y >> 12;
        if (targetX == curX && targetY == curY) {
            mBombs[index].substate = 4;
            if (mBombs[index].color != 0)
                func_02012718((int)0x1d9, mBombs[index].x);
            else
                func_02012718((int)0x1da, mBombs[index].x);
            return;
        }
        if (targetY != curY) {
            mBombs[index].y += mBombs[index].speed;
            if ((mBombs[index].y >> 12) >= targetY)
                mBombs[index].y = targetY << 12;
            return;
        }
        if (targetX > curX) {
            mBombs[index].x += mBombs[index].speed;
            if (targetX <= (mBombs[index].x >> 12))
                mBombs[index].x = targetX << 12;
        } else if (targetX < curX) {
            mBombs[index].x -= mBombs[index].speed;
            if (targetX >= curX)
                mBombs[index].x = targetX << 12;
        }
    }
}

#pragma pop

// @symbol _ZN14dScMgBomroom_c19func_ov006_020d7958Ev

void dScMgBomroom_c::func_ov006_020d7958() {
}


// @symbol _ZN14dScMgBomroom_c19func_ov006_020d795cEi
#pragma push
#pragma opt_strength_reduction off
#pragma opt_common_subs off

void dScMgBomroom_c::func_ov006_020d795c(int index) {
    char * raw = (char *)this;
    int color, x;
    {
        s16 sine = data_02082214[((mBombs[index].angle) >> 4) * 2 + 1];
        mBombs[index].x +=
            (int)(((s64)sine * mBombs[index].speed + 0x800) >> 12);
    }
    {
        s16 sine = data_02082214[((mBombs[index].angle) >> 4) * 2];
        mBombs[index].y +=
            (int)(((s64)sine * mBombs[index].speed + 0x800) >> 12);
    }
    color = mBombs[index].color;
    x = mBombs[index].x >> 12;
    if (color != 0) {
        if (x >= 0x110) {
            mBombs[index].substate = 2;
            mBombs[index].x = 0x80000;
            mBombs[index].y = -0xf0000;
        }
    } else {
        if (x <= -0x10) {
            mBombs[index].substate = 2;
            mBombs[index].x = 0x80000;
            mBombs[index].y = -0xf0000;
        }
    }
    func_ov006_020d634c(mBombs[index].color);
}

#pragma pop

// @symbol _ZN14dScMgBomroom_c19func_ov006_020d7a84Ei
#pragma push
#pragma opt_strength_reduction off
#pragma opt_common_subs off

void dScMgBomroom_c::func_ov006_020d7a84(int index) {
    char * raw = (char *)this;
    int color, x, z, tx, ty;

    tx = (mBombs[index].color != 0) ? 0x100 : 0;
    ty = 0x60;
    tx -= mBombs[index].x >> 12;
    ty -= mBombs[index].y >> 12;

    mBombs[index].angle =
        _ZN4cstd5atan2E5Fix12IiES1_(ty, tx);

    {
        s16 sine = data_02082214[
            ((mBombs[index].angle >> 4) * 2) + 1];

        mBombs[index].x +=
            (int)(((s64)sine *
                   mBombs[index].speed + 0x800) >> 12);
    }

    {
        s16 sine = data_02082214[
            (mBombs[index].angle >> 4) * 2];

        mBombs[index].y +=
            (int)(((s64)sine *
                   mBombs[index].speed + 0x800) >> 12);
    }

    color = mBombs[index].color;
    x = mBombs[index].x >> 12;
    z = mBombs[index].y >> 12;
    tx = color ? 0x100 : 0;

    if (x > tx + 2)
        goto done;
    if (x < tx - 2)
        goto done;
    if (z > 0x62)
        goto done;
    if (z < 0x5e)
        goto done;

    mBombs[index].substate = 1;
    if (mBombs[index].color != 0)
        mBombs[index].angle = 0;
    else
        mBombs[index].angle = 0x8000;

done:
    func_ov006_020d634c(mBombs[index].color);
}

#pragma pop

// @symbol _ZN14dScMgBomroom_c19func_ov006_020d7c00Ei
/* Runs the bomb's current handler: the byte at +0x3b indexes the
 * pointer-to-member table data_ov006_02141708. */

extern "C" PMFEntry data_ov006_02141708[];
void dScMgBomroom_c::func_ov006_020d7c00(int index) {
  unsigned char state = mBombs[index].substate;
  (this->*(data_ov006_02141708[state].pmf))(index);
}


// @symbol _ZN14dScMgBomroom_c19func_ov006_020d7c4cEi
/* Roaming step: advance x/y along the angle at speed (sine table, Fix12),
 * then bounce off the box selected by color (0xc0..0x100 x 0x40..0x80 for
 * color 1, 0..0x40 x 0x40..0x80 for color 0): a vertical wall mirrors the
 * angle (0x8000 - a), a horizontal one negates it, and the coordinate is
 * pinned just inside the wall. */

void dScMgBomroom_c::func_ov006_020d7c4c(int idx) {
    dScMgBomroom_c * self = (dScMgBomroom_c *)this;
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


// @symbol _ZN14dScMgBomroom_c19func_ov006_020d7e7cEi

void dScMgBomroom_c::func_ov006_020d7e7c(int i) {
    char * c = (char *)this;
    char *b = c + i * 64;
    if (mBombs[i].unk_3d == 0) return;
    if (mBombs[i].frame != 0) return;
    if (mBombs[i].counter != 1) return;
    Sound::PlayBank2_2D(0x1e0);
    unk_62f0 = 0x60;
}


// @symbol _ZN14dScMgBomroom_c19func_ov006_020d7edcEi

void dScMgBomroom_c::func_ov006_020d7edc(int idx) {
    unsigned char * c = (unsigned char *)this;
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


// @symbol _ZN14dScMgBomroom_c19func_ov006_020d7f5cEi
/* A held bomb follows the stylus less its grab offset, and +0x69e records
 * whether it is over its own pen. With nothing held (0x62f6 is 0xff) the
 * bomb goes back to state 1 instead. */
#pragma push
#pragma opt_common_subs off

#define B ((char *)self_ + idx * 0x40 + 0x4000)
void dScMgBomroom_c::func_ov006_020d7f5c(int idx) {
    char * self_ = (char *)this;
    if (mHeldColor == 0xff) {
        mBombs[idx].state = 1;
        mBombs[idx].unk_32 += 0x40;
        func_ov006_020d6b88(idx);
        func_ov006_020d6c90(idx);
    } else {
        int old_x = mBombs[idx].x;
        int old_y = mBombs[idx].y;
        int cx, cy;
        u8 was;
        int t = gActivePlayerSlot;

        mBombs[idx].x = (gTouchX[t << 2] << 12) - mBombs[idx].grabX;
        mBombs[idx].y = (gTouchY[t << 2] << 12) - mBombs[idx].grabY;
        cx = old_x >> 12;
        cy = old_y >> 12;

        func_ov006_020d6e8c(idx);

        {
            int t2 = gActivePlayerSlot;
            mBombs[idx].x = (gTouchX[t2 << 2] << 12) - mBombs[idx].grabX;
            mBombs[idx].y = (gTouchY[t2 << 2] << 12) - mBombs[idx].grabY;
        }

        was = mBombs[idx].unk_3e;
        if (mBombs[idx].color != 0) {
            if (cx > 0xc0 && cy > 0x40 && cy < 0x80) {
                mBombs[idx].unk_3e = 1;
            } else {
                mBombs[idx].unk_3e = 0;
            }
        } else {
            if (cx < 0x40 && cy > 0x40 && cy < 0x80) {
                mBombs[idx].unk_3e = 1;
            } else {
                mBombs[idx].unk_3e = 0;
            }
        }

        if (was == 0 && mBombs[idx].unk_3e != 0) {
            func_02012718(0x1e4, mBombs[idx].x);
        }

        mBombs[idx].unk_28 = func_02012468(mBombs[idx].unk_28, 2, 0x1e5, 4, 0, 0,
                                            func_020126e8(mBombs[idx].x), 0);
        mBombs[idx].prevX = mBombs[idx].x;
        mBombs[idx].prevY = mBombs[idx].y;
    }
}
#undef B

#pragma pop

// @symbol _ZN14dScMgBomroom_c19func_ov006_020d816cEi
/* While the bomb's +0x32 timer runs it counts down (unless +0x62fb is
 * set) and the bomb moves along its angle; once it reaches 0 the bomb goes
 * to state 4 and func_ov006_020d68a8 takes over. */

void dScMgBomroom_c::func_ov006_020d816c(int idx) {
    char * self = (char *)this;
    int t;

    if (mBombs[idx].unk_32 != 0) {
        if (unk_62fb == 0) {
            mBombs[idx].unk_32--;
        }
        if ((short)mBombs[idx].unk_32 < 0) {
            mBombs[idx].unk_32 = 0;
        }
        if (mBombs[idx].unk_32 <= 0x40) {
            mBombs[idx].type = 2;
        }
    } else {
        mBombs[idx].unk_32 = 0;
        mBombs[idx].state = 4;
        mBombs[idx].type = 3;
        mBombs[idx].frame = 0;
        mBombs[idx].counter = 0;
        mBombs[idx].pen = 0;
        mBombs[idx].unk_3d = 1;
        func_ov006_020d68a8(idx);
        return;
    }

    if (unk_62fb != 0) {
        return;
    }

    t = data_02082214[((int)mBombs[idx].angle >> 4) * 2 + 1];
    mBombs[idx].x += (int)(((long long)t * mBombs[idx].speed + 0x800) >> 12);
    t = data_02082214[((int)mBombs[idx].angle >> 4) * 2];
    mBombs[idx].y += (int)(((long long)t * mBombs[idx].speed + 0x800) >> 12);
    func_ov006_020d6e8c(idx);
    func_ov006_020d6d7c(idx);
    mBombs[idx].prevX = mBombs[idx].x;
    mBombs[idx].prevY = mBombs[idx].y;
}


// @symbol _ZN14dScMgBomroom_c19func_ov006_020d8324Ei

void dScMgBomroom_c::func_ov006_020d8324(int i) {
    char * c = (char *)this;
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


// @symbol _ZN14dScMgBomroom_c19func_ov006_020d836cEv
/* Per-frame bomb pass: with no touch, nothing is held (0x62f6 = 0xff);
 * then every live bomb runs its state handler from the pointer-to-member
 * table data_ov006_02141730, indexed by +0x697, and func_ov006_020d69b8. */
#pragma push
#pragma opt_strength_reduction off

typedef void (dScMgBomroom_c::*PMF)(int);
extern PMF data_ov006_02141730[];
void dScMgBomroom_c::func_ov006_020d836c() {
    char * c = (char *)this;
    if (*(u8 *)(gTouchHeld + gActivePlayerSlot) == 0)
        mHeldColor = 0xff;
    int i;
    for (i = 0; i < 0x70; i++) {
        u8 *o = (u8 *)c + i * 0x40;
        u8 flag;
        o += 0x4000;
        flag = o[0x698];
        if (flag != 0) {
            (this->*data_ov006_02141730[o[0x697]])(i);
            func_ov006_020d69b8(i);
        }
    }
}

#pragma pop

// @symbol _ZN14dScMgBomroom_c19func_ov006_020d8408Ev
/* Spawner: once the +0x62e2 delay runs out, picks a step from the spawn
 * count at +0x62d8, drops that many new bombs into free records with a
 * random color and angle, and reloads the delay from
 * data_ov006_0212e2e8. */
#pragma push
#pragma opt_strength_reduction off
#pragma opt_common_subs off
#pragma opt_loop_invariants off

void dScMgBomroom_c::func_ov006_020d8408()
{
    char * c = (char *)this;
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
  if (mSpawnDelay != 0)
  {
    (*((u16 *) ( ((int) (c + 0x62e2)))))--;
    if ((s16)mSpawnDelay < 0)
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
            func_ov006_020d66c4(z1);
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
              func_ov006_020d66c4(z2);
            }
            else
            {
              *((s16 *) ((c + (j << 6)) + 0x468c)) = ang + 0xa000;
              *((s32 *) ((c + (j << 6)) + 0x4664)) = vB8000;
              func_ov006_020d66c4(one);
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
            func_ov006_020d66c4(z3);
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
            func_ov006_020d66c4(one);
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
  mSpawnDelay = data_ov006_0212e2e8[sb];
}

#pragma pop

// @symbol _ZN14dScMgBomroom_c19func_ov006_020d8904Ev

void dScMgBomroom_c::func_ov006_020d8904() {
    char * p = (char *)this;
    int i;
    for (i = 0; i < 0x70; i++) {
        if (mBombs[i].active != 0) {
            if (mBombs[i].state == 6)
                mBombs[i].unk_39 = 0;
        }
    }
}


// @symbol _ZN14dScMgBomroom_c19func_ov006_020d893cEv
/* Zeroes all 0x70 bomb records (stride 0x40 at +0x4660): 11 words, 4
 * halfwords and 10 bytes each. */

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

void dScMgBomroom_c::func_ov006_020d893c() {
    int i;
    for (i = 0; i < 0x70; i++) {
        mBombs[i].x = 0;
        mBombs[i].y = 0;
        mBombs[i].grabX = 0;
        mBombs[i].grabY = 0;
        mBombs[i].speed = 0;
        mBombs[i].unk_14 = 0;
        mBombs[i].unk_18 = 0;
        mBombs[i].prevX = 0;
        mBombs[i].prevY = 0;
        mBombs[i].sound = 0;
        mBombs[i].unk_28 = 0;
        mBombs[i].angle = 0;
        mBombs[i].counter = 0;
        mBombs[i].unk_30 = 0;
        mBombs[i].unk_32 = 0;
        mBombs[i].type = 0;
        mBombs[i].frame = 0;
        mBombs[i].color = 0;
        mBombs[i].state = 0;
        mBombs[i].active = 0;
        mBombs[i].substate = 0;
        mBombs[i].unk_39 = 0;
        mBombs[i].pen = 0;
        mBombs[i].unk_3c = 0;
        mBombs[i].unk_3d = 0;
    }
}


// @symbol _ZN14dScMgBomroom_c19func_ov006_020d89c4Ev
#pragma push
#pragma opt_strength_reduction off

void dScMgBomroom_c::func_ov006_020d89c4() {
    char * self = (char *)this;
    int j;

    func_ov006_020d836c();

    if (mStateTimer != 0) {
        (mStateTimer)--;
        if ((s16)mStateTimer < 0)
            mStateTimer = 0;
        return;
    }

    for (j = 0; j < 0x70; j++) {
        if (mBombs[j].active == 2) {
            if (mBombs[j].state == 6
             && mBombs[j].substate == 4) {
                mBombs[j].active = 0;
                mBombs[j].unk_39 = 0;
            }
        }
    }

    if (mBlastPen != 0) {
        mStateTimer = 0x60;
        mSceneState = 4;
        mSubState = 0;
        func_ov004_020adb1c(unk_62ee + func_ov004_020adbc0());
    } else {
        func_ov004_020adb1c(unk_62ee + func_ov004_020adbc0());
        mSceneState = 2;
        mSubState = 0;
        mBlastPen = 0;
        unk_62fb = 0;
    }
    unk_62f9 = 0;
    unk_62ee = 0;
    SetBg0Offset(0, 0);
}

#pragma pop

// @symbol _ZN14dScMgBomroom_c19func_ov006_020d8af8Ev

void dScMgBomroom_c::func_ov006_020d8af8() {
    char * self = (char *)this;
    int found;
    int j;
    u8 (*arr)[0x40] = (u8 (*)[0x40])self;

    func_ov006_020d836c();

    if (mStateTimer != 0) {
        (mStateTimer)--;
        if ((s16)mStateTimer > 0)
            return;
        mStateTimer = 0;
        unk_62e0 = 0x10;
        if (func_ov006_020d8c88() != 0) {
            mBg2State = 1;
            Sound::PlayBank2_2D(0x1dd);
        }
        return;
    }

    if (unk_62e0 != 0) {
        (unk_62e0)--;
        return;
    }

    found = 0;
    j = 0;
    do {
        if (mBombs[j].active == 1 && mBombs[j].state == 6 && mBombs[j].substate == 4)
            found = j + 1;
        j++;
    } while (j < 0x70);

    (unk_62ea)++;
    if (unk_62ea < 4)
        return;

    if (found != 0) {
        mBombs[found - 1].active = 2;
        unk_62ea = 0;
        (unk_62ee)++;
        Sound::PlayBank2_2D(0x1bc);
    } else {
        unk_62ea = 0;
        mStateTimer = 0x40;
        mSubState = 3;
    }
}


// @symbol _ZN14dScMgBomroom_c19func_ov006_020d8c88Ev

int dScMgBomroom_c::func_ov006_020d8c88() {
    char * c = (char *)this;
    int i;
    unsigned char (*arr)[0x40];
    i = 0;
    arr = (unsigned char (*)[0x40])c;
    do {
        if (mBombs[i].active == 1 && mBombs[i].state == 6)
            return 1;
        i++;
    } while (i < 0x70);
    return 0;
}


// @symbol _ZN14dScMgBomroom_c19func_ov006_020d8cc4Ev
#pragma push
#pragma opt_strength_reduction off

void dScMgBomroom_c::func_ov006_020d8cc4() {
    char * r5 = (char *)this;
    func_ov006_020d836c();
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
        func_ov006_020d7604();
    }
    if (c4 != 0) return;
    func_ov006_020d5e1c();
    mStateTimer = 0x10;
    *(int *)(r5 + 0x6000 + 0x2d4) = 2;
    *(unsigned char *)(r5 + 0x6000 + 0x2f9) = 1;
}

#pragma pop

// @symbol _ZN14dScMgBomroom_c19func_ov006_020d8d84Ev

void dScMgBomroom_c::func_ov006_020d8d84() {
    char * self = (char *)this;
    int flag;
    int count;
    int i;
    u8 (*arr)[0x40] = (u8 (*)[0x40])self;
    int (*iarr)[0x10] = (int (*)[0x10])self;

    func_ov006_020d836c();

    if (mStateTimer != 0) {
        (mStateTimer)--;
        if ((s16)mStateTimer < 0)
            mStateTimer = 0;
        return;
    }

    if (mBlastPen != 0) {
        flag = 0;
        count = 0;
        i = 0;
        do {
            if (mBombs[i].active != 0 && mBombs[i].state != 6) {
                mBombs[i].state = 6;
                mBombs[i].substate = 0;
                iarr[i][0x119c] = 0x4000;
                if (mBombs[i].color != 0)
                    flag = 1;
                count++;
            }
            i++;
        } while (i < 0x70);
        if (count != 0) {
            Sound::PlayBank2_2D(0x1e6);
            if (mBlastPen == 1) {
                func_ov006_020d634c(0);
                func_ov006_020d634c(1);
            } else {
                func_ov006_020d634c(flag);
            }
        }
    } else {
        i = 0;
        do {
            if (mBombs[i].active != 0 && mBombs[i].state == 5) {
                if (mWinColor == mBombs[i].color) {
                    mBombs[i].state = 6;
                    mBombs[i].substate = 0;
                    iarr[i][0x119c] = 0x4000;
                }
            }
            i++;
        } while (i < 0x70);
        Sound::PlayBank2_2D(0x1e6);
        func_ov006_020d634c(mWinColor);
    }
    mSubState = 1;
}


// @symbol _ZN14dScMgBomroom_c19func_ov006_020d8f34Ev

void dScMgBomroom_c::func_ov006_020d8f34() {
    char * c = (char *)this;
    if (mStateTimer == 0)
        return;
    mStateTimer -= 1;
    if ((s16)mStateTimer > 0)
        return;
    mStateTimer = 0;
    func_ov004_020b0a54(0x10);
    *(u8 *)(c + 0xc3) = 0;
}


// @symbol _ZN14dScMgBomroom_c19func_ov006_020d8f98Ev
/* Runs the +0x62d4 sub-state through the pointer-to-member table
 * data_ov006_021416a0, decoded by hand (a function word, then a
 * this-adjust whose low bit marks a virtual call), then the three
 * per-frame passes. */

void dScMgBomroom_c::func_ov006_020d8f98() {
    unsigned char * c = (unsigned char *)this;
    int idx = mSubState;
    int *e = &data_ov006_021416a0[idx * 2];
    int off = e[1];
    void *obj = c + (off >> 1);
    void (*f)(void *);
    if (off & 1) f = (void (*)(void *))(*(int *)(*(int *)obj + e[0]));
    else f = (void (*)(void *))e[0];
    f(obj);
    func_ov006_020d65c8();
    func_ov006_020d6278();
    func_ov006_020d5fec();
}


// @symbol _ZN14dScMgBomroom_c19func_ov006_020d8ff4Ev

void dScMgBomroom_c::func_ov006_020d8ff4() {
    void * c = (void *)this;
    func_ov006_020d65c8();
    func_ov006_020d8408();
    func_ov006_020d836c();
    func_ov006_020d6784();
}


// @symbol _ZN14dScMgBomroom_c19func_ov006_020d9020Ev

void dScMgBomroom_c::func_ov006_020d9020() {
    void * c = (void *)this;
    unsigned char *p = (unsigned char *)c;
    if (p[0xc4] == 0) { p[0xc3] = 1; p[0xc4] = 1; *(short *)(p + 0xc0) = 0; }
    *(int *)(p + 0x6000 + 0x2d0) = 2;
}


// @symbol _ZN14dScMgBomroom_c19func_ov006_020d904cEv

void dScMgBomroom_c::func_ov006_020d904c() {
    void * c = (void *)this;
    func_ov006_020d6630();
    func_ov006_020d62e0();
    func_ov006_020d604c();
    *(int *)((char *)c + 0x6000 + 0x2d0) = 1;
}


// @symbol _ZN14dScMgBomroom_c19func_ov006_020d907cEv
/* Resets the round: clears every bomb record and the round counters
 * between +0x62d8 and +0x62fc, then runs the other reset helpers. */

void dScMgBomroom_c::func_ov006_020d907c() {
    void * p = (void *)this;
    char *c = (char *)p;
    func_ov006_020d893c();
    mSpawnDelay = 0;
    unk_62e4 = 0;
    unk_62e6 = 0;
    mHeldColor = 0xff;
    unk_62f7 = 0;
    unk_62ea = 0;
    mBlastPen = 0;
    mSpawnCount = 0;
    unk_62f0 = 0;
    unk_62f9 = 0;
    unk_62fb = 0;
    *(unsigned char *)(c + 0x62fc) = 0;
    func_ov004_020adb1c(0);
    func_ov006_020d669c();
    func_ov006_020d63ac();
    func_ov006_020d6084();
    func_ov006_020d5dd4();
    func_ov006_020d5c60();
    func_ov006_020d5b00();
}


/* Slot 18 override of dScMgBase_c::OnYoshiTryEat(int); the signature
 * repeats the base declaration exactly, or mwcc appends a slot instead of
 * overriding. */
// @symbol _ZN14dScMgBomroom_c13OnYoshiTryEatEi
void dScMgBomroom_c::OnYoshiTryEat(int /* arg */)
{
    unsigned char *c = (unsigned char *)this;

    func_ov006_020d907c();
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
    func_ov006_020d6098();
    func_ov006_020d5e5c();
    func_ov006_020d672c();
    func_ov006_020d7524();
    func_ov006_020d63d4();
    func_ov006_020d5c88();
    func_ov006_020d5ab0();
    return 1;
}

// @symbol _ZN14dScMgBomroom_c8BehaviorEv
/* Waits out the +0x62f0 delay, then runs the round state at +0x62d0 from
 * the pointer-to-member table data_ov006_021416e0 and steps the sprite
 * records and the BG2 offset. */
typedef void (dScMgBomroom_c::*PMFv)();
extern "C" PMFv data_ov006_021416e0[];
s32 dScMgBomroom_c::Behavior()
{
    char *c = (char *)this;
    if (*(unsigned short *)(c + 0x6200 + 0xf0) != 0) {
        unsigned short *t = (unsigned short *)(((int)c + 0x62f0));
        *t = *t - 1;
        if (*(short *)(c + 0x6200 + 0xf0) <= 0)
            *(short *)(c + 0x6200 + 0xf0) = 0;
    } else {
        (this->*data_ov006_021416e0[*(int *)(c + 0x6000 + 0x2d0)])();
        func_ov006_020d5d08();
        func_ov006_020d5b10();
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

    func_ov006_020d907c();
    func_ov006_020d6630();
    func_ov006_020d62e0();
    func_ov006_020d604c();
    *(int *)(c + 0x6000 + 0x2d0) = 1;
    *(u16 *)(c + 0x6200 + 0xee) = 0;
    func_ov004_020b04d0(0x20);
    func_ov004_020adb1c(0);
    return 1;
}

/* Reconstructed source-style name: SM64DS proves dScMgBomroom_c through RTTI,
 * allocation size, vtable identity, and the MG_BOMROOM registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: MgSortOrSplode_Spawn. */
// @symbol dScMgBomroom_c_classInit
extern "C" void *dScMgBomroom_c_classInit()
{
    return new dScMgBomroom_c;
}
