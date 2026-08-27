//cpp
// @symbol _ZN6Player17St_NoControl_InitEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern int Player_DisableInteraction(void* c);
extern int Player_ReleaseHeldActor(void* c);
struct PMF { int adj; int ptr; };
extern struct PMF data_ov002_02110884[];
}

int Player::St_NoControl_Init()
{
  mIsControlDisabled=1;
  mStatePhase=0;
  Player_DisableInteraction(((char*)this));
  Player_ReleaseHeldActor(((char*)this));
  mRidingShell=0;
  mHeldObj=0;
  mGrabbedByActor=0;
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
