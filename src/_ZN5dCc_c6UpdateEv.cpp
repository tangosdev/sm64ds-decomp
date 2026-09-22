//cpp
// @symbol _ZN5dCc_c6UpdateEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "dCc_c.h"
extern "C" {
}

/* Link this node in at the head of the active list. The mirror of the unlink
   that the destructors run; see include/dCc_c.h. */
void dCc_c::Update()
{
  if(flags & 1) return;
  void* h = data_0209cee8;
  *(void**)&next = h;
  if(data_0209cee8) *(void**)((char*)data_0209cee8+0x28) = ((char*)this);
  data_0209cee8 = ((char*)this);
}
