#ifndef DAOBJRC_TIKUWA_C_H
#define DAOBJRC_TIKUWA_C_H

#include "types.h"

/* daObjRc_Tikuwa_c -- the RC_TIKUWA donut block that falls.
 *
 * NAME: the cartridge spells this class daObjRc_Tikuwa_c. The typeinfo word
 * of the vtable header, at ov036 0x02113cf4 (address point - 4), is
 * 0x02113cb4. That __si_class_type_info record reads [0x0209a764
 * (_ZTVN3abi20__si_class_type_infoE), 0x02113cc0, 0x021089ec], and the string
 * at 0x02113cc0 is "16daObjRc_Tikuwa_c\0". The tree previously called the
 * class DonutBlock, a coined name present only on the vtable address.
 *
 * BASE: dBgActor_c, direct -- the record's third word is _ZTI10dBgActor_c at
 * ov002 0x021089ec. The destructor stores this class's vtable, then
 * dBgActor_c's -- inlined -- then destroys the dBgW_KcMbg at 0x124 and the
 * Model at 0xd4 before chaining to dActor_c. Those two members belong to
 * dBgActor_c and are inherited.
 *
 * MEMBERS: the dBgCh_Actr at 0x320 is named by the class's own D1/D0 calling
 * _ZN10dBgCh_ActrD1Ev at +0x320, a relocation the ROM build checks. The home
 * position at 0x4dc is copied from mPosX/Y/Z by InitResources and restored in
 * Behavior's landing state.
 *
 * SIZE 0x4ec is the literal daObjRc_Tikuwa_c_classInit (historically
 * DonutBlock_Spawn) passes to fBase_c::operator new.
 *
 * THE VTABLE at ov036 0x02113cf8 has the same 32 slots as dBgActor_c. Only
 * slots 0, 3, 6, 9, 12, 16 and 17 differ, exactly the overrides declared here.
 */

#ifdef __cplusplus

#include "dBgActor_c.h"
#include "dBgCh_Actr.h"

struct daObjRc_Tikuwa_c : dBgActor_c {
    u8 pad_31e[0x2];                 /* 0x31e -- dBgActor_c tail */
    dBgCh_Actr mWithMeshClsn;        /* 0x320 */
    s32 mHomePosX;                   /* 0x4dc */
    s32 mHomePosY;                   /* 0x4e0 */
    s32 mHomePosZ;                   /* 0x4e4 */
    u8 mHadClsn;                     /* 0x4e8 */
    u8 mClsnTimer;                   /* 0x4e9 */
    u8 mState;                       /* 0x4ea */

    /* OUT OF LINE, DECLARED FIRST: the key function. The TU defines it first
       under `#pragma defer_codegen off`, which emits D1 then D0 -- the
       cartridge's order -- then a D2 the cartridge has no home for. */
    virtual ~daObjRc_Tikuwa_c();

    virtual s32 InitResources();       /* slot 0 */
    virtual s32 CleanupResources();    /* slot 3 */
    virtual s32 Behavior();            /* slot 6 */
    virtual s32 Render();              /* slot 9 */
    virtual void OnPendingDestroy();   /* slot 12 -- empty body in the ROM */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjRc_Tikuwa_c_size_must_be_0x4ec[sizeof(daObjRc_Tikuwa_c) == 0x4ec ? 1 : -1];
#endif

#endif /* __cplusplus */

#endif /* DAOBJRC_TIKUWA_C_H */
