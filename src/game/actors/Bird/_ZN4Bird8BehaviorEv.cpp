//cpp
// @symbol _ZN4Bird8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "Bird.h"
extern "C" {
extern void Vec3_Asr(void* dst, void* src, int n);
extern void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationZ(void* m, short a);
extern void Matrix4x3_ApplyInPlaceToRotationY(void* m, short a);
extern int _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void* thiz, void* sm, void* m, int rad, int h, unsigned u);
struct Mtx { int w[12]; };
extern struct Mtx data_020a0e68;
}

typedef void (Bird::*BirdState)();
extern "C" BirdState data_ov009_02113c48[];

int Bird::Behavior()
{
  (this->*data_ov009_02113c48[mState])();
  int tmp[3];
  Vec3_Asr(tmp, &mPosX, 3);
  Matrix4x3_FromTranslation(&data_020a0e68, tmp[0], tmp[1], tmp[2]);
  mAngleY = mPrevAngleY;
  Matrix4x3_ApplyInPlaceToRotationZ(&data_020a0e68, mAngleZ);
  Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mAngleY);
  *(struct Mtx*)((char*)&mModelAnim.mat4x3) = data_020a0e68;
  _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(this, &mShadowModel, &mModelAnim.mat4x3, 0x1e000, 0x7d0000, 0xf);
  mModelAnim.Advance();
  return 1;
}
