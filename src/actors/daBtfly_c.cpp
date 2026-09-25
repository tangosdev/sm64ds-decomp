//cpp
/* daBtfly_c -- the butterfly of ov100. One translation unit, 15 functions,
 * .text 0x02140d80..0x02141ea4.
 *
 * FUNCTION ORDER HERE IS ROM-ASCENDING, WHICH IS THE OPPOSITE OF THE USUAL
 * CONVENTION FOR THESE FILES, AND IT IS DELIBERATE. mwccarm 2004/b56 normally
 * emits one .text section per function in the REVERSE of source order, which
 * is why most reconstructed TUs are written highest-ROM-address first. This TU carries
 * `#pragma defer_codegen off` (see below): State2 needs a per-member
 * `#pragma opt_common_subs off`, and without defer_codegen off that bracket
 * does not bind -- the last file-global state wins for every member and State2
 * comes out 8 bytes short. Turning deferred codegen off makes the bracket
 * bind, and it also makes the compiler emit sections in FORWARD source order.
 * So the source is ROM-ascending to compensate. Verified: all 15 functions
 * land in ROM-ascending section order, including the destructor pair, which
 * the reverse-order convention cannot achieve. Do not reorder either of these
 * two things without the other.
 *
 * The 15 members below were each byte-matched on their own before being folded
 * here. In ROM address order they are ~daBtfly_c (D1 at 0x02140d80, D0 at
 * 0x02140dd8), State7, State6, State5, State4, State3, State2, State1, State0,
 * CleanupResources, OnPendingDestroy, Render, Behavior, and InitResources at
 * 0x02141c6c. Per-symbol credit for each is preserved in attribution.json
 * through `path#symbol` overrides, and the manifest entry
 * config/tu_manifest.d/ov100/daBtfly_c.json keeps each one's pre-promotion
 * source path, address and size.
 *
 * daBtfly_c_classInit (0x02141ea4) and g_profile_BUTTERFLY (0x02147e78) sit
 * just past this TU's evidenced run and are not part of it yet; the manifest
 * records why that is what blocks licensing this class's .data.
 */

/* PRAGMAS, BOTH LOAD-BEARING -- see the manifest notes.
 * `#pragma opt_common_subs off` / `... on` brackets State2 below, carried from
 * the one-function source that used to hold _ZN9daBtfly_c6State2Ev. Under
 * mwccarm's default deferred codegen that directive is FILE-GLOBAL last-wins,
 * so the bracket alone does nothing; the file-global `#pragma defer_codegen
 * off` after the includes is what makes it bind. Without the pair this TU is
 * 14/15 -- State2 compiles 8 bytes short of its 0x244.
 */

/* Includes: the union of the absorbed sources', first-seen in ROM-ascending
 * processing order. */
#include "daBtfly_c.h"
#include "types.h"
#include "common.h"
#include "decl_common.h"
#include "decl_Actor.h"
#include "SharedFilePtr.h"

/* Makes the per-member `#pragma opt_common_subs` bracket around State2 below
 * actually bind; without it mwccarm defers codegen and the last file-global
 * state wins for every member. */
#pragma defer_codegen off

/* Local declarations carried from the legacy files verbatim; not yet
 * reconciled against the project headers. */
typedef short s16;

struct Vector3;

struct Mtx { int w[12]; };

typedef void (daBtfly_c::*ButterflyState)();

struct Vec3 { s32 x, y, z; };

#define L(p) ((int)(p))
#define M(p) (p)

