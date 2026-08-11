//cpp
// @symbol _ZN5Bully13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Bully.h"
extern "C" {
extern void func_ov064_02116ec0(char* c);
}

int Bully::InitResources()
{
    unk_3fc = 0;
    mFileTable = (int)&data_ov064_0211b834;
    func_ov064_02116ec0(((char*)this));
}
