//cpp
// @symbol _ZN8SignPostD0Ev
/* D0, the deleting destructor, forced. SignPost's destructor is defined inline
 * in its class body on purpose -- so every descendant inlines it -- which
 * means a TU that merely includes the header emits no out-of-line variant at
 * all. A `delete` expression asks for one that the inline copy cannot satisfy:
 * mwcc emits D0 (destroy through the class and its bases, then hand the object
 * to the inherited operator delete), and objisolate keeps it while dropping
 * this forcing function's own .text. The D1 half is forced the same way in
 * src/_ZN8SignPostD1Ev.cpp. */
#include "SignPost.h"

void SignPost_EmitDeletingDestructor(SignPost *p)
{
    delete p;
}
