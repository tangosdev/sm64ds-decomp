//cpp
#include "types.h"
// @symbol _ZN6Player12St_Jump_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern int func_ov002_020eeca8(void*, void*);
extern int func_ov002_020e28d4(void*, int, int);
extern int _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
extern u32 _ZNK6Player14GetBodyModelIDEjb(void*, u32, int);
extern void _ZN5Sound9PlayBank0EjRK7Vector3(u32, void*);
extern int Player_AdvanceAnims(void*);

extern char data_ov002_02110424[];
extern u8 data_020a0e40;
extern u16 data_0209f49e[];
}

int Player::St_Jump_Main()
{
  func_ov002_020eeca8((char*)((void*)this) + 0x380, ((void*)this));
  func_ov002_020e28d4(((void*)this), 0xd00, 0x800);

  if (*(u8*)((char*)&mIsAirborne) == 0) {
    if (*(u16*)((char*)&mStateTimer) != 0) {
      u16* q = (u16*)((char*)&mStateFlags);
      *q |= 0x100;
    }
    _ZN6Player11ChangeStateERNS_5StateE(((void*)this), data_ov002_02110424);
  } else {
    {
      int off = data_020a0e40 * 0x18;
      u8* p = (u8*)((char*)data_0209f4ab + off);
      if (*p != 0) {
        *p = 5;
      }
      if (*(u16*)((char*)data_0209f49e + off) & 2) {
        *(u16*)((char*)&mStateTimer) = 5;
      }
    }

    if (func_ov002_020e2664(((void*)this))) return 1;

    if (*(u8*)((char*)&mJumpComboStage) == 2) {
      *(s32*)((char*)&mVertAccel) = -0x4000;
      *(s32*)((char*)&mTerminalVelocity) = -0x4b000;
      if (*(s32*)((char*)&mVertSpeed) >= 0) {
        *(s32*)((char*)&mVertAccel) = -0x3400;
      }

      {
        u32 id = _ZNK6Player14GetBodyModelIDEjb(((void*)this), *(u32*)((char*)&param1) & 0xff, 0);
        void* anim = *(void**)((char*)((void*)this) + (id << 2) + 0xdc);
        u32 w = *(u32*)((char*)((long long)(int)((char*)anim + 0x50)) + 8);
        u16 t = (u16)(w >> 12);
        if (t == 4 || t == 0x18 || t == 0x2c) {
          _ZN5Sound9PlayBank0EjRK7Vector3(0xf, (char*)((void*)this) + 0x74);
        }
      }
    } else {
      int idx = *(int*)((char*)&param1);
      if (*(u8*)((char*)&mIsMega) != 0) {
        idx = 0;
      }
      int* row = &data_ov002_0211073c[idx * 2];
      int v = row[1];
      void* p2 = (char*)((void*)this) + (v >> 1);
      int (*f)(void*);
      if (v & 1) {
        f = *(int (**)(void*))((char*)(*(int**)p2) + row[0]);
      } else {
        f = (int (*)(void*))row[0];
      }
      f(p2);
    }
  }

  Player_AdvanceAnims(((void*)this));
  return 1;
}