extern "C" {
void _Z14ApproachLinearRiii(int* p, int a, int b);
void _Z14ApproachLinearRsss(s16* p, s16 a, s16 b);
s16 Vec3_VertAngle(void* v1, void* v0);
void dBgCh_Actr_UpdateContinuous_Veneer(void* c);
void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void* self, struct Vector3* pos, unsigned int a, int b, unsigned int c, unsigned int d, unsigned int e);
extern s16 data_02082214[];
extern void _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(void *c, int a, int b, int d, int e);
extern s16 Vec3_HorzAngle(const void* a, const void* b);
extern int RandomIntInternal(int* seed);
extern int data_0209e650;
extern short data_02082214[];
extern void Vec3_Sub(struct Vector3 *out, void *a, void *b);
extern int Vec3_HorzLen(struct Vector3 *v);
extern SharedFilePtr data_ov100_02148608;
extern SharedFilePtr data_ov100_02148600;
extern SharedFilePtr data_ov002_0210d9d8;
extern SharedFilePtr data_ov100_02148668;
void Vec3_Asr(void* d, void* s, int sh);
void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToRotationY(void* m, s16 ang);
void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
void* a, void* sm, void* m, int r, int h, unsigned int f);
extern struct Mtx data_020a0e68;
extern "C" ButterflyState data_ov100_02148628[];
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* ma, void* bca, int a, int f, u32 e);
void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void* w, void* a, int r, int rr, void* v, void* vv);
void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(void* c, void* a, void* pos, int r, int rr, u32 e, u32 f);
u32 __aeabi_uidiv(u32 a, u32 b);
/* TUBUILD CONFLICT -- alternate declaration of _ZN7fBase_c18MarkForDestructionEv, from the legacy file for _ZN9daBtfly_c6State6Ev, NOT applied: extern "C" void _ZN7fBase_c18MarkForDestructionEv(void*); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN7fBase_c18MarkForDestructionEv, from the legacy file for _ZN9daBtfly_c6State5Ev, NOT applied: extern void _ZN7fBase_c18MarkForDestructionEv(void* c); */
/* TUBUILD CONFLICT -- alternate declaration of _Z14ApproachLinearRiii, from the legacy file for _ZN9daBtfly_c6State5Ev, NOT applied: extern void _Z14ApproachLinearRiii(int* val, int target, int step); */
/* TUBUILD CONFLICT -- alternate declaration of _Z14ApproachLinearRsss, from the legacy file for _ZN9daBtfly_c6State5Ev, NOT applied: extern void _Z14ApproachLinearRsss(s16* val, s16 target, s16 step); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN8dActor_c13DistToCPlayerEv, from the legacy file for _ZN9daBtfly_c6State4Ev, NOT applied: extern int _ZN8dActor_c13DistToCPlayerEv(void* self); */
/* TUBUILD CONFLICT -- alternate declaration of RandomIntInternal, from the legacy file for _ZN9daBtfly_c6State4Ev, NOT applied: extern unsigned int RandomIntInternal(void* g); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as, from the legacy file for _ZN9daBtfly_c6State4Ev, NOT applied: extern int _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned int a, unsigned int b, const struct Vector3* pos, const struct Vector3_16* rot, int e, int f); */
/* TUBUILD CONFLICT -- alternate declaration of Vec3_HorzAngle, from the legacy file for _ZN9daBtfly_c6State3Ev, NOT applied: extern short Vec3_HorzAngle(const void *a, const void *b); */
/* TUBUILD CONFLICT -- alternate declaration of Vec3_VertAngle, from the legacy file for _ZN9daBtfly_c6State3Ev, NOT applied: extern short Vec3_VertAngle(const void *a, const void *b); */
/* TUBUILD CONFLICT -- alternate declaration of _Z14ApproachLinearRsss, from the legacy file for _ZN9daBtfly_c6State3Ev, NOT applied: extern void _Z14ApproachLinearRsss(short *dst, short target, short step); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN8dActor_c13ClosestPlayerEv, from the legacy file for _ZN9daBtfly_c6State2Ev, NOT applied: extern void *_ZN8dActor_c13ClosestPlayerEv(void *self); */
/* TUBUILD CONFLICT -- alternate declaration of Vec3_HorzAngle, from the legacy file for _ZN9daBtfly_c6State2Ev, NOT applied: extern short Vec3_HorzAngle(const void *a, const void *b); */
/* TUBUILD CONFLICT -- alternate declaration of Vec3_VertAngle, from the legacy file for _ZN9daBtfly_c6State2Ev, NOT applied: extern short Vec3_VertAngle(const void *a, const void *b); */
/* TUBUILD CONFLICT -- alternate declaration of _Z14ApproachLinearRsss, from the legacy file for _ZN9daBtfly_c6State2Ev, NOT applied: extern void _Z14ApproachLinearRsss(short *dst, short target, short step); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN8dActor_c15IsPlayerInRangeEi, from the legacy file for _ZN9daBtfly_c6State1Ev, NOT applied: int _ZN8dActor_c15IsPlayerInRangeEi(void* c, int n); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN8dActor_c13ClosestPlayerEv, from the legacy file for _ZN9daBtfly_c6State1Ev, NOT applied: void* _ZN8dActor_c13ClosestPlayerEv(void* c); */
/* TUBUILD CONFLICT -- alternate declaration of Vec3_HorzAngle, from the legacy file for _ZN9daBtfly_c6State1Ev, NOT applied: short Vec3_HorzAngle(const void* a, const void* b); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as, from the legacy file for _ZN9daBtfly_c6State0Ev, NOT applied: extern char* _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned int a, unsigned int b, Vector3* v, Vector3_16* w, int e, int f); */
/* TUBUILD CONFLICT -- alternate declaration of RandomIntInternal, from the legacy file for _ZN9daBtfly_c13InitResourcesEv, NOT applied: int RandomIntInternal(int* seed); */
/* TUBUILD CONFLICT -- alternate declaration of data_ov002_0210d9d8, from the legacy file for _ZN9daBtfly_c13InitResourcesEv, NOT applied: extern SFP data_ov002_0210d9d8; */
/* TUBUILD CONFLICT -- alternate declaration of data_ov100_02148600, from the legacy file for _ZN9daBtfly_c13InitResourcesEv, NOT applied: extern SFP data_ov100_02148600; */
/* TUBUILD CONFLICT -- alternate declaration of data_ov100_02148668, from the legacy file for _ZN9daBtfly_c13InitResourcesEv, NOT applied: extern SFP data_ov100_02148668; */
/* TUBUILD CONFLICT -- alternate declaration of data_ov100_02148608, from the legacy file for _ZN9daBtfly_c13InitResourcesEv, NOT applied: extern SFP data_ov100_02148608; */
}

