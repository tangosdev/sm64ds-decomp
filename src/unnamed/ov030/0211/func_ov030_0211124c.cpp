//cpp
// @symbol func_ov030_0211124c
/* recovered: shared common types */
#include "common.h"
extern "C" {
extern void dBgCh_Actr_UpdateContinuous_Veneer(void*);
extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void*);
extern void _ZN7fBase_c18MarkForDestructionEv(void*);
extern void _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned int, unsigned int, void*, void*, int, int);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int, int, int, int);

int func_ov030_0211124c(char *c, char *mc){
  dBgCh_Actr_UpdateContinuous_Veneer(mc);
  int g = _ZNK10dBgCh_Actr10IsOnGroundEv(mc);
  if(!g) return g;
  _ZN7fBase_c18MarkForDestructionEv(*(void**)(c+0x4dc));
  int px = *(int*)(c+0x5c);
  int pz = *(int*)(c+0x64);
  int py = *(int*)(c+0x60) + 0x96000;
  Vector3 p = { px, py, pz };
  _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(0xb2, (unsigned char)((*(unsigned int*)(c+8))&0xf) | 0x20, &p, 0, *(signed char*)(c+0xcc), -1);
  _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xb, *(int*)(c+0x5c), *(int*)(c+0x60), *(int*)(c+0x64));
  _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xd, *(int*)(c+0x5c), *(int*)(c+0x60), *(int*)(c+0x64));
  _ZN7fBase_c18MarkForDestructionEv(c);
}
}
