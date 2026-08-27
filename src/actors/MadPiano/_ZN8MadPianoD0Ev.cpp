//cpp
// @symbol _ZN8MadPianoD0Ev
/* D0, the deleting destructor. Same shadow struct as the D1 file beside this one;
 * mwcc emits D0, D1 and D2 together from one destructor definition and objisolate
 * keeps the variant this file's delinks entry names. The trailing operator delete
 * comes from the base -- see the inline one in include/dActor_c.h. */
#include "dBgActor_c.h"
#include "ModelAnim.h"
#include "ShadowModel.h"
#include "dCcAcPos_c.h"
#include "dBgCh_Actr.h"

struct MadPiano : dBgActor_c {
    ModelAnim mModelAnim;               /* 0x320 */
    ShadowModel mShadowModel1;          /* 0x384 */
    ShadowModel mShadowModel2;          /* 0x3ac */
    ShadowModel mShadowModel3;          /* 0x3d4 */
    u8 pad_3fc[0x90];
    dCcAcPos_c mAcPos[2];               /* 0x48c */
    dBgCh_Actr mWithMeshClsn;           /* 0x50c */
    u8 pad_6c8[0x1c];
    virtual ~MadPiano();
};

typedef char MadPiano_size_must_be_0x6e4[sizeof(MadPiano) == 0x6e4 ? 1 : -1];

MadPiano::~MadPiano()
{
}
