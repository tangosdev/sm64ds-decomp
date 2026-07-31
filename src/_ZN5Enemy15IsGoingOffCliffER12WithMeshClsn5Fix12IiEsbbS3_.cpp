//cpp
// @symbol _ZN5Enemy15IsGoingOffCliffER12WithMeshClsn5Fix12IiEsbbS3_
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_ClsnResult.h"
/* recovered: named members + shared header */
#include "Enemy.h"
extern "C" {
extern int _ZNK12WithMeshClsn10IsOnGroundEv(void* self);
extern void _ZN11RaycastLineC1Ev(void* self);
extern void _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(void* self, void* a, void* b, void* act);
extern void _ZN4BgCh19StartDetectingWaterEv(void* self);
extern int _ZN11RaycastLine10DetectClsnEv(void* self);
extern void _ZN10ClsnResultC1Ev(void* self);
extern void _ZNK10ClsnResult6CopyToERS_(void* self, void* other);
extern void _ZN10ClsnResultD1Ev(void* self);
extern void _ZN11RaycastLineD1Ev(void* self);
extern void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(void* self, void* v);
extern short data_02082214[];
}

extern "C" int _ZN5Enemy15IsGoingOffCliffER12WithMeshClsn5Fix12IiEsbbS3_(struct Enemy *self, void* clsn, int fix2, short a3, unsigned char a4, unsigned char a5, int fix6) {
  Vector3 v1;
  Vector3 v2;
  char cr[0x28];
  Vector3 normal;
  char rl[0x7c];
  ((char*)self)[0x106] = 0;
  if (_ZNK12WithMeshClsn10IsOnGroundEv(clsn) != 0) {
    _ZN11RaycastLineC1Ev(rl);
    v1.x = self->mPosX;
    v1.y = self->mPosY;
    v1.z = self->mPosZ;
    v1.y += fix6;
    v2.x = self->mPosX;
    v2.y = self->mPosY;
    v2.z = self->mPosZ;
    v2.y -= fix2;
    _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(rl, &v1, &v2, ((char*)self));
    if (a4 != 0)
      _ZN4BgCh19StartDetectingWaterEv(rl);
    if (_ZN11RaycastLine10DetectClsnEv(rl) != 0) {
      if (*(int*)(rl + 0x60) - fix6 >= fix2)
        ((char*)self)[0x106] = 1;
      if (a5 == 0) {
        _ZN10ClsnResultC1Ev(cr);
        _ZNK10ClsnResult6CopyToERS_(rl + 0x10, cr);
        if (_ZNK10ClsnResult9GetClsnIDEv(cr) != -1) {
          ((char*)self)[0x106] = 1;
          _ZN10ClsnResultD1Ev(cr);
          _ZN11RaycastLineD1Ev(rl);
          return 1;
        }
        _ZN10ClsnResultD1Ev(cr);
      }
      _ZNK11SurfaceInfo12CopyNormalToER7Vector3(rl + 0x14, &normal);
      int idx = ((unsigned short)a3 >> 4) * 2;
      short s = data_02082214[idx + 1];
      if (normal.y < s)
        ((char*)self)[0x106] = 2;
    } else {
      ((char*)self)[0x106] = 1;
    }
    _ZN11RaycastLineD1Ev(rl);
  }
  return self->unk_106 != 0;
}
