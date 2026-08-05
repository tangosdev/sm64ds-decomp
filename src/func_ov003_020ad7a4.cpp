//cpp
// @symbol func_ov003_020ad7a4
// recovered name: dScTitle_c_Render
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* dScTitle_c::Render - recovered from vtable slot identity */
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
extern "C" int func_ov003_020ad7a4(void)
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
