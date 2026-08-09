//cpp
// @symbol func_ov006_020e17f8
/* ov006, 0x020e17f8, size 0x5c. RESOLVED: this held the name
 * _ZN6Player16St_WallJump_InitEv, and that symbol has now been moved to the
 * ov002 function at the same shared address -- the one that adds half a turn
 * to the facing angle and launches, reached from ov002's Player::State
 * pointer-to-member table (two `kind:load ... module:overlay(2)` sites).
 *
 * This function is reached by an arm_call from ov006, module:overlay(6), and
 * reads this+0x4eb0, this+0x4eb4 and this+0x4ee5 -- roughly 0x4700 bytes past
 * the end of a Player, which is 0x768. It was never Player's. Back to a
 * placeholder name until its own class is identified; the offsets stay
 * untyped, because typing them was always the wrong repair.
 */
#include "decl_common.h"
extern "C" {
extern int func_ov004_020afdd0(int a, int b, int c, int d, int e);
}

extern "C" void func_ov006_020e17f8(char *self)
{
  if(*(unsigned char*)(self+0x4ee5)==0) return;
  int x=*(int*)(self+0x4eb0);
  int y=*(int*)(self+0x4eb4);
  func_ov004_020afdd0((int)data_ov006_0213c2e4,(x>>12)-0x20,(y>>12)-8,-1,0);
}
