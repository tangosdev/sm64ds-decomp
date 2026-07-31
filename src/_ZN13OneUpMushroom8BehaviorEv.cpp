//cpp
// @symbol _ZN13OneUpMushroom8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "OneUpMushroom.h"
struct C;
typedef void (C::*PMF)();
struct C {
  char pad[0x500];
};
extern "C" {
extern int _ZN5Enemy14UpdateYoshiEatER12WithMeshClsn(char* c, char* clsn);
extern void _ZN12CylinderClsn5ClearEv(char* c);
extern void _ZN12CylinderClsn6UpdateEv(char* c);
extern PMF data_ov002_0210dc00[];
#define AT(p, off) ((void*)(int)(((long long)(int)((char*)(p) + (off)))))
}

int OneUpMushroom::Behavior()
{
  if(_ZN5Enemy14UpdateYoshiEatER12WithMeshClsn(((char*)this), ((char*)this)+0x144) != 0){
    func_ov002_020af4ec(((char*)this));
    _ZN12CylinderClsn5ClearEv((char*)&mMovingCylinderClsn);
    return 1;
  }
  mEatingPlayer = 0;
  {
    int old = unk_388;
    C* self = (C*)((char*)this);
    (self->*data_ov002_0210dc00[mMushroomType])();
    ++*(unsigned short*)AT(((char*)this), 0x100);
    ++*(unsigned short*)AT(((char*)this), 0x38c);
    if(old != unk_388){
      *(unsigned short*)AT(((char*)this), 0x100) = 0;
      *(unsigned short*)(((char*)this)+0x300+0x8c) = 0;
    }
  }
  _ZN12CylinderClsn5ClearEv((char*)&mMovingCylinderClsn);
  _ZN12CylinderClsn6UpdateEv((char*)&mMovingCylinderClsn);
  func_ov002_020af4ec(((char*)this));
  return 1;
}
