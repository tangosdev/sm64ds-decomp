//cpp
// @symbol _ZN13MotherPenguin6RenderEv
/* MotherPenguin::Render -- vtable slot 9. Real C++ method over the shared
   header. TextureSequence::Update and Model::Render are DIRECT (non-virtual)
   calls in the ROM, kept as their literal mangled extern "C" spellings so the
   call sites stay direct; converting the callees to the real member types would
   turn Model::Render into a vtable dispatch and miss. The ModelComponents view
   at +0xdc and the Model view of +0xd4 are reached by raw offset. */
#include "MotherPenguin.h"
extern "C" {
int _ZN15TextureSequence6UpdateER15ModelComponents(void *self, void *mc);
void _ZN5Model6RenderEPK7Vector3(void *self, const void *scale);
}
int MotherPenguin::Render() {
    _ZN15TextureSequence6UpdateER15ModelComponents((char *)this + 0x138, (char *)this + 0xdc);
    _ZN5Model6RenderEPK7Vector3((char *)this + 0xd4, 0);
    return 1;
}
