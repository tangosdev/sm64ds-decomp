//cpp
// @symbol _ZN9Animation12SetAnimationEti5Fix12IiEt
/* Animation::SetAnimation(u16, int, Fix12<int>, u16): the declaration in
 * include/Animation.h is the true signature, but the definition stays a
 * mangled free function because of wall 6az in notes/mwccarm-codegen.md
 * (CW homes class-typed by-value parameters). Scalar args keep the bytes.
 */
#include "Animation.h"
extern "C" {
void _ZN9Animation12SetAnimationEti5Fix12IiEt(Animation *self, unsigned short numFrames, int flags, int speed, unsigned short startFrame) {
  *(int*)((char*)&self->numFramesAndFlags) = flags | (numFrames << 12);
  *(int*)((char*)&self->currFrame) = startFrame << 12;
  *(int*)((char*)&self->speed) = speed;
}
}
