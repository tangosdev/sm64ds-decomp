//cpp
// @symbol _ZN6Player16St_WallJump_InitEv
/* NOT a Player method. ov006, 0x020e17f8.
 *
 * Reads this+0x4eb0, this+0x4eb4 and this+0x4ee5. sizeof(Player) is 0x768, so
 * these are ~0x4700 bytes past the end of the object. Player.h currently
 * carries a footnote about fields "far outside the object" -- this file is
 * where that came from.
 *
 * Detached from Player.h; see _ZN6Player23St_InYoshiMouth_CleanupEv.cpp.
 */
#include "decl_common.h"
extern int func_ov004_020afdd0(int a, int b, int c, int d, int e);

extern "C" void _ZN6Player16St_WallJump_InitEv(char *self)
{
  if(*(unsigned char*)(self+0x4ee5)==0) return;
  int x=*(int*)(self+0x4eb0);
  int y=*(int*)(self+0x4eb4);
  func_ov004_020afdd0((int)data_ov006_0213c2e4,(x>>12)-0x20,(y>>12)-8,-1,0);
}
