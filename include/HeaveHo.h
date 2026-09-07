#ifndef HEAVEHO_H
#define HEAVEHO_H

#include "types.h"

/* Derives from dEnemyBase_c, on the evidence of its own destructor: `_ZN7HeaveHoD1Ev`
 * stores this vtable, destroys its members in reverse declaration order, then
 * calls `dEnemyBase_c::~dEnemyBase_c`. Everything this header used to restate below 0x110
 * belongs to that chain and is inherited now.
 *
 * The members close exactly on one another:
 *
 *     0x110 dCcAc_c         0x34   -> 0x144
 *     0x144 dCcAcPos_c  0x40   -> 0x184
 *     0x184 dBgCh_Actr               0x1bc  -> 0x340
 *     0x340 ModelAnim                  0x64   -> 0x3a4
 *     0x3a4 ShadowModel                0x28   -> 0x3cc
 *
 * Typing them absorbed these markers, which were a member's insides:
 *   - 0x390 mAnimation   = mModelAnim + 0x50
 *   - 0x39c unk_39c      = mModelAnim + 0x5c
 *
 * Member NAMES are the ones this header already used -- a rebase should not
 * also rename things its callers spell.
 *
 * SIZE IS THE ROM'S OWN, not a rounded-up field span: `daPopoi_c_classInit` calls
 * `fBase_c::operator new(1068)` -- 0x42c -- and stores `_ZTV7HeaveHo`,
 * so that literal IS this class's sizeof. The observed fields only span to
 * 0x428; the difference is trailing space no source reads.
 *
 * SM64DS RTTI names the implementation daPopoi_c. The reconstructed
 * factory daPopoi_c_classInit (historical alias
 * HeaveHo_Spawn) constructs it for the POPOI
 * registry profile.
 */

#include "dEnemyBase_c.h"
#include "Model.h"
#include "ModelAnim.h"
#include "dCcAc_c.h"
#include "dCcAcPos_c.h"
#include "ShadowModel.h"
#include "TextureTransformer.h"
#include "dBgCh_Actr.h"

struct HeaveHo : dEnemyBase_c {
    dCcAc_c           mdCcAc_c;   /* 0x110 */
    dCcAcPos_c    mdCcAcPos_c; /* 0x144 */
    dBgCh_Actr                 mWithMeshClsn;         /* 0x184 */
    ModelAnim                    mModelAnim;            /* 0x340 */
    ShadowModel                  mShadowModel;          /* 0x3a4 */
    u8  pad_3cc[0x30];
    s32                          unk_3fc;               /* 0x3fc */
    s32                          unk_400;               /* 0x400 */
    s32                          unk_404;               /* 0x404 */
    s32                          unk_408;               /* 0x408 */
    s32                          unk_40c;               /* 0x40c */
    s32                          unk_410;               /* 0x410 */
    s32                          unk_414;               /* 0x414 */
    s32                          unk_418;               /* 0x418 */
    u8  pad_41c[0xa];
    u8                           unk_426;               /* 0x426 */
    u8  pad_427[0x5];

    /* --- vtable --- */
    virtual ~HeaveHo();

    int Behavior();
    int InitResources();
    int Render();
    int CleanupResources();
    void OnPendingDestroy();
};

typedef char HeaveHo_size_must_be_0x42c[sizeof(HeaveHo) == 0x42c ? 1 : -1];

#endif /* HEAVEHO_H */
