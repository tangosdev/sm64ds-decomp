//cpp
// @symbol _ZN6Player17St_PunchKick_InitEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern int _ZN6Player15IsCollectingCapEv(void*);
extern int _ZN12CylinderClsn5ClearEv(void*);
extern int _ZN12CylinderClsn6UpdateEv(void*);
}

int Player::St_PunchKick_Init()
{
  *(int*)((char*)&mPeakY)=*(int*)((char*)&mPosY);
  *(char*)((char*)&unk_726)=0;
  *(short*)((char*)&mStateTimer)=2;
  *(char*)((char*)&unk_6e6)=0;
  if(*(unsigned char*)((char*)&mPunchKickStep)==2 || _ZN6Player15IsCollectingCapEv(((void*)this))) return 1;
  if(*(unsigned char*)((char*)&mIsMega)==0){
    func_ov002_020dc020(((void*)this));
    _ZN12CylinderClsn5ClearEv((char*)&mAttackClsn);
    _ZN12CylinderClsn6UpdateEv((char*)&mAttackClsn);
  } else {
    _ZN12CylinderClsn5ClearEv((char*)&mAttackClsn);
  }
  return 1;
}
