//cpp
// @symbol _ZN7daDsn_c13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "daDsn_c.h"

int daDsn_c::InitResources()
{
    mFileTable = (int)data_ov091_02135138;
    int r = Init();
    char *p = ((char *)this) + 0x300;
    mState = 0;
    *(short*)(p + 0xa0) = 0;
    mTriggered = 0;
    return r;
}