// @symbol _ZN9daBtfly_cD1Ev
daBtfly_c::~daBtfly_c()
{
}
// @symbol _ZN9daBtfly_cD0Ev
/* (no separate definition: the single ~daBtfly_c() below emits the D0 and D1
 * variants together -- a hand-mangled D0 next to a real destructor is the
 * known mwccarm ICE, ELFgen.c:483.) */

// @symbol _ZN9daBtfly_c6State7Ev
void daBtfly_c::State7()
{
    char* c = (char*)this;
    void* player;
    int hasContact;
    int v;

    if (*(int*)(c + 0x3e8) > 0x78) {
        int ang = (short)*(s16*)(c + 0x3ee);
        ang = (unsigned short)(short)ang;
        int idx = ang >> 4;
        v = (int)(((s64)(int)data_02082214[(idx << 1) + 1] * 0xa3 + 0x800) >> 12);
        if (v > 0) {
            v = (int)(((s64)v * 0x4800 + 0x800) >> 12);
            *(u16*)L(c + 0x3ee) += 0x2710;
        } else {
            *(u16*)L(c + 0x3ee) += 0xfa0;
        }
        *(int*)L(c + 0x3e0) += v;
    }

    _Z14ApproachLinearRiii((int*)(c + 0x98), 0x14000, 0x1000);

    player = ((dActor_c *)c)->ClosestPlayer();
    if (player != 0) {
        _Z14ApproachLinearRsss((s16*)(c + 0x94), ((dActor_c *)c)->HorzAngleToCPlayer(), 0x320);
        _Z14ApproachLinearRsss((s16*)(c + 0x92), Vec3_VertAngle(c + 0x5c, (char*)player + 0x5c), 0x320);
    }

    dBgCh_Actr_UpdateContinuous_Veneer(c + 0x1d8);

    {
        int noId = (*(int*)(c + 0x3b8) == 0);
        hasContact = (noId == 0);
    }
    if (hasContact == 0) {
        if (((dBgCh_Actr *)(c + 0x1d8))->IsOnWall() == 0) {
            if (((dBgCh_Actr *)(c + 0x1d8))->IsOnGround() == 0) {
                if (func_02035638((u8*)(c + 0x1d8)) == 0) {
                    if (*(int*)(c + 0x3e8) <= 0x9d)
                        goto cylinder_only;
                }
            }
        }
    }

    if (hasContact != 0) {
        void* a = dActor_c::FindWithID(*(unsigned int*)(c + 0x3b8));
        if (a != 0) {
            int isPlayer = (*(u16*)((char*)a + 0xc) == 0xbf);
            if (isPlayer != 0) {
                struct Vector3 pos;
                pos.x = mPosX;
                pos.y = mPosY;
                pos.z = mPosZ;
                _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(a, &pos, 2, 0xc000, 1, 0, 1);
            }
        }
    }

    ((dActor_c *)c)->TriplePoofDust();
    ((fBase_c *)c)->fBase_c::MarkForDestruction();

cylinder_only:
    ((dCc_c *)(c + 0x394))->Clear();
    {
        struct Vector3 off;
        off.x = 0;
        off.y = -0x32000;
        off.z = 0;
        ((dCcAcPos_c *)(c + 0x394))->SetPosRelativeToActor(off);
    }
    ((dCc_c *)(c + 0x394))->dCc_c::Update();
}

