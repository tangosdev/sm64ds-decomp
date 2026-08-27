//cpp
// @symbol _ZN12FlamethrowerD1Ev
/* D1, the complete-object destructor. The shadow struct below is the point: the
 * shared header spells this class FLAT, with the base's fields restated as padding,
 * so nothing in it tells mwcc there are subobjects to tear down. Naming the real
 * base and the real member types makes the compiler generate the ROM's teardown --
 * every member destroyed in reverse declaration order, then the base chain -- from
 * an empty body. The shared header is left alone; it has matched functions compiled
 * against its flat form. */
#include "dActor_c.h"
#include "dCcPos_c.h"

struct Flamethrower : dActor_c {
    u8 pad_0d0[0x4];
    dCcPos_c mClsn[12];                 /* 0x0d4 */
    Vector3 mPartPos[12];               /* 0x3a4 */
    u8 pad_434[0x38];
    virtual ~Flamethrower();
};

typedef char Flamethrower_size_must_be_0x46c[sizeof(Flamethrower) == 0x46c ? 1 : -1];

Flamethrower::~Flamethrower()
{
}
