//cpp
// @symbol _ZN9AnimationC2Ev
/* recovered: real C++ constructor -- the base-subobject variant of C1; the
 * body is identical (root class, nothing to chain). Every ROM caller is a
 * derived base step (MaterialChanger / TextureTransformer / TextureSequence /
 * ModelAnim construct their Animation at its base offset), so this variant's
 * enrolled home is this file and the C1 sibling it emits is stripped by
 * objisolate -- section 1 of notes/ctor-migration.md.
 */
#include "Animation.h"

Animation::Animation()
{
    currFrame = 0;
    speed = 0x1000;
}
