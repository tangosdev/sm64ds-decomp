//cpp
// @symbol _ZN12dEnemyBase_c15IsGoingOffCliffER10dBgCh_Actrisbbi
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_dBgPi.h"
/* recovered: named members + shared header */
#include "dEnemyBase_c.h"
extern "C" {
extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void* self);
extern void _ZN9dBgCh_LinC1Ev(void* self);
extern void _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(void* self, void* a, void* b, void* act);
extern void _ZN5dBgCh19StartDetectingWaterEv(void* self);
extern int _ZN9dBgCh_Lin10DetectClsnEv(void* self);
extern void _ZN5dBgPiC1Ev(void* self);
extern void _ZNK5dBgPi6CopyToERS_(void* self, void* other);
extern void _ZN5dBgPiD1Ev(void* self);
extern void _ZN9dBgCh_LinD1Ev(void* self);
extern void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(void* self, void* v);
extern short data_02082214[];
}

int dEnemyBase_c::IsGoingOffCliff(dBgCh_Actr &clsn_, Fix12i fix2, s16 a3,
                                  bool a4, bool a5, Fix12i fix6)
{
  void *clsn = &clsn_;
  Vector3 v1;
  Vector3 v2;
  char cr[0x28];
  Vector3 normal;
  char rl[0x7c];
  ((char*)this)[0x106] = 0;
  if (_ZNK10dBgCh_Actr10IsOnGroundEv(clsn) != 0) {
    _ZN9dBgCh_LinC1Ev(rl);
    v1.x = this->mPosX;
    v1.y = this->mPosY;
    v1.z = this->mPosZ;
    v1.y += fix6;
    v2.x = this->mPosX;
    v2.y = this->mPosY;
    v2.z = this->mPosZ;
    v2.y -= fix2;
    _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(rl, &v1, &v2, ((char*)this));
    if (a4 != 0)
      _ZN5dBgCh19StartDetectingWaterEv(rl);
    if (_ZN9dBgCh_Lin10DetectClsnEv(rl) != 0) {
      if (*(int*)(rl + 0x60) - fix6 >= fix2)
        ((char*)this)[0x106] = 1;
      if (a5 == 0) {
        _ZN5dBgPiC1Ev(cr);
        _ZNK5dBgPi6CopyToERS_(rl + 0x10, cr);
        if (_ZNK5dBgPi9GetClsnIDEv(cr) != -1) {
          ((char*)this)[0x106] = 1;
          _ZN5dBgPiD1Ev(cr);
          _ZN9dBgCh_LinD1Ev(rl);
          return 1;
        }
        _ZN5dBgPiD1Ev(cr);
      }
      _ZNK11SurfaceInfo12CopyNormalToER7Vector3(rl + 0x14, &normal);
      int idx = ((unsigned short)a3 >> 4) * 2;
      short s = data_02082214[idx + 1];
      if (normal.y < s)
        ((char*)this)[0x106] = 2;
    } else {
      ((char*)this)[0x106] = 1;
    }
    _ZN9dBgCh_LinD1Ev(rl);
  }
  return this->unk_106 != 0;
}
