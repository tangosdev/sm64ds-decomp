//cpp
// @symbol _ZN12dEnemyBase_c15IsGoingOffCliffER10dBgCh_Actrisbbi
/* recovered: named members + shared header */
#include "dEnemyBase_c.h"
#include "dBgCh_Lin.h"
extern "C" {
extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void* self);
extern short data_02082214[];
}

int dEnemyBase_c::IsGoingOffCliff(dBgCh_Actr &clsn_, Fix12i fix2, s16 a3,
                                  bool a4, bool a5, Fix12i fix6)
{
  void *clsn = &clsn_;
  Vector3 v1;
  Vector3 v2;
  ((char*)this)[0x106] = 0;
  if (_ZNK10dBgCh_Actr10IsOnGroundEv(clsn) != 0) {
    dBgCh_Lin line;
    v1.x = this->mPosX;
    v1.y = this->mPosY;
    v1.z = this->mPosZ;
    v1.y += fix6;
    v2.x = this->mPosX;
    v2.y = this->mPosY;
    v2.z = this->mPosZ;
    v2.y -= fix2;
    line.SetObjAndLine(v1, v2, this);
    if (a4 != 0)
      line.StartDetectingWater();
    if (line.DetectClsn()) {
      if (line.clsnDist - fix6 >= fix2)
        ((char*)this)[0x106] = 1;
      if (a5 == 0) {
        dBgPi result;
        line.CopyTo(result);
        if (result.GetClsnID() != -1) {
          ((char*)this)[0x106] = 1;
          return 1;
        }
      }
      Vector3 normal;
      line.surface.CopyNormalTo(normal);
      int idx = ((unsigned short)a3 >> 4) * 2;
      short s = data_02082214[idx + 1];
      if (normal.y < s)
        ((char*)this)[0x106] = 2;
    } else {
      ((char*)this)[0x106] = 1;
    }
  }
  return this->unk_106 != 0;
}
