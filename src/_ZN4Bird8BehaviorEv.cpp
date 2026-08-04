//cpp
// @symbol _ZN4Bird8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "Bird.h"
extern "C" {
extern void Vec3_Asr(void* dst, void* src, int n);
extern void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationZ(void* m, short a);
extern void Matrix4x3_ApplyInPlaceToRotationY(void* m, short a);
extern int _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void* thiz, void* sm, void* m, int rad, int h, unsigned u);
extern int _ZN9Animation7AdvanceEv(void* a);
struct PMF { int fn; int ptr; };
struct Mtx { int w[12]; };
extern struct PMF data_ov009_02113c48[];
extern struct Mtx data_020a0e68;
}

int Bird::Behavior()
{
  int idx = unk_17c;
  struct PMF* m = &data_ov009_02113c48[idx];
  char* obj = ((char*)this) + (m->ptr >> 1);
  int p = m->ptr;
  void (*f)(void*);
  if(p & 1){
    f = *(void(**)(void*))(*(int*)obj + m->fn);
  } else {
    f = (void(*)(void*))m->fn;
  }
  f(obj);
  int tmp[3];
  Vec3_Asr(tmp, ((char*)this)+0x5c, 3);
  Matrix4x3_FromTranslation(&data_020a0e68, tmp[0], tmp[1], tmp[2]);
  mAngleY = mPrevAngleY;
  Matrix4x3_ApplyInPlaceToRotationZ(&data_020a0e68, mAngleZ);
  Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mAngleY);
  *(struct Mtx*)((char*)&unk_0f0) = data_020a0e68;
  _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(((char*)this), ((char*)this)+0x138, ((char*)this)+0xf0, 0x1e000, 0x7d0000, 0xf);
  _ZN9Animation7AdvanceEv((char*)&mAnimation);
  return 1;
}
