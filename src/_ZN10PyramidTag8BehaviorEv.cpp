//cpp
// @symbol _ZN10PyramidTag8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "PyramidTag.h"
extern "C" {
extern void _ZN7fBase_c18MarkForDestructionEv(void* c);
extern char* _ZN8dActor_c10FindWithIDEj(unsigned int id);
extern void _ZN5dCc_c5ClearEv(void* a);
extern void _ZN5dCc_c6UpdateEv(void* a);
}

int PyramidTag::Behavior()
{
  if(mdCcAc_c.otherOwner != 0){
    unsigned int id = unk_108;
    if(id == 0){
      _ZN7fBase_c18MarkForDestructionEv(((char*)this));
      return 1;
    }
    char* a = _ZN8dActor_c10FindWithIDEj(id);
    if(a){
      unsigned char* p = (unsigned char*)(((int)a + 0x3b6));
      *p += 1;
    }
    _ZN7fBase_c18MarkForDestructionEv(((char*)this));
    return 1;
  }
  _ZN5dCc_c5ClearEv((char*)&mdCcAc_c);
  _ZN5dCc_c6UpdateEv((char*)&mdCcAc_c);
  return 1;
}
