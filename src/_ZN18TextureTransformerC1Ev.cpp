//cpp
// @symbol _ZN18TextureTransformerC1Ev
/* recovered: real C++ constructor -- the base step (bl _ZN9AnimationC2Ev)
 * and the vptr store are synthesized from the declared Animation()/~X()
 * in the header; the single body statement zeroes the Bx?_File pointer at
 * +0x10. The C2 sibling this TU also emits is stripped by objisolate --
 * section 1 of notes/ctor-migration.md.
 */
#include "TextureTransformer.h"

TextureTransformer::TextureTransformer()
{
    file = 0;
}
