//cpp
#include "daOts_c.h"
#include "dBgCh_Gnd.h"

extern "C" {
typedef struct dActor_c dActor_c;
typedef struct BCA_File BCA_File;
int func_02037e38(unsigned int* p);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* self, BCA_File* f, int a, int b, unsigned int e);

void func_ov064_02116220(char* c){
  dBgCh_Gnd rg;
  Vector3 v;
  int y = *(int*)(c+0x60);
  int yoff = *(int*)(c+0x3ec);
  int z = *(int*)(c+0x64);
  int x = *(int*)(c+0x5c);
  int sum = y + yoff;
  int yv = sum + 0x96000;
  v.x = x;
  v.y = yv;
  v.z = z;

  rg.SetObjAndPos(v, (dActor_c*)c);
  if (rg.DetectClsn() != 0) {
    *(int*)(c+0x3f4) = rg.clsnY;
    if (*(int*)(c+0x60) <= rg.clsnY + 0x14000) {
      int r = func_02037e38((unsigned int*)&rg.surface);
      if (r == 4 || r == 5 || r == 0x13) {
        *(int*)(c+0x398) = 5;
      } else if (r == 1) {
        *(int*)(c+0x398) = 4;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c+0x110,
            *(BCA_File**)(*(int*)(*(int*)(c+0x330) + 4) + 4), 0, 0x1000, 0);
        *(unsigned int *)(c + 0xb0) &= ~1u;
        ((daOts_c*)c)->PlayDeathSound();
        *(int*)(c+0x3a8) = *(int*)(c+0x5c);
        *(int*)(c+0x3ac) = *(int*)(c+0x60);
        *(int*)(c+0x3b0) = *(int*)(c+0x64);
        *(int*)(c+0x3ac) = rg.clsnY + 0x5000;
      }
    }
  }
}
}
