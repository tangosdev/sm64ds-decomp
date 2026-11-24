#ifndef DAOBJYAJIRUSI_C_H
#define DAOBJYAJIRUSI_C_H

#include "types.h"
#include "dBgW_KcMbg.h"

struct daPly_c;

/* Derives from dBgActor_c: the destructor stores this class's vtable, then
 * dBgActor_c's -- inlined -- then destroys the dBgW_KcMbg at 0x124 and
 * the dExtSimpleModel_c at 0xd4 before chaining to dActor_c. All three belong to dBgActor_c.
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

#include "common.h"
#include "dExtSimpleModel_c.h"
#include "dExtShadowModel_c.h"
#include "dBgActor_c.h"

struct daObjYajirusi_c : dBgActor_c {
    u8  pad_31e[0x2];
    dExtShadowModel_c mShadowModel;         /* 0x320 */
    /* Behavior passes `mShadowMat' as the `Matrix4x3 &' argument of
       dActor_c::DropShadowScaleXYZ, with mShadowModel as the argument before
       it; UpdateShadowMatrix builds it. A Matrix4x3 is 0x30 bytes, so it
       ends at 0x378. Raw bytes rather than a Matrix4x3, whose Vector3 would
       bring a destructor (see the .cpp). Was a u8 marker and pad_349. */
    Matrix4x3 mShadowMat; /* 0x348 */
    u8 pad_378[0x4];
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
    virtual void OnAttacked1(dActor_c &other);      /* slot 22 */
    virtual void OnHitByMegaChar(daPly_c &player);  /* slot 27 */
    /* THE VTABLE SAYS SO. _ZTV15daObjYajirusi_c is ov098 0x0213c3d8 and the word
       at +0x7c relocates to ov098 0x02137ccc, while _ZTV10dBgActor_c carries
       _ZN10dBgActor_c4KillEv at the same slot -- so this is this class's own
       override of dBgActor_c's Kill, not a new virtual. Slot 30 (+0x78) is still
       the main-module 0x02010124 both tables share, which is what makes 31 the
       first slot where they differ. An override adds no field and no slot; the
       0x380 assertion below is unchanged. */
    virtual void Kill();                            /* slot 31 */

    /* Shadow matrix at +0x348: yaw, then position >> 3 in the translation
       words. Not virtual. */
    void UpdateShadowMatrix();
};

typedef char daObjYajirusi_c_size_must_be_0x380[sizeof(daObjYajirusi_c) == 0x380 ? 1 : -1];

#endif /* DAOBJYAJIRUSI_C_H */
