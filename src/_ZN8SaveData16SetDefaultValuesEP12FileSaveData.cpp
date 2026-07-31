//cpp
// @symbol _ZN8SaveData16SetDefaultValuesEP12FileSaveData
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "SaveData.h"
struct FileSaveData;
extern "C" {
}

void SaveData::SetDefaultValues(FileSaveData * fsd_)
{
    void* fsd = (void*)fsd_;

  func_0205a588(((void*)this), 0, 0x44);
  *(int*)((void*)this) = 0x30303038;
  *(unsigned char*)((char*)&unk_041) = 3;
  *(int*)(((long long)(int)((char*)&unk_008))) |= 8;
  *(unsigned char*)((char*)&unk_042) = 0;
}
