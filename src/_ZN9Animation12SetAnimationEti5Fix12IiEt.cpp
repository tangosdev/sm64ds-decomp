//cpp
// @symbol _ZN9Animation12SetAnimationEti5Fix12IiEt
/* recovered: named members + shared header */
#include "Animation.h"
extern "C" {
void _ZN9Animation12SetAnimationEti5Fix12IiEt(struct Animation *self, unsigned short a, int i, int fix, unsigned short b) {
  *(int*)((char*)&self->mFrameCountAndFlags)=i|(a<<12);
  *(int*)((char*)&self->unk_008)=b<<12;
  *(int*)((char*)&self->unk_00c)=fix;
}
}
