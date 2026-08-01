//cpp
// @symbol _ZN10PyramidTag8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "PyramidTag.h"
extern "C" {
extern void _ZN9ActorBase18MarkForDestructionEv(void* c);
extern char* _ZN5Actor10FindWithIDEj(unsigned int id);
extern void _ZN12CylinderClsn5ClearEv(void* a);
extern void _ZN12CylinderClsn6UpdateEv(void* a);
}

int PyramidTag::Behavior()
{
  if(unk_0f8 != 0){
    unsigned int id = unk_108;
    if(id == 0){
      _ZN9ActorBase18MarkForDestructionEv(((char*)this));
      return 1;
    }
    char* a = _ZN5Actor10FindWithIDEj(id);
    if(a){
      unsigned char* p = (unsigned char*)(((int)a + 0x3b6));
      *p += 1;
    }
    _ZN9ActorBase18MarkForDestructionEv(((char*)this));
    return 1;
  }
  _ZN12CylinderClsn5ClearEv((char*)&mMovingCylinderClsn);
  _ZN12CylinderClsn6UpdateEv((char*)&mMovingCylinderClsn);
  return 1;
}