// @symbol _ZN9daBtfly_c6State6Ev
void daBtfly_c::State6()
{
    char *c = (char*)this;
    int st = *(int*)(c+0x3e8);
    if (st == 0x14) {
        if (*(unsigned char*)(c+0x3f0) != 1) {
            *(int*)(c+0x3e0) = 0;
            *(unsigned char*)(c+0x3f1) = 0;
            return;
        }
        int sub = *(signed char*)(c+0xcc);
        dActor_c::Spawn(0x114, 0, *((struct Vector3*)(c+0x5c)), 0, sub, -1);
        ((fBase_c *)c)->fBase_c::MarkForDestruction();
        return;
    }
    if (st <= 0x14) return;
    (*(int *)(((int)c + 0x3e0))) += 0x40;
    if (*(int*)(c+0x3e0) < 0x800) return;
    _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(c, 0x32000, 0x32000, 0x1000000, 0x320000);
    *(short*)(c+0x3ee) = 0;
    *(int*)(c+0x3e8) = 0;
    *(int*)(c+0x3e4) = 7;
}

// @symbol _ZN9daBtfly_c6State5Ev
void daBtfly_c::State5()
{
    char* c = (char*)this;
    s16 hAngle;

    if (!((dActor_c *)c)->IsPlayerInRange(0x5dc)) {
        ((fBase_c *)c)->fBase_c::MarkForDestruction();
        return;
    }

    if (*(int*)(c + 0x3e8) > 0x6e && ((dActor_c *)c)->DistToCPlayer() < 0xc8000 &&
        (unsigned char)(*(unsigned char*)(c + 0x3f0) + 0xff) <= 1) {
        mHorzSpeed = 0;
        *(int*)(c + 0x3e8) = 0;
        *(int*)(c + 0x3e4) = 6;
        *(int*)(c + 0xb0) &= ~0x10000;
    } else {
        _Z14ApproachLinearRiii((int*)(c + 0x98), 0x8000, 0x800);
    }

    if (*(int*)(c + 0x3e8) >= 0x3c) {
        hAngle = *(s16*)(c + 0x3ec);
    } else {
        hAngle = Vec3_HorzAngle((Vector3*)(c + 0x5c), (Vector3*)(c + 0x3d4));
    }

    {
        s16 target;
        if (mPosY < mHomePosY +
                (int)(((unsigned)RandomIntInternal(&data_0209e650) >> 16 & 0xfff) * 0x32 + 0x32000)) {
            target = -0x2000;
        } else {
            target = 0x2000;
        }
        _Z14ApproachLinearRsss((s16*)(c + 0x92), target, 0x190);
    }

    {
        _Z14ApproachLinearRsss((s16*)(c + 0x94), hAngle,
            (s16)((((unsigned)RandomIntInternal(&data_0209e650) >> 16) % 800) + 0x190));
    }
}

