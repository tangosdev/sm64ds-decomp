#ifndef DAOBJFL_BLOCK_C_H
#define DAOBJFL_BLOCK_C_H

#include "types.h"
#include "dBgW_KcMbg.h"

/* Lethal Lava Land's block that floats on the lava (registry profile
 * FL_BLOCK): a dBgActor_c that sinks while something stands on it and rises
 * back to its spawn height once it is left alone. Model at 0xd4, moving mesh
 * collider at 0x124, clsn matrix at 0x2ec, all inherited. Layout evidence:
 * notes/platform-provenance.md.
 *
 * NAME: daObjFl_Block_c is the cartridge's own RTTI spelling. _ZTS at ov022
 * 0x02113e74 is the byte string "15daObjFl_Block_c", and _ZTI at 0x02113e68
 * points at that string and at _ZTI10dBgActor_c, so the ROM states the direct
 * base too. The vtable symbol ov022/symbols.txt spells _ZTV15daObjFl_Block_c
 * (0x02113eac) carries a relocation at its address point minus 4 -- ov022
 * relocs.txt `from:0x02113ea8 to:0x02113e68` -- which is that same _ZTI, so
 * the three records are one triple and this is the class the ROM names.
 */

#ifdef __cplusplus

#include "dBgActor_c.h"

struct daObjFl_Block_c : dBgActor_c {
    u8  pad_31e[0x2];
    s32 mMaxPosY;                      /* 0x320 */
    u8 mHadClsn;                       /* 0x324 */

    /* --- vtable --- */
    virtual ~daObjFl_Block_c();

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjFl_Block_c_size_must_be_0x328[sizeof(daObjFl_Block_c) == 0x328 ? 1 : -1];
#endif

#else

/* The same object spelled flat, for the compiler-generated destructor, which
   lives in a C translation unit and can never be migrated. */
struct daObjFl_Block_c {
    u8  pad_000[0x60];
    s32 mPosY;            /* 0x060 */
    u8  pad_064[0x2a];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x44];
    Model mModel;            /* 0x0d4 */
    dBgW_KcMbg mMeshCollider;            /* 0x124 */
    u8  pad_2ec[0x34];
    s32 mMaxPosY;            /* 0x320 */
    u8  mHadClsn;            /* 0x324 */
};

#endif /* __cplusplus */

#endif /* DAOBJFL_BLOCK_C_H */
