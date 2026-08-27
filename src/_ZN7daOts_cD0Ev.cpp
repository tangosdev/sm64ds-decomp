//cpp
// @symbol _ZN7daOts_cD0Ev
/* D0, the deleting destructor, forced. daOts_c's destructor is defined inline
 * in its class body on purpose -- so every descendant inlines it -- which
 * means a TU that merely includes the header emits no out-of-line variant at
 * all. A `delete` expression asks for one that the inline copy cannot satisfy:
 * mwcc emits D0 (destroy through the class and its bases, then hand the object
 * to the inherited operator delete), and objisolate keeps it while dropping
 * this forcing function's own .text. The D1 half is forced the same way in
 * src/_ZN7daOts_cD1Ev.cpp. */
#include "daOts_c.h"

void daOts_c_EmitDeletingDestructor(daOts_c *p)
{
    delete p;
}
