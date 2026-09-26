/* dScMg3DEsp_c, scene 0x185 (profile MG_3DESP). The ROM's RTTI names the
 * class and its one base, dScMgSingle3DBase_c. The factory
 * dScMg3DEsp_c_classInit, historically MgPsycheOut_Spawn, allocates 0x5558
 * bytes.
 *
 * The factory and the destructor fix the members after the base:
 *   - two Models at 0x4f38 and 0x4f88, 0x50 bytes each
 *   - a dMg3DEspModel_c at 0x4fd8, 0x21c bytes; unk_51e4 is its unk_20c
 *   - a TextureTransformer at 0x51f4, 0x14 bytes
 *   - this class's own fields from 0x5208 to 0x5558
 * The factory constructs the four in declaration order and the destructor
 * tears them down in reverse, so they could become typed members. They stay
 * raw storage until the factory and the destructor change together.
 *
 * The destructor is the key function, defined out of line; the D1 and D0
 * files carry the same definition. dScMgBase_c's operator delete serves D0.
 */
#ifndef DSCMG3DESP_C_H
#define DSCMG3DESP_C_H
#include "dScMgSingle3DBase_c.h"

struct dScMg3DEsp_c : dScMgSingle3DBase_c {
    virtual ~dScMg3DEsp_c();
    virtual void OnYoshiTryEat(int arg);               /* slot 18 */
    virtual void Virtual50();                          /* slot 20 */

    u8  mModel1[0x50];             /* 0x4f38, a Model */
    u8  mModel2[0x50];             /* 0x4f88, a Model */
    u8  pad_4fd8[0x20c];           /* 0x4fd8, a dMg3DEspModel_c up to 0x51f4 */
    s32 unk_51e4;                  /* 0x51e4 */
    u8  pad_51e8[0xc];             /* 0x51e8 */
    u8  mTextureTransformer[0x14]; /* 0x51f4, a TextureTransformer */
    u8  pad_5208[0x334];           /* 0x5208 */
    s32 unk_553c;                  /* 0x553c, the state index Behavior dispatches on */
    u8  pad_5540[0x18];            /* 0x5540 */

    /* Overrides of fBase_c's slots 0, 3, 6 and 9, defined out of line. The
       destructor stays the key function, so only its files emit the vtable. */
    s32 InitResources();      /* slot 0 */
    s32 CleanupResources();   /* slot 3 */
    s32 Behavior();           /* slot 6 */
    s32 Render();             /* slot 9 */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dScMg3DEsp_c_size_must_be_0x5558[sizeof(dScMg3DEsp_c) == 0x5558 ? 1 : -1];
#endif

#endif
