//cpp
// @symbol _ZN11daBgSnwmn_c6RenderEv
/* daBgSnwmn_c::Render() -- vtable slot 9. See include/daBgSnwmn_c.h. `Sub`
 * is a local stand-in for Model's own 6-slot vtable shape (mModel1/2's
 * shared g5, whichever ModelBase virtual that is) -- kept as the previous
 * recovery pass wrote it, not the typed Model member, since it calls
 * through a vtable slot rather than a named method. Real C++ method now. */
#include "daBgSnwmn_c.h"
extern "C" int _ZN15TextureSequence6UpdateER15ModelComponents(void*, void*);
struct Sub { virtual int g0(); virtual int g1(); virtual int g2(); virtual int g3(); virtual int g4(); virtual int g5(void*); };
s32 daBgSnwmn_c::Render(){
  _ZN15TextureSequence6UpdateER15ModelComponents((char*)this+0x174, (char*)this+0xdc);
  ((Sub*)((char*)this+0xd4))->g5((char*)this+0x80);
  ((Sub*)((char*)this+0x124))->g5((char*)this+0x80);
  return 1;
}
