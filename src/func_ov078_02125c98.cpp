//cpp
// @symbol func_ov078_02125c98
/* recovered: shared common types */
#include "common.h"
#include "dBgCh_Gnd.h"
/* func_ov078_02125c98 at 0x02125c98 (ov078), size 0x148
 * Matched byte-for-byte with mwccarm 1.2/sp2p3.
 * flags: -O4,p -enum int -lang c++ -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc
 */
struct dActor_c;

extern "C" {
int _ZNK10dBgCh_Actr10IsOnGroundEv(void* self);
void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void* self, void* sm, void* mtx, int fix, int t, unsigned int j);
}
extern Matrix4x3 IDENTITY_MATRIX4X3;

extern "C" void func_ov078_02125c98(char* c) {
  int h = *(int*)(c+0x60);
  if (_ZNK10dBgCh_Actr10IsOnGroundEv(c+0x110) == 0) {
    dBgCh_Gnd rg;
    rg.SetObjAndPos(*(const Vector3*)(c+0x5c), 0);
    if (rg.DetectClsn() != 0)
      h = rg.clsnY;
  }
  int b = (*(int*)(c+0xb0) & 0x4000) != 0;
  if (b) {
    char* p = *(char**)(c+0x494);
    if (p != 0)
      h = *(int*)(p+0x60);
  }
  int ip = *(int*)(c+0x60) - h;
  if (ip <= 0x1000)
    ip = 0x1000;
  int r8 = 0x15e000 - (int)(((long long)ip * 0x180 + 0x800) >> 12);
  if (r8 < 0xa000)
    r8 = 0xa000;
  *(Matrix4x3*)(c+0x434) = IDENTITY_MATRIX4X3;
  *(int*)(c+0x458) = *(int*)(c+0x5c) >> 3;
  *(int*)(c+0x45c) = *(int*)(c+0x60) >> 3;
  *(int*)(c+0x460) = *(int*)(c+0x64) >> 3;
  _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
      c, c+0x3f8, c+0x434, r8, ip + 0x28000, 0xf);
}
