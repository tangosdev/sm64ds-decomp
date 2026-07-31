//cpp
// @symbol _ZN6Thwomp13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Thwomp.h"
extern int func_ov091_02133254(char*);

int Thwomp::InitResources()
{
    mFileTable = (int)data_ov091_02135138;
    int r = func_ov091_02133254(((char *)this));
    char *p = ((char *)this) + 0x300;
    mState = 0;
    *(short*)(p + 0xa0) = 0;
    mTriggered = 0;
    return r;
}
