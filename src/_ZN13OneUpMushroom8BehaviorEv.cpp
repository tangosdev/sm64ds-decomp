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
extern int _ZN12dEnemyBase_c14UpdateYoshiEatER10dBgCh_Actr(char* c, char* clsn);
extern void _ZN5dCc_c5ClearEv(char* c);
extern void _ZN5dCc_c6UpdateEv(char* c);
extern PMF data_ov002_0210dc00[];
#define AT(p, off) ((void*)(int)((char*)(p) + (off)))
}

int OneUpMushroom::Behavior()
{
  if(_ZN12dEnemyBase_c14UpdateYoshiEatER10dBgCh_Actr(((char*)this), ((char*)this)+0x144) != 0){
    func_ov002_020af4ec(((char*)this));
    _ZN5dCc_c5ClearEv((char*)&mdCcAc_c);
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
  _ZN5dCc_c5ClearEv((char*)&mdCcAc_c);
  _ZN5dCc_c6UpdateEv((char*)&mdCcAc_c);
  func_ov002_020af4ec(((char*)this));
  return 1;
}
