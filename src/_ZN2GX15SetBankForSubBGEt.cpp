//cpp
// @symbol _ZN2GX15SetBankForSubBGEt
/* GX::SetBankForSubBG() -- language-mode migration only (phase 1 of
 * notes/plan-cpp-language-mode.md). GX is a namespace, not a class: no
 * `this`, no vtable, no layout, so nothing outside this file can shift.
 * The body and every callee declaration are unchanged from the C version;
 * the symbol is now mangled by the compiler instead of spelled by hand.
 */
#include "types.h"
struct VramReg
{
  u16 w0;
  u16 pad[8];
  u16 f12;
};
extern struct VramReg data_020a6088;

extern "C" {
extern void Vram__Map(u16 bits);
}

namespace GX {
void SetBankForSubBG(u16 x){
  int s = (int) x;
  int new_var;
  unsigned int notx = ~((unsigned int) x);
  u16 old = data_020a6088.f12;
  new_var = notx & (data_020a6088.w0 | old);
  data_020a6088.f12 = x;
  data_020a6088.w0 = new_var;
  if (s <= 0x80)
  {
    if (s >= 0x80)
    {
      goto sub248;
    }
    else
      if (((s <= 4) && (s >= 0)) && (s != 0))
    {
      if (s == 4)
      {
        *((volatile unsigned char *) 0x4000242) = 0x84;
      }
    }
  }
  else
    if (s == 0x180)
  {
    *((volatile unsigned char *) 0x4000249) = 0x81;
    goto sub248;
  }
  goto done;
  sub248:
  *((volatile unsigned char *) 0x4000248) = 0x81;

  done:
  Vram__Map(data_020a6088.w0);

}
}
