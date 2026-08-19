// @symbol func_ov032_02111350
/* recovered: shared common types */
#include "common.h"
extern char* _ZN8dActor_c13ClosestPlayerEv(void);
extern int _ZNK10dBgCh_Actr8IsOnWallEv(void* self);
extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void* self);
extern int Vec3_HorzDist(const struct Vector3* a, const struct Vector3* b);
extern char data_ov032_02113abc[];
extern char data_0209f32c[];

int func_ov032_02111350(char* c) {
  if (_ZN8dActor_c13ClosestPlayerEv() == 0) return 1;
  if (_ZNK10dBgCh_Actr8IsOnWallEv(c+0x190) != 0 || _ZNK10dBgCh_Actr10IsOnGroundEv(c+0x190) != 0) return 1;
  if (Vec3_HorzDist((struct Vector3*)(c+0x40c), (struct Vector3*)(c+0x5c)) > 0x4b0000) return 1;
  if (*(char**)(c+0x3b0) != (char*)data_ov032_02113abc) {
    if (*(int*)data_0209f32c < *(int*)(c+0x60)) return 1;
  }
  return 0;
}
