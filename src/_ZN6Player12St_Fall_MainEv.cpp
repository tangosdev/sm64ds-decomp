//cpp
// @symbol _ZN6Player12St_Fall_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
typedef int Fix12i;
extern int func_ov002_020e28d4(void*, int, int);
extern int _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
extern int func_ov002_020bedd4(void*);
extern Fix12i func_ov002_020bf30c(void* c, Fix12i a);
extern char data_ov002_02110424[];

int Player::St_Fall_Main()
{
  func_ov002_020e28d4(((void*)this), 0x1800, 0x800);
  if (*(unsigned char*)((char*)&mIsAirborne) == 0) {
    _ZN6Player11ChangeStateERNS_5StateE(((void*)this), data_ov002_02110424);
  } else if (func_ov002_020e2664(((void*)this))) {
    return 1;
  }
  func_ov002_020bedd4(((void*)this));
  if (*(int*)((char*)&mHorzSpeed) >= func_ov002_020bf30c(((void*)this), 0x28000))
    *(int*)((char*)&mHorzSpeed) = func_ov002_020bf30c(((void*)this), 0x28000);
  return 1;
}
