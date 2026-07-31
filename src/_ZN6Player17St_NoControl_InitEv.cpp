//cpp
// @symbol _ZN6Player17St_NoControl_InitEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern int func_ov002_020c9e40(void* c);
extern int func_ov002_020dab14(void* c);
struct PMF { int adj; int ptr; };
extern struct PMF data_ov002_02110884[];
}

int Player::St_NoControl_Init()
{
  mIsControlDisabled=1;
  unk_6e6=0;
  func_ov002_020c9e40(((char*)this));
  func_ov002_020dab14(((char*)this));
  mRidingShell=0;
  mHeldObj=0;
  unk_35c=0;
  mIsOpeningBigDoor=0;
  unsigned idx = mNoCtrlKind;
  struct PMF* m=&data_ov002_02110884[idx];
  char* obj=((char*)this) + (m->ptr >> 1);
  int fn=m->ptr;
  void (*f)(void*);
  if(fn & 1){
    f=*(void(**)(void*))(*(int*)obj + m->adj);
  } else {
    f=(void(*)(void*))m->adj;
  }
  f(obj);
  return 1;
}
