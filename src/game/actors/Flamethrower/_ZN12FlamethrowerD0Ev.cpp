//cpp
// @symbol _ZN12FlamethrowerD0Ev
/* D0, the deleting destructor. Same shadow struct as the D1 file beside this one;
 * mwcc emits D0, D1 and D2 together from one destructor definition and objisolate
 * keeps the variant this file's delinks entry names. The trailing operator delete
 * comes from the base -- see the inline one in include/dActor_c.h. */
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