// @symbol _ZN9daBtfly_c6State4Ev
void daBtfly_c::State4()
{
    struct StateRotation { u16 w[3]; };
    char* sl = (char*)this;
    int typ = *(int*)(sl + 8) & 0xff;
    if ((typ & 0xc0) != 0) {
        *(u8*)(sl + 0x3f0) = (u8)(typ >> 6);
    } else {
        if (((dActor_c *)sl)->DistToCPlayer() >= 0xc8000)
            return;
        {
            int sb = (int)((unsigned int)RandomIntInternal(&data_0209e650) % 3);
            int sel;
            int i;
            int mask = typ & 0x30;
            struct StateRotation rot;
            int two = 2;
            int three = 3;
            i = 1;
            rot = *(struct StateRotation*)(sl + 0x92);
            for (; i < 3; i++) {
                if (i == sb) {
                    sel = 1;
                } else if (mask == 0x20) {
                    sel = two;
                } else {
                    sel = three;
                }
                rot.w[1] = (s16)((s16)rot.w[1] + (s16)((unsigned int)RandomIntInternal(&data_0209e650) >> 0x10));
                dActor_c::Spawn(0x150, mask | (sel << 6), *((struct Vector3*)(sl + 0x5c)), (struct Vector3_16*)&rot, (int)*(s8*)(sl + 0xcc), -1);
            }
            if (sb == 0) {
                *(u8*)(sl + 0x3f0) = 1;
            } else if (mask == 0x20) {
                *(u8*)(sl + 0x3f0) = 2;
            } else {
                *(u8*)(sl + 0x3f0) = 3;
            }
        }
    }

    *(s16*)(sl + 0x300 + 0xec) = (s16)((unsigned int)RandomIntInternal(&data_0209e650) >> 0x10);
    mPrevAngleY = (s16)(*(s16*)(sl + 0x300 + 0xec) + (int)(((unsigned int)RandomIntInternal(&data_0209e650) >> 0x10) & 0x3fff));
    *(int*)(sl + 0x98) = (int)(((unsigned int)RandomIntInternal(&data_0209e650) >> 0x10) & 0xfff) * 0xf + 0xf000;
    *(int*)(sl + 0x3e8) = 0;
    *(int*)(sl + 0x3e4) = 5;
}

// @symbol _ZN9daBtfly_c6State3Ev
void daBtfly_c::State3()
{
    char *c = (char*)this;
    int *p;

    _Z14ApproachLinearRsss((short *)(c + 0x94),
        Vec3_HorzAngle(c + 0x5c, c + 0x3d4), 0x800);
    _Z14ApproachLinearRsss((short *)(c + 0x92),
        Vec3_VertAngle(c + 0x5c, c + 0x3d4), 0x50);
    ((dActor_c *)c)->UpdatePos(0);

    p = (int *)(c + 0x60);
    *p = *p - ((int)(((long long)mHorzSpeed
        * data_02082214[(*(unsigned short *)(c + 0x92) >> 4) * 2] + 0x800) >> 12)
        + (short)data_02082214[
        ((unsigned short)(short)((*(int *)(c + 0x3e8) << 16) / 100) >> 4) * 2 + 1]
        * (short)20 / 4);

    {
        int *cnt = (int *)(c + 0x3e8);
        *cnt = *cnt + 1;
        if (*(int *)(c + 0x3e8) > 100)
            *(int *)(c + 0x3e8) = 0;
    }

    if (((dActor_c *)c)->IsPlayerInRange(0xbb8))
        return;

    mPosX = mHomePosX;
    mPosY = mHomePosY;
    mPosZ = mHomePosZ;
    *(int *)(c + 0x3e4) = 1;
}

#pragma opt_common_subs off   /* carried from the legacy _ZN9daBtfly_c6State2Ev source */
// @symbol _ZN9daBtfly_c6State2Ev
void daBtfly_c::State2()
{
    char *c = (char*)this;
    char *player;
    struct Vector3 v;
    struct Vector3 d;

    mPosX = mHomePosX;
    mPosY = mHomePosY;
    mPosZ = mHomePosZ;

    player = (char *)((dActor_c *)c)->ClosestPlayer();
    if (player != 0) {
        int *pp;
        int k = 0x5000;
        int py;
        int t;
        int five = 5;
        Vec3_Sub(&d, c + 0x5c, player + 0x5c);
        v.x = d.x;
        v.y = d.y;
        v.z = d.z;
        if (Vec3_HorzLen(&v) > 0x4b0000)
            *(int *)(c + 0x3e4) = 3;

        pp = (int *)(int)M(player + 0x5c);
        mPosX = mPrevPosX;
        mPosY = mPrevPosY;
        mPosZ = mPrevPosZ;

        v.x = pp[0];
        v.y = pp[1];
        v.z = pp[2];
        t = *(int *)(c + 0x3e8) * k;
        v.x = v.x + t / 4;
        t = *(int *)(c + 0x3e8) * k;
        v.z = v.z + t / 4;
        _Z14ApproachLinearRsss((short *)(c + 0x94),
            Vec3_HorzAngle(c + 0x5c, &v), 0x300);

        pp = (int *)(int)M(player + 0x5c);
        v.x = pp[0];
        py = pp[1];
        v.y = py;
        v.z = pp[2];
        v.y = py + ((*(int *)(c + 0x3e8) * five + 0x100) << 12) / 4;
        _Z14ApproachLinearRsss((short *)(c + 0x92),
            Vec3_VertAngle(c + 0x5c, &v), 0x500);

        ((dActor_c *)c)->UpdatePos(0);

        {
        int *p = (int *)(c + 0x60);
        *p = *p - ((int)(((long long)mHorzSpeed
            * data_02082214[(*(unsigned short *)(c + 0x92) >> 4) * 2] + 0x800) >> 12)
            + (short)data_02082214[
            ((unsigned short)(short)((*(int *)(c + 0x3e8) << 16) / 100) >> 4) * 2 + 1]
            * (short)20 / 4);
        int *cnt = (int *)(c + 0x3e8);
        *cnt = *cnt + 1;
        if (*(int *)(c + 0x3e8) > 100)
            *(int *)(c + 0x3e8) = 0;
        }
        return;
    }
    mPosX = mPrevPosX;
    mPosY = mPrevPosY;
    mPosZ = mPrevPosZ;
}

