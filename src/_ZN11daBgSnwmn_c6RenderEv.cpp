//cpp
// @symbol _ZN11daBgSnwmn_c6RenderEv
/* daBgSnwmn_c::Render() -- vtable slot 9. See include/daBgSnwmn_c.h. `Sub`
 * is a local stand-in for Model's own 6-slot vtable shape (mModel1/2's
 * shared g5, whichever ModelBase virtual that is) -- kept as the previous
 * recovery pass wrote it, not the typed Model member, since it calls
 * through a vtable slot rather than a named method. */
extern "C" {
extern int _ZN15TextureSequence6UpdateER15ModelComponents(void*, void*);
}
struct Sub { virtual int g0(); virtual int g1(); virtual int g2(); virtual int g3(); virtual int g4(); virtual int g5(void*); };
extern "C" {
int _ZN11daBgSnwmn_c6RenderEv(char* c){
  _ZN15TextureSequence6UpdateER15ModelComponents(c+0x174, c+0xdc);
  ((Sub*)(c+0xd4))->g5(c+0x80);
  ((Sub*)(c+0x124))->g5(c+0x80);
  return 1;
}
}
