//cpp
// @symbol _ZN6ToxBoxD1Ev
/* D1, the complete-object destructor. The shadow struct below is the point: the
 * shared header spells this class FLAT, with the base's fields restated as padding,
 * so nothing in it tells mwcc there are subobjects to tear down. Naming the real
 * base and the real member types makes the compiler generate the ROM's teardown --
 * every member destroyed in reverse declaration order, then the base chain -- from
 * an empty body. The shared header is left alone; it has matched functions compiled
 * against its flat form. */
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