#pragma opt_common_subs on

// @symbol _ZN9daBtfly_c6State1Ev
void daBtfly_c::State1(){
    char* c = (char*)this;
    if(((dActor_c *)c)->IsPlayerInRange(0x3e8) == 0) return;
    *(int*)(c+0x3e4) = 2;
    void* p = ((dActor_c *)c)->ClosestPlayer();
    *(short*)(c+0x94) = Vec3_HorzAngle(c+0x5c, (char*)p+0x5c);
}

// @symbol _ZN9daBtfly_c6State0Ev
void daBtfly_c::State0(){
  char* c = (char*)this;
  struct StateRotation { u16 w[3]; };
  StateRotation rot;
  int n;
  int i;
  n = (*(int*)(c+8) & 0xf) - 1;
  rot = *(StateRotation*)(c+0x92);
  i = 0;
  if (n > 0) {
    do {
    Vector3 pos;
    int r;
    r = RandomIntInternal(&data_0209e650);
    pos.x = ((int)((unsigned int)r % 20) - 0xa) * 0xa000 + *(int*)(c+0x5c);
    r = RandomIntInternal(&data_0209e650);
    pos.y = *(int*)(c+0x60) + ((int)((unsigned int)r % 40) << 13);
    r = RandomIntInternal(&data_0209e650);
    pos.z = ((int)((unsigned int)r % 20) - 0xa) * 0xa000 + *(int*)(c+0x64);
    r = RandomIntInternal(&data_0209e650);
    rot.w[1] = (s16)((s16)rot.w[1] + ((r << 1) >> 16));
    dActor_c::Spawn(0x150, 0, pos, (Vector3_16*)&rot, *(signed char*)(c+0xcc), -1);
      i++;
    } while (i < n);
  }
  *(int*)(c+0x3e4) = 1;
}

// @symbol _ZN9daBtfly_c16CleanupResourcesEv
/* daBtfly_c::CleanupResources -- vtable slot 3. Releases the four shared files
 * the class holds -- three of its own in ov100 and one shared with the other
 * small fauna in ov002; it never touches `this`. */
int daBtfly_c::CleanupResources()
{
    data_ov100_02148608.Release();
    data_ov100_02148600.Release();
    data_ov002_0210d9d8.Release();
    data_ov100_02148668.Release();
    return 1;
}

// @symbol _ZN9daBtfly_c16OnPendingDestroyEv
/* daBtfly_c::OnPendingDestroy -- vtable slot 12. The ROM body is empty: the
 * override exists only to occupy the slot. */
void daBtfly_c::OnPendingDestroy()
{
}

// @symbol _ZN9daBtfly_c6RenderEv
int daBtfly_c::Render()
{
  if(mState == 4) return 1;
  if(mUseAnimModel != 0){
    mModelAnim.Render(0);
  } else {
    mModel.Render((Vector3*)&mScaleX);
  }
  return 1;
}

