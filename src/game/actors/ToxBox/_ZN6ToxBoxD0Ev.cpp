//cpp
// @symbol _ZN6ToxBoxD0Ev
/* D0, the deleting destructor. Same shadow struct as the D1 file beside this one;
 * mwcc emits D0, D1 and D2 together from one destructor definition and objisolate
 * keeps the variant this file's delinks entry names. The trailing operator delete
 * comes from the base -- see the inline one in include/dActor_c.h. */
#include "dBgActor_c.h"
#include "dBgCh_Actr.h"
#include "dCcAcPos_c.h"

struct ToxBox : dBgActor_c {
    s32 mPlayerActor;                   /* 0x320 */
    dBgCh_Actr mWithMeshClsn;           /* 0x324 */
    u8 pad_4e0[0x8];
    dCcAcPos_c mdCcAcPos_c;             /* 0x4e8 */
    u8 pad_528[0x6c];
    virtual ~ToxBox();
};

typedef char ToxBox_size_must_be_0x594[sizeof(ToxBox) == 0x594 ? 1 : -1];

ToxBox::~ToxBox()
{
}
