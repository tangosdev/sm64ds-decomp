#ifndef DAOBJYAJIRUSI_C_H
#define DAOBJYAJIRUSI_C_H

#include "types.h"
#include "dBgW_KcMbg.h"

struct Player;

/* Derives from dBgActor_c: the destructor stores this class's vtable, then
 * dBgActor_c's -- inlined -- then destroys the dBgW_KcMbg at 0x124 and
 * the Model at 0xd4 before chaining to dActor_c. All three belong to dBgActor_c.
 * Everything this header used to restate below 0x31e was dActor_c's and
 * dBgActor_c's, and is inherited now.
 *
 * SIZE IS THE OBSERVED FIELD SPAN, rounded up. It guards this declaration; it
 * is not independent evidence about the ROM.
 *
 * SM64DS RTTI names the implementation daObjYajirusi_c. The reconstructed
 * factory daObjYajirusi_c_classInit_YAJIRUSI_R (historical alias
 * ArrowSignRight_Spawn) constructs it for the YAJIRUSI_R
 * registry profile.
 */

#ifdef __cplusplus

#include "common.h"
#include "Model.h"
#include "ShadowModel.h"

#if defined(SM64DS_PLATFORM_PC) && defined(_MSC_VER)
#include <stddef.h>
/* mwccarm reuses dBgActor_c's two bytes of tail padding, placing this class's
   first derived storage at 0x31e. MSVC starts after sizeof(dBgActor_c)==0x320,
   which shifts the derived fields and grows the object to 0x384 even though
   daObjYajirusi_c_classInit_YAJIRUSI_R allocates exactly 0x380. The port supplies its vtable
   manually, so use a flat host view with every exercised field pinned to the
   ROM offset. The matching build below retains the real inheritance. */
struct daObjYajirusi_c {
    void *vtable;                      /* 0x000 */
    u8  pad_004[0x8];
    u16 actorID;                       /* 0x00c */
    u8  pad_00e[0x4e];
     /* dActor_c's position and camera-space position, at dActor_c's own offsets
        (include/dActor_c.h lines 59-65). The flat view drops the inheritance but
        src/actors/daObjYajirusi_c.cpp (Kill) still reads these four fields by
        name, so they are pinned here rather than buried in padding. */
    s32 mPosX;                         /* 0x05c */
    s32 mPosY;                         /* 0x060 */
    s32 mPosZ;                         /* 0x064 */
    u8  pad_068[0xc];
    s32 mCamSpacePosX;                 /* 0x074 */
    u8  pad_078[0x16];
    s16 mAngleY;                       /* 0x08e */
    u8  pad_090[0x44];
    Model mModel;                      /* 0x0d4 */
    dBgW_KcMbg mMeshCollider;          /* 0x124 */
    Matrix4x3 mClsnMat;                /* 0x2ec */
    u8  pad_31c[0x4];
    ShadowModel mShadowModel;          /* 0x320 */
    u8 mShadowMat;                        /* 0x348 */
    u8  pad_349[0x33];
    u8 mVariant;                        /* 0x37c */
    u8  pad_37d[0x3];

    ~daObjYajirusi_c();
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
    int OnAttacked1(dActor_c &other);
    void OnHitByMegaChar(Player &player);
    void Kill();

    /* INHERITED, AND THEREFORE THE PORT'S TO BIND. Kill() calls both of these
       unqualified; on the ARM they come from dActor_c and fBase_c, which the
       flat view cannot derive from without moving every field. Declared here so
       the translation unit still compiles for the host, non-virtual so no slot
       and no field moves. They resolve to _ZN8dActor_c19DisappearPoofDustAtERK7Vector3
       and _ZN7fBase_c19MarkForDestructionEv, which is an /alternatename the port
       can write; without these two declarations the file cannot be compiled at
       all. */
    void DisappearPoofDustAt(const Vector3 &pos);
    void MarkForDestruction();
};

