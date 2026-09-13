#ifndef DAPKN_C_H
#define DAPKN_C_H

#include "types.h"

/* Derives from dEnemyBase_c, on the evidence of its own destructor: `_ZN7daPkn_cD1Ev`
 * stores this vtable, destroys six members, then calls `dEnemyBase_c::~dEnemyBase_c`.
 * Everything this header used to restate below 0x110 belongs to that chain and
 * is inherited now.
 *
 * The members close exactly on one another, and dEnemyBase_c's own 0x110 closes
 * exactly on the first:
 *
 *     0x110 ModelAnim                  0x64   -> 0x174
 *     0x174 Model                      0x50   -> 0x1c4
 *     0x1c4 dBgCh_Actr               0x1bc  -> 0x380
 *     0x380 dCcAc_c         0x34   -> 0x3b4
 *     0x3b4 dCcAc_c         0x34   -> 0x3e8
 *     0x3e8 dCcAcPos_c  0x40   -> 0x428
 *
 * Typing them absorbed these markers, which were a member's insides:
 *   - 0x160 mAnimation   = the Animation base of mModelAnim
 *   - 0x170 unk_170      = mModelAnim.file (+0x60)
 *
 * SIZE IS THE ROM'S OWN, and the observed field span agrees with it:
 * daPkn_c_classInit loads the literal 0x47c from ov084 0x0213016c and hands it
 * to fBase_c::operator new, and the last touched word (0x478) closes on 0x47c.
 *
 * THE NAME IS THE CARTRIDGE'S OWN. ov084 0x02130bec holds the bytes
 * "7daPkn_c\0" -- the length-prefixed mangled type name -- _ZTI7daPkn_c at
 * 0x02130bf8 points its +4 word back at that string, and the vtable's -4 header
 * word at 0x02130c24 points back at the _ZTI. The class was carried here under
 * the coined name PiranhaPlant, which occurs in none of the 106 extracted
 * images; that spelling is gone and every member now mangles as _ZN7daPkn_c*.
 * The reconstructed factory daPkn_c_classInit (historical alias
 * PiranhaPlant_Spawn) constructs it for the PAKUN registry profile at
 * 0x02130c04, whose id is 0xfa.
 */

#include "dEnemyBase_c.h"
#include "ModelAnim.h"
#include "Model.h"
#include "dCcAc_c.h"
#include "dCcAcPos_c.h"
#include "dBgCh_Actr.h"

/* Leaf until #2570: fBase_c still has no in-class operator new, so this
   class forwards the `new daPkn_c()` allocation to the retail allocator. */
extern "C" void *_ZN7fBase_cnwEj(unsigned size);

struct daPkn_c : dEnemyBase_c {
    ModelAnim                    mModelAnim;            /* 0x110 */
    Model                        mModel;                /* 0x174 */
    dBgCh_Actr                 mWithMeshClsn;         /* 0x1c4 */
    dCcAc_c           mdCcAc_c1;  /* 0x380 */
    dCcAc_c           mdCcAc_c2;  /* 0x3b4 */
    dCcAcPos_c    mdCcAcPos_c; /* 0x3e8 */
    Vector3                      mBubbleScale;          /* 0x428 -- sleep-bubble Model */
    Vector3                      mBubblePos;            /* 0x434 -- sleep-bubble world pos */
    Vector3                      mHeadClsnOffset;       /* 0x440 -- head cylinder; rewritten every frame */
    Vector3                      mHomePos;              /* 0x44c -- spawn point; InitResources copies 0x440 once */
    s32                          mState;                /* 0x458 */
    u8                           mClsnEnabled;          /* 0x45c */
    u8                           unk_45d;               /* 0x45d */
    u8  pad_45e[0x2];
    s32                          unk_460;               /* 0x460 */
    s32                          unk_464;               /* 0x464 */
    s16                          mTargetAngleY;         /* 0x468 -- yaw toward player; rewritten every frame */
    u8  pad_46a[0x2];
    s32                          unk_46c;               /* 0x46c */
    s32                          mParticleHandle;       /* 0x470 */
    s32                          unk_474;               /* 0x474 */
    s32                          unk_478;               /* 0x478 */

    /* --- vtable --- */

    /* INLINE, AND DECLARED FIRST. The cartridge puts D1 at 0x0212eaf0 below D0
       at 0x0212eb48 and carries no D2 anywhere, which is what mwccarm 2004/b56
       emits for an inline in-class destructor; the out-of-line form emits
       D2/D0/D1 in the wrong order plus a homeless D2. The typed member list
       above makes the empty body own the dCcAcPos_c, the two dCcAc_c, the
       dBgCh_Actr, the Model and the ModelAnim teardowns in that
       reverse-declaration order, and the chain into _ZN12dEnemyBase_cD2Ev --
       which is exactly what both ROM bodies do.

       With the destructor inline, OnAimedAtWithEgg becomes the first
       out-of-line virtual this class declares -- the key function -- so the
       compiler emits the vtable and the RTTI group into the translation unit
       that defines it, src/actors/daPkn_c.cpp. They do not ship from here:
       ov084 delinks no .data, so text-only isolation discards all three and
       the ROM copies stand -- see compiler_only_output in
       config/tu_manifest.d/ov084/daPkn_c.json. */
    virtual ~daPkn_c() {}

    virtual s32   OnAimedAtWithEgg();      /* slot 29 -- key function */

    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();
    int Render();

    static void *operator new(unsigned long size) {
        return _ZN7fBase_cnwEj((unsigned)size);
    }
};

typedef char daPkn_c_size_must_be_0x47c[sizeof(daPkn_c) == 0x47c ? 1 : -1];

/* The class's own vtable, declared next to the class rather than restated in
   the one translation unit that stores it. config/arm9/overlays/ov084/symbols.txt
   binds _ZTV7daPkn_c to the public ADDRESS POINT at 0x02130c28; mwcc's own
   emitted symbol addresses the vtable OBJECT two words lower, so `new daPkn_c()`
   and the inline destructor store `_ZTV7daPkn_c + 2` (int-indexed, eight bytes).
   include/decl_common.h already carries the same declaration for the C shards
   that read it. */
extern int _ZTV7daPkn_c[];

#endif /* DAPKN_C_H */
