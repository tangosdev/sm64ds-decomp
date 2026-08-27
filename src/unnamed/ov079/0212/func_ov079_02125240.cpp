//cpp
// @symbol func_ov079_02125240
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"

struct Anim { int _pad; void *f; };
extern struct Anim *data_ov079_021275ec[];

extern "C" int _ZNK10dBgCh_Actr10IsOnGroundEv(void *thiz);
extern "C" void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *thiz, void *file, int a, int b, unsigned int e);
extern "C" void _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(void *thiz, struct Vector3 *v, int f);
extern "C" void func_020393c4(int *p, int v);
extern "C" void func_ov079_02123d4c(struct Vector3 *out, void *c);
extern "C" void func_0200fa04(void *c, struct Vector3 *v, int b);
extern "C" void _ZN8dActor_c13LandingDustAtER7Vector3b(void *thiz, struct Vector3 *v, int b);

extern "C" void func_ov079_02125240(char *c){
  struct Vector3 v;
  struct Vector3 a;
  struct Vector3 b;
  if(*(unsigned char*)(c+0x40c)) return;
  if(!_ZNK10dBgCh_Actr10IsOnGroundEv(c+0x110)) return;
  *(int*)(c+0xa8) = 0;
  func_01ffb098(c+0x418);
  func_01ffb0bc(c+0x418);
  *(unsigned char *)(((int)c + 0x40c)) += 1;
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c+0x2cc, data_ov079_021275ec[(unsigned char)c[0x414]*5+1]->f, 0x40000000, 0x1000, 0);
  v.x = *(int*)(c+0x5c);
  v.y = *(int*)(c+0x60);
  v.z = *(int*)(c+0x64);
  _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(c, &v, 0x7d0000);
  *(int*)(c+0x3b0) = 6;
  func_020393c4((int*)(c+0x418), (int)func_ov079_0212522c);
  if(*(unsigned char*)(c+0x414) != 0){
    func_ov079_02123d4c(&a, c);
    func_0200fa04(c, &a, 0);
  } else {
    func_ov079_02123d4c(&b, c);
    _ZN8dActor_c13LandingDustAtER7Vector3b(c, &b, 0);
  }
}