static_assert(offsetof(daObjYajirusi_c, actorID) == 0x00c, "daObjYajirusi_c actorID");
static_assert(offsetof(daObjYajirusi_c, mPosX) == 0x05c, "daObjYajirusi_c mPosX");
static_assert(offsetof(daObjYajirusi_c, mCamSpacePosX) == 0x074, "daObjYajirusi_c mCamSpacePosX");
static_assert(offsetof(daObjYajirusi_c, mAngleY) == 0x08e, "daObjYajirusi_c mAngleY");
static_assert(offsetof(daObjYajirusi_c, mModel) == 0x0d4, "daObjYajirusi_c mModel");
static_assert(offsetof(daObjYajirusi_c, mMeshCollider) == 0x124, "daObjYajirusi_c collider");
static_assert(offsetof(daObjYajirusi_c, mClsnMat) == 0x2ec, "daObjYajirusi_c matrix");
static_assert(offsetof(daObjYajirusi_c, mShadowModel) == 0x320, "daObjYajirusi_c shadow");
static_assert(offsetof(daObjYajirusi_c, mShadowMat) == 0x348, "daObjYajirusi_c +0x348");
static_assert(offsetof(daObjYajirusi_c, mVariant) == 0x37c, "daObjYajirusi_c +0x37c");
static_assert(sizeof(daObjYajirusi_c) == 0x380, "daObjYajirusi_c host size");

#else

#include "dBgActor_c.h"

struct daObjYajirusi_c : dBgActor_c {
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
    /* Inline and first: measured (class-form skill) that mwccarm 2004/b56
       emits the retail D1-then-D0 pair in ROM order plus _ZTV/_ZTI/_ZTS
       homed in the instantiating TU, and no leaf D2. Out-of-line emits
       D0 before D1, which the production isolate refuses. */
    virtual ~daObjYajirusi_c() {}

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
    /* Reference spellings below are coined guesses: a reference and a pointer
       mangle differently but generate identical ARM for these bodies, so the
       bytes cannot distinguish them. Class ownership, slot identity, bodies
       and relocations are proven; the exact original spelling is not. */
    virtual int  OnAttacked1(dActor_c &other);      /* slot 22 */
    virtual void OnHitByMegaChar(Player &player);   /* slot 27 */
    /* THE VTABLE SAYS SO. _ZTV15daObjYajirusi_c is ov098 0x0213c3d8 and the word
       at +0x7c relocates to ov098 0x02137ccc, while _ZTV10dBgActor_c carries
       _ZN10dBgActor_c4KillEv at the same slot -- so this is this class's own
       override of dBgActor_c's Kill, not a new virtual. Slot 30 (+0x78) is still
       the main-module 0x02010124 both tables share, which is what makes 31 the
       first slot where they differ. An override adds no field and no slot; the
       0x380 assertion below is unchanged. */
    virtual void Kill();                            /* slot 31 */
};

typedef char daObjYajirusi_c_size_must_be_0x380[sizeof(daObjYajirusi_c) == 0x380 ? 1 : -1];

#endif /* SM64DS_PLATFORM_PC && _MSC_VER */

#else

/* The C spelling of the same object, flat. Kept because the port builds
   this header for the host, and the flat view cannot derive from dBgActor_c
   without moving every field. Same arrangement as include/ShadowModel.h. */
struct daObjYajirusi_c {
    u8  pad_000[0xc];
    u16 actorID;            /* 0x00c */
    u8  pad_00e[0x80];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x44];
    /* Model member, named by _ZN5ModelD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel;            /* 0x0d4 */
    /* dBgW_KcMbg member. The cartridge's own ~daObjYajirusi_c calls _ZN10dBgW_KcMbgD1Ev
       at +0x124 (D0/D1), a relocation the ROM build checks; recovered by
       tools/dtor_members.py. D1 and not D2, so it is this type and not an inlined base. */
    dBgW_KcMbg mMeshCollider;            /* 0x124 */
    u8  pad_2ec[0x34];
    /* ShadowModel member, named by the class's own destructor calling
       ShadowModel's D1 at +0x320 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN15daObjYajirusi_cD1Ev.c] */
    ShadowModel mShadowModel;            /* 0x320 */
    u8  mShadowMat;         /* 0x348 */
    u8  pad_349[0x33];
    u8  mVariant;           /* 0x37c */
};

#endif /* __cplusplus */

#endif /* DAOBJYAJIRUSI_C_H */
