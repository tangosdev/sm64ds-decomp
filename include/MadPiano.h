/* MadPiano's constructor and destructor sequences independently establish a
 * dBgActor_c base followed by the typed members named below. */
#ifndef MADPIANO_H
#define MADPIANO_H
#include "types.h"
#include "dBgActor_c.h"
#include "ModelAnim.h"
#include "ShadowModel.h"
#include "dCcAcPos_c.h"
#include "dBgCh_Actr.h"

/* dBgActor_c owns the Model at 0xd4, dBgW_KcMbg at 0x124, and collision
 * matrix through 0x31d. The class's own first member therefore starts at
 * 0x320, exactly where the factory constructs it. */
struct MadPiano : dBgActor_c {
    /* ModelAnim member, named by _ZN9ModelAnimD1Ev at +0x320 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    ModelAnim mModelAnim;            /* 0x320 */
    /* ShadowModel member. The cartridge's own ~MadPiano calls _ZN11ShadowModelD1Ev at
       +0x384 (D0/D1), a relocation the ROM build checks; recovered by
       tools/dtor_members.py. D1 and not D2, so it is this type and not an inlined base. */
    ShadowModel mShadowModel1;            /* 0x384 */
    /* ShadowModel member. The cartridge's own ~MadPiano calls _ZN11ShadowModelD1Ev at
       +0x3ac (D0/D1), a relocation the ROM build checks; recovered by
       tools/dtor_members.py. D1 and not D2, so it is this type and not an inlined base. */
    ShadowModel mShadowModel2;            /* 0x3ac */
    /* ShadowModel member. The cartridge's own ~MadPiano calls _ZN11ShadowModelD1Ev at
       +0x3d4 (D0/D1), a relocation the ROM build checks; recovered by
       tools/dtor_members.py. D1 and not D2, so it is this type and not an inlined base. */
    ShadowModel mShadowModel3;            /* 0x3d4 */
    u8  pad_3fc[0x90];
    /* The factory's array-construction helper and both destructor variants
       agree on two 0x40-byte dCcAcPos_c objects starting at 0x48c. */
    dCcAcPos_c mCylinderClsn[2];            /* 0x48c */
    /* dBgCh_Actr member. The cartridge's own ~MadPiano calls _ZN10dBgCh_ActrD1Ev at
       +0x50c (D0/D1), a relocation the ROM build checks; recovered by
       tools/dtor_members.py. D1 and not D2, so it is this type and not an inlined base. */
    dBgCh_Actr mWithMeshClsn;            /* 0x50c */
    u8  pad_6c8[0x4];
    s32 mMinPosY;            /* 0x6cc */
    u8  pad_6d0[0x4];
    Vector3 mHomePos;            /* 0x6d4 */
    /* trailing extent the ROM's `new MadPiano` literal proves; see tools/opnew_sizes.py */
    u8 pad_6e0[0x4];
    virtual ~MadPiano();

    virtual int InitResources();
    virtual int CleanupResources();
    virtual int Behavior();
    virtual int Render();
};

typedef char MadPiano_size_must_be_0x6e4[sizeof(struct MadPiano) == 0x6e4 ? 1 : -1];

#endif
