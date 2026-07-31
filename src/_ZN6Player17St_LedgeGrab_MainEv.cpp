//cpp
// @symbol _ZN6Player17St_LedgeGrab_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_Animation.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern int _ZN6Player12FinishedAnimEv(void*);
extern void _ZN6Player11ChangeStateERNS_5StateE(void*,void*);
extern int _ZN6Player6IsAnimEj(void*,unsigned);
extern int _ZNK6Player14GetBodyModelIDEjb(void*,unsigned,int);
extern void _ZN5Sound9PlayBank0EjRK7Vector3(unsigned,void*);
extern int func_ov002_020bedd4(void*);
extern int data_ov002_0211013c[];
}

int Player::St_LedgeGrab_Main()
{
  if(_ZN6Player12FinishedAnimEv(((char*)this)))
    _ZN6Player11ChangeStateERNS_5StateE(((char*)this),data_ov002_0211013c);
  if(_ZN6Player6IsAnimEj(((char*)this),0x20)){
    int id=_ZNK6Player14GetBodyModelIDEjb(((char*)this),mParam&0xff,0);
    void* anim=*(void**)(((char*)this)+(id<<2)+0xdc);
    if(_ZNK9Animation12WillHitFrameEi((char*)anim+0x50,0x1e))
      _ZN5Sound9PlayBank0EjRK7Vector3(mGroundSoundType+0x40,((char*)this)+0x74);
  }
  func_ov002_020bedd4(((char*)this));
  return 1;
}
