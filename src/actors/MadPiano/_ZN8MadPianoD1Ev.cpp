//cpp
// @symbol _ZN8MadPianoD1Ev
/* D1, the complete-object destructor. The shadow struct below is the point: the
 * shared header spells this class FLAT, with the base's fields restated as padding,
 * so nothing in it tells mwcc there are subobjects to tear down. Naming the real
 * base and the real member types makes the compiler generate the ROM's teardown --
 * every member destroyed in reverse declaration order, then the base chain -- from
 * an empty body. The shared header is left alone; it has matched functions compiled
 * against its flat form. */
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
