#ifndef DAOBJMARIOCAP_C_H
#define DAOBJMARIOCAP_C_H

/* RECONSTRUCTED NAMES USED IN THIS HEADER. SM64DS RTTI names the
 * implementation(s) below; the registry profile object and the factory
 * spelling are Tier B reconstructions -- evidence-bounded proposals, not
 * recovered SM64DS symbols. Exact original spellings are not preserved.
 *
 *   daObjMarioCap_c -- daObjMarioCap_c_classInit (was Cap_Spawn), g_profile_OBJ_MARIO_CAP (was Cap_SpawnInfo)
 */

#include "types.h"
#include "dEnemyBase_c.h"
#include "CapIcon.h"
#include "ModelAnim.h"
#include "dCcAc_c.h"
#include "ShadowModel.h"
#include "dBgCh_Actr.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

/* Derives from dEnemyBase_c, and both witnesses agree offset for offset:
 *
 *   daObjMarioCap_c_classInit (ov002) allocates 0x410, calls _ZN12dEnemyBase_cC2Ev, stores
 *   _ZTV15daObjMarioCap_c, then constructs dCcAc_c 0x110, dBgCh_Actr 0x144,
 *   ModelAnim 0x300, ShadowModel 0x364 and the CapIcon at 0x3d0.
 *
 *   _ZN15daObjMarioCap_cD1Ev tears the same five down in exactly the reverse order and
 *   chains to _ZN12dEnemyBase_cD2Ev.
 *
 * THE 0x3d0 MEMBER IS dCapIcon_c (the CapIcon compatibility spelling), whose
 * ROM RTTI and two-slot vtable identify its constructor/destructor at
 * 0x020ab3c4 / 0x020ab3a0. It is the same member dCapEnemy_c holds at 0x164.
 * Left as padding the destructor emits a short chain and comes out a different
 * SIZE, which reads as `999 word(s) differ` and looks like a total failure
 * rather than one missing member.
 *
 * SIZE 0x410, the literal in daObjMarioCap_c_classInit's fBase_c::operator new. CapIcon is 0x1c, so
 * 0x3d0 + 0x1c = 0x3ec closes onto the scalars below it.
 *
 * This class used to be named WaterfallMist; RTTI ov002:0x021095ac names
 * 15daObjMarioCap_c at vtable 0x021095f0, and the waterfall name belongs to
 * daObjWaterfall_c.
 */
struct daObjMarioCap_c : dEnemyBase_c {
    dCcAc_c  mdCcAc_c;    /* 0x110 */
    dBgCh_Actr        mWithMeshClsn;          /* 0x144 */
    ModelAnim           mModelAnim;             /* 0x300 */
    ShadowModel         mShadowModel;           /* 0x364 */
    Matrix4x3           unk_38c;                /* 0x38c -- drop-shadow matrix */
    s32 unk_3bc;                                /* 0x3bc -- PMF holder* */
    s32 unk_3c0;                                /* 0x3c0 -- Player* */
    s32 unk_3c4;                                /* 0x3c4 */
    s32 unk_3c8;                                /* 0x3c8 */
    s32 unk_3cc;                                /* 0x3cc */
    dCapIcon_c mCapIcon;                        /* 0x3d0 */
    s32 unk_3ec;                                /* 0x3ec */
    s32 mType;                                  /* 0x3f0 */
    s32 mModelIndex;                            /* 0x3f4 */
    u8  pad_3f8[0x4];
    s16 unk_3fc;                                /* 0x3fc */
    u8  unk_3fe;                                /* 0x3fe */
    u8  unk_3ff;                                /* 0x3ff */
    u8  unk_400;                                /* 0x400 */
    u8  unk_401;                                /* 0x401 */
    u8  unk_402;                                /* 0x402 */
    u8  unk_403;                                /* 0x403 */
    u16 unk_404;                                /* 0x404 */
    u8  pad_406[0x2];
    s32 unk_408;                                /* 0x408 */
    s32 unk_40c;                                /* 0x40c */

    /* INLINE, AND DECLARED FIRST. The cartridge puts D1 at 0x020b6f18 below
       D0 at 0x020b6f68 and carries no D2, which is exactly what mwccarm 2004
       emits for an inline destructor; an out-of-line one emits D2/D0/D1 in the
       wrong order plus a homeless D2. The typed member list below makes the
       empty body own the dCapIcon_c, ShadowModel, ModelAnim, dBgCh_Actr and
       dCcAc_c teardowns and the chain into _ZN12dEnemyBase_cD2Ev.

       With the destructor inline, OnYoshiTryEat becomes the first out-of-line
       virtual this class declares -- the key function -- so the vtable and the
       RTTI group land in the translation unit that defines it,
       src/actors/daObjMarioCap_c.cpp. */
    virtual ~daObjMarioCap_c() {}

    virtual s32   OnYoshiTryEat();         /* slot 18 -- key function */

    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
    void OnPendingDestroy();
    void OnTurnIntoEgg(Player &player);  /* slot 19, ov002 0x020b81e0 */

    /* Leaf until fBase_c can declare operator new (#2570). unsigned long, not
       unsigned int: size_t is unsigned int on this include path and mangles
       nwEj, colliding with fBase_c's own allocator. */
    static void *operator new(unsigned long size) {
        return _ZN7fBase_cnwEj((unsigned)size);
    }
};

typedef char daObjMarioCap_c_size_must_be_0x410[sizeof(daObjMarioCap_c) == 0x410 ? 1 : -1];

#endif /* DAOBJMARIOCAP_C_H */
