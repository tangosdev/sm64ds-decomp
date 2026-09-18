/* daObjKi_Fune_c's matched layout and real dBgActor_c inheritance. */
#ifndef DAOBJKI_FUNE_C_H
#define DAOBJKI_FUNE_C_H
#include "dBgActor_c.h"

struct daObjKi_Fune_c : dBgActor_c {
    /* dBgActor_c's data ends at 0x31e and its sizeof rounds to 0x320. The ABI
       reuses that tail padding for the first derived byte, as the matched
       daObjKi_Fune_c methods prove. */
    u8  mModelIndex;            /* 0x31e */
    u8  pad_31f[0x1];
    u16 mBobAngle;            /* 0x320 */
    u8  pad_322[0x2];
    s32 mSoundHandle;            /* 0x324 */
    u16 unk_328;            /* 0x328 */
    /* Inline is load-bearing. Written out of line mwccarm 2004/b56 emits the
       synthesized D0 ahead of the written D1 and adds a homeless D2, the
       reverse of the cartridge's D1 0x0211260c / D0 0x02112650 pair, which
       has no D2 at all. Inline emits D1 then D0 and no D2, and moves the key
       function to InitResources, the first non-inline virtual, which is what
       makes the promoted TU emit this class's vtable and RTTI records under
       the ROM's own names. Body is empty: destroying the dBgW_KcMbg and Model
       subobjects and running dBgActor_c's own destructor all follow from the
       declarations above. Keep the brace on the signature line --
       tools/check_header_offsets.py only recognises a body written that way.

       Every member of this class is defined in one translation unit,
       src/game/actors/d_a_obj_ki_fune.cpp, which owns the whole
       0x0211260c..0x021129a0 linker run. */
    virtual ~daObjKi_Fune_c() {}
    virtual int InitResources();
    virtual int CleanupResources();
    virtual int Behavior();
    virtual int Render();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjKi_Fune_c_size_must_be_0x32c[sizeof(struct daObjKi_Fune_c) == 0x32c ? 1 : -1];
#endif

#endif
