//cpp
// @symbol _ZN10dScTitle_c6RenderEv
/* dScTitle_c::Render() -- vtable slot 9. Real C++ method over the shared
 * header; this body never touches its own fields. */
#include "decl_common.h"
#include "dScTitle_c.h"
extern "C" {
extern int data_0209b2f4;
extern char data_0209e674[];
}
struct OamTmp { int x; };
namespace OAM { void EnableSubOAM(OamTmp*);
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" void _ZN3OAM12EnableSubOAMEv(OamTmp*);
 }
s32 dScTitle_c::Render()
{
    OamTmp tmp;
    OAM::_ZN3OAM12EnableSubOAMEv(&tmp);
    {
        int idx = data_0209e664;
        unsigned int v = ((data_0209b2f4 - data_0209b2f8 + 2) << 3) & 0xff;
        char* p = data_0209e674 + (idx << 3);
        *(int*)p = v | 0x80000;
        *(short*)(p + 4) = 0;
    }
    return 1;
}
