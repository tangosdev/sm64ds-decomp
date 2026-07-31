//cpp
// @symbol _ZN12CylinderClsn6UpdateEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "CylinderClsn.h"
extern "C" {
}

void CylinderClsn::Update()
{
  if(unk_018 & 1) return;
  void* h = data_0209cee8;
  *(void**)((char*)&unk_02c) = h;
  if(data_0209cee8) *(void**)((char*)data_0209cee8+0x28) = ((char*)this);
  data_0209cee8 = ((char*)this);
}