// @symbol _ZN9daBtfly_c8BehaviorEv
int daBtfly_c::Behavior()
{
    char* c = (char*)this;
    (this->*data_ov100_02148628[mState])();

    if (*(unsigned char*)(c + 0x3f0) != 0) {
        int spd = *(int*)(c + 0x3e0);
        *(int*)(c + 0x80) = spd;
        *(int*)(c + 0x84) = spd;
        *(int*)(c + 0x88) = spd;

        {
            int s = mHorzSpeed;
            int idx = (*(unsigned short*)(c + 0x92) >> 4) << 1;
            long long p;
            p = (long long)(-(int)data_02082214[idx]) * s;
            *(int*)(c + 0xa8) = (int)((p + 0x800) >> 0xc);
            p = (long long)(int)data_02082214[idx + 1] * s;
            int horiz = (int)((p + 0x800) >> 0xc);
            {
                int idx2 = (*(unsigned short*)(c + 0x94) >> 4) << 1;
                p = (long long)horiz * (int)data_02082214[idx2];
                *(int*)(c + 0xa4) = (int)((p + 0x800) >> 0xc);
            }
            {
                int idx3 = ((*(unsigned short*)(c + 0x94) >> 4) << 1) + 1;
                p = (long long)horiz * (int)data_02082214[idx3];
                *(int*)(c + 0xac) = (int)((p + 0x800) >> 0xc);
            }
        }
        ((dActor_c *)c)->UpdatePosWithOnlySpeed(0);
        (*(int*)(((int)c + 0x3e8)))++;
    }

    if (*(int*)(c + 0x3e4) != 4) {
        int t[3];
        Vec3_Asr(t, c + 0x5c, 3);
        Matrix4x3_FromTranslation(&data_020a0e68, t[0], t[1], t[2]);
        mAngleY = mPrevAngleY;
        Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mAngleY);
        if (*(unsigned char*)(c + 0x3f1) != 0) {
            *(struct Mtx*)(c + 0xf0) = data_020a0e68;
            _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
                c, c + 0x188, c + 0xf0, 0x14000, 0x12c000, 0xf);
            ((Animation *)(c + 0x124))->Advance();
        } else {
            *(struct Mtx*)(c + 0x154) = data_020a0e68;
            _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
                c, c + 0x1b0, c + 0x154, 0x64000, 0x12c000, 0xf);
        }
    }
    return 1;
}

// @symbol _ZN9daBtfly_c13InitResourcesEv
int daBtfly_c::InitResources()
{
    u8* c = (u8*)((void*)this);
    Model::LoadFile(data_ov002_0210d9d8);
    Animation::LoadFile(data_ov100_02148600);
    Model::LoadFile(data_ov100_02148668);
    void* bmd = Model::LoadFile(data_ov100_02148608);

    if (((ModelBase *)((void*)(c+0xd4)))->ModelBase::SetFile((BMD_File *)bmd, 1, 1) == 0) return 0;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((void*)(c+0xd4), ((void**)&data_ov100_02148600)[1], 0, 0x1000, 0);
    if (((ShadowModel *)((void*)(c+0x188)))->InitCylinder() == 0) return 0;
    if (((ModelBase *)((void*)(c+0x138)))->ModelBase::SetFile((BMD_File *)(((void**)&data_ov100_02148668)[1]), 1, 1) == 0) return 0;
    if (((ShadowModel *)((void*)(c+0x1b0)))->InitCylinder() == 0) return 0;

    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_((void*)(c+0x1d8), ((void*)this), 0x32000, 0x32000, 0, 0);

    Vec3 v;
    v.x = 0;
    v.y = -0x32000;
    v.z = 0;
    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj((void*)(c+0x394), ((void*)this), &v, 0x32000, 0x64000, 0x200000, 0);

    int sub = (int)(u8)(*(u32*)(c+8) & 0x30);
    if (sub != 0x10 && sub != 0x20) {
        *(u8*)(c+0x3f0) = 0;
        mHorzSpeed = 0x7800;
        *(s32*)(c+0x3e8) = ((u32)RandomIntInternal(&data_0209e650) >> 16) % 100;
        if ((u32)(u8)(*(u32*)(c+8) & 0xf) > 1)
            *(s32*)(c+0x3e4) = 0;
        else
            *(s32*)(c+0x3e4) = 1;
    } else {
        *(s32*)(c+0x3e4) = 4;
    }

    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;

    int r = RandomIntInternal(&data_0209e650);
    int fc = ((Animation *)((void*)(c+0x124)))->GetFrameCount();
    u32 rem = (u32)r % (u32)fc;
    *(s32*)(c+0x12c) = (rem << 16) >> 4;

    *(s32*)(c+0x3e0) = 0x1000;
    s32 spd = *(s32*)(c+0x3e0);
    *(s32*)(c+0x80) = spd;
    *(s32*)(c+0x84) = spd;
    *(s32*)(c+0x88) = spd;
    *(u8*)(c+0x3f1) = 1;
    return 1;
}

