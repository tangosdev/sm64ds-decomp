//cpp
// @symbol _ZN4ViewD1Ev
/* D1, the complete-object destructor. The base chain, the intermediate vptr
 * stores and the member teardown are all the compiler's -- the class is spelt
 * against its real base header rather than a flat shadow. */
#include "dBase_c.h"

struct View : dBase_c {
    virtual ~View();
};

View::~View()
{
}
