//cpp
// @symbol _ZN12CylinderClsn6UpdateEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "CylinderClsn.h"
extern "C" {
}

/* Link this node in at the head of the active list. The mirror of the unlink
   that the destructors run; see include/CylinderClsn.h. */
void CylinderClsn::Update()
{
  if(flags & 1) return;
  void* h = data_0209cee8;
  *(void**)&next = h;
  if(data_0209cee8) *(void**)((char*)data_0209cee8+0x28) = ((char*)this);
  data_0209cee8 = ((char*)this);
}
