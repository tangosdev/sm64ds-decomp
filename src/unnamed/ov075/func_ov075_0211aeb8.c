#include "nitro/fx/fx.h"
extern int _ZN4cstd4fdivEii(int a, int b);
short func_ov075_0211aeb8(char* c, int x){
  int t = _ZN4cstd4fdivEii(0xffff, 0x16c000);
  int v = FX_MUL(x, t);
  return (short)(v - *(short*)(c+0xa0));
}
