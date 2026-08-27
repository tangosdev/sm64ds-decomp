//cpp
// @symbol _ZN4ViewD0Ev
/* D0, the DELETING destructor: run the destructor body, then hand the object
 * to operator delete. Longer than the D1/D2 pair, but built from the same one
 * `View::~View()` definition -- so this file carries the same definition as its
 * D1 sibling, and objisolate keeps the variant the delinks entry names. */
#include "dBase_c.h"

struct View : dBase_c {
    virtual ~View();
};

View::~View()
{
}
