#ifndef ARROWSIGNRIGHT_H
#define ARROWSIGNRIGHT_H

#include "types.h"
#include "dBgW_KcMbg.h"

/* Derives from dBgActor_c: the destructor stores this class's vtable, then
 * dBgActor_c's -- inlined -- then destroys the dBgW_KcMbg at 0x124 and
 * the Model at 0xd4 before chaining to dActor_c. All three belong to dBgActor_c.
 * Everything this header used to restate below 0x31e was dActor_c's and
 * dBgActor_c's, and is inherited now.
 *
 * SIZE IS THE OBSERVED FIELD SPAN, rounded up. It guards this declaration; it
 * is not independent evidence about the ROM.
 */

#ifdef __cplusplus

#include "dBgActor_c.h"
#include "ShadowModel.h"

struct ArrowSignRight : dBgActor_c {
    u8  pad_31e[0x2];
    ShadowModel mShadowModel;         /* 0x320 */
    /* Behavior passes `&mShadowMat' as the `Matrix4x3 &' argument of
       dActor_c::DropShadowScaleXYZ, with mShadowModel as the argument before
       it. A Matrix4x3 is 0x30 bytes and 0x348 + 0x30 = 0x378. Left a u8
       marker, the idiom this family's C twins already use. */
    u8  mShadowMat;                   /* 0x348 */
    u8  pad_349[0x33];
    u8  mVariant;                     /* 0x37c -- 0/1 from actorID; indexes all three ov098 resource columns */

    /* --- vtable --- */
    virtual ~ArrowSignRight();

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
    virtual int  OnAttacked1(dActor_c &other);      /* slot 22 */
    virtual void OnHitByMegaChar(Player &player);   /* slot 27 */
    /* THE VTABLE SAYS SO. _ZTV14ArrowSignRight is ov098 0x0213c3d8 and the word
       at +0x7c relocates to ov098 0x02137ccc, while _ZTV10dBgActor_c carries
       _ZN10dBgActor_c4KillEv at the same slot -- so this is this class's own
       override of dBgActor_c's Kill, not a new virtual. Slot 30 (+0x78) is still
       the main-module 0x02010124 both tables share, which is what makes 31 the
       first slot where they differ. An override adds no field and no slot; the
       0x380 assertion below is unchanged. */
    virtual void Kill();                            /* slot 31 */
};

typedef char ArrowSignRight_size_must_be_0x380[sizeof(ArrowSignRight) == 0x380 ? 1 : -1];

#else

/* The C spelling of the same object, flat. Kept because the D0 file is a C
   translation unit that reads these fields, and D0 is compiler-generated so it
   can never be migrated. Same arrangement as include/ShadowModel.h. */
struct ArrowSignRight {
    u8  pad_000[0xc];
    u16 actorID;            /* 0x00c */
    u8  pad_00e[0x80];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x44];
    /* Model member, named by _ZN5ModelD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel;            /* 0x0d4 */
    /* dBgW_KcMbg member. The cartridge's own ~ArrowSignRight calls _ZN10dBgW_KcMbgD1Ev
       at +0x124 (D0/D1), a relocation the ROM build checks; recovered by
       tools/dtor_members.py. D1 and not D2, so it is this type and not an inlined base. */
    dBgW_KcMbg mMeshCollider;            /* 0x124 */
    u8  pad_2ec[0x34];
    /* ShadowModel member, named by the class's own destructor calling
       ShadowModel's D1 at +0x320 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN14ArrowSignRightD1Ev.c] */
    ShadowModel mShadowModel;            /* 0x320 */
    u8  mShadowMat;         /* 0x348 */
    u8  pad_349[0x33];
    u8  mVariant;           /* 0x37c */
};

#endif /* __cplusplus */

#endif /* ARROWSIGNRIGHT_H */
