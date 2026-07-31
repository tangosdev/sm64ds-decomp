//cpp
// @symbol func_ov030_0211124c
/* recovered: shared common types */
#include "common.h"
extern "C" {
extern void func_020383fc(void*);
extern int _ZNK12WithMeshClsn10IsOnGroundEv(void*);
extern void _ZN9ActorBase18MarkForDestructionEv(void*);
extern void _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned int, unsigned int, void*, void*, int, int);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int, int, int, int);

int func_ov030_0211124c(char *c, char *mc){
  func_020383fc(mc);
  int g = _ZNK12WithMeshClsn10IsOnGroundEv(mc);
  if(!g) return g;
  _ZN9ActorBase18MarkForDestructionEv(*(void**)(c+0x4dc));
  int px = *(int*)(c+0x5c);
  int pz = *(int*)(c+0x64);
  int py = *(int*)(c+0x60) + 0x96000;
  Vector3 p = { px, py, pz };
  _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0xb2, (unsigned char)((*(unsigned int*)(c+8))&0xf) | 0x20, &p, 0, *(signed char*)(c+0xcc), -1);
  _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xb, *(int*)(c+0x5c), *(int*)(c+0x60), *(int*)(c+0x64));
  _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xd, *(int*)(c+0x5c), *(int*)(c+0x60), *(int*)(c+0x64));
  _ZN9ActorBase18MarkForDestructionEv(c);
}
}
