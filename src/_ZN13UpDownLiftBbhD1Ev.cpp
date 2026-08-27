//cpp
// @symbol _ZN13UpDownLiftBbhD1Ev
/* D1, the complete-object destructor. The base chain, the intermediate vptr
 * stores and the member teardown are all the compiler's -- the class is spelt
 * against its real base header rather than a flat shadow. */
#include "dBgActor_c.h"

struct UpDownLiftBbh : dBgActor_c {
    virtual ~UpDownLiftBbh();
};

UpDownLiftBbh::~UpDownLiftBbh()
{
}
