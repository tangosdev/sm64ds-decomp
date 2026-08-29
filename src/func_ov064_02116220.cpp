//cpp
#include "daOts_c.h"

extern "C" {
typedef struct dActor_c dActor_c;
typedef struct BCA_File BCA_File;
struct dBgCh_Gnd { char buf[0x50]; };
void _ZN9dBgCh_GndC1Ev(dBgCh_Gnd* self);
void _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(dBgCh_Gnd* self, const Vector3* p, dActor_c* a);
int _ZN9dBgCh_Gnd10DetectClsnEv(dBgCh_Gnd* self);
int func_02037e38(unsigned int* p);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* self, BCA_File* f, int a, int b, unsigned int e);
void _ZN9dBgCh_GndD1Ev(dBgCh_Gnd* self);

void func_ov064_02116220(char* c){
  dBgCh_Gnd rg;
  _ZN9dBgCh_GndC1Ev(&rg);
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

  _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(&rg, &v, (dActor_c*)c);
  if (_ZN9dBgCh_Gnd10DetectClsnEv(&rg) != 0) {
    *(int*)(c+0x3f4) = *(int*)(rg.buf + 0x44);
    if (*(int*)(c+0x60) <= *(int*)(rg.buf + 0x44) + 0x14000) {
      int r = func_02037e38((unsigned int*)(rg.buf + 0x14));
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
        *(int*)(c+0x3ac) = *(int*)(rg.buf + 0x44) + 0x5000;
      }
    }
  }
  _ZN9dBgCh_GndD1Ev(&rg);
}
}
