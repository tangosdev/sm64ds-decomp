//cpp
// @symbol _ZN10daPgDfdr_c6RenderEv
/* daPgDfdr_c::Render -- vtable slot 9. Real C++ method over the shared header.
   Sub is a stand-in for ModelAnim's own vtable, whose slot 5 is Render: this
   header does not declare it callable, so the call still goes through a shadow
   -- but through &mModelAnim rather than through a 0x320-byte pad. The
   TextureSequence update takes mModelAnim's ModelComponents, which is what
   `this + 0x328` was (0x320 + Model's own +0x08). */
#include "daPgDfdr_c.h"
struct Sub { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void m(int); };
extern "C" void _ZN15TextureSequence6UpdateER15ModelComponents(void *, void *);
s32 daPgDfdr_c::Render() {
    _ZN15TextureSequence6UpdateER15ModelComponents(&mTextureSequence, &mModelAnim.data);
    Sub *b = (Sub *)&mModelAnim; b->m(0);
    return 1;
}
