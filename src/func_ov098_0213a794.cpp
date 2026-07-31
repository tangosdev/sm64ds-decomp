//cpp
// @symbol func_ov098_0213a794
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
extern "C" void* _ZN5Model8LoadFileER13SharedFilePtr(void* fp);
extern "C" void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, void* f, int a, int b);
extern "C" void _ZN8Platform21UpdateModelPosAndRotYEv(void* self);
extern "C" void _ZN8Platform19UpdateClsnPosAndRotEv(void* self);
extern "C" void* _ZN12MeshCollider8LoadFileER13SharedFilePtr(void* fp);
extern "C" void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void* self, void* f, void* m, int fx, short s, void* b);
extern "C" void func_020393c4(int* p, int v);
extern "C" void _ZN13RaycastGroundC1Ev(void* self);
extern "C" void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(void* self, void* pos, void* actor);
extern "C" int _ZN13RaycastGround10DetectClsnEv(void* self);
extern "C" void _ZN13RaycastGroundD1Ev(void* self);


struct RG { char b[0x4c]; };

extern "C" int func_ov098_0213a794(char* self, char** fp){
  RG rc;
  Vector3 v;
  int r2;
  int y;
  _ZN9ModelBase7SetFileEP8BMD_Fileii(self+0xd4, _ZN5Model8LoadFileER13SharedFilePtr((void*)fp[0]), 1, -1);
  _ZN8Platform21UpdateModelPosAndRotYEv(self);
  _ZN8Platform19UpdateClsnPosAndRotEv(self);
  _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(self+0x124, _ZN12MeshCollider8LoadFileER13SharedFilePtr((void*)fp[1]), self+0x2ec, 0x199, *(short*)(self+0x8e), (void*)fp[2]);
  func_020393c4((int*)(self+0x124), (int)&func_ov098_0213a8ec);
  v.x = *(int*)(self+0x5c);
  y = *(int*)(self+0x60);
  v.y = y;
  v.z = *(int*)(self+0x64);
  v.y = y - 0x64000;
  _ZN13RaycastGroundC1Ev(&rc);
  _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(&rc, &v, 0);
  *(int*)(self+0x32c) = v.y;
  if(_ZN13RaycastGround10DetectClsnEv(&rc)){
    *(int*)(self+0x32c) = *(int*)((char*)&rc + 0x44);
  }
  *(int*)(self+0x9c) = -0x4000;
  *(int*)(self+0xa0) = -0xc8000;
  r2 = 1;
  *(short*)(self+0x33a) = 4;
  *(unsigned char*)(self+0x340) = r2;
  *(int*)(self+0x320) = *(int*)(self+0x5c);
  *(int*)(self+0x324) = *(int*)(self+0x60);
  *(int*)(self+0x328) = *(int*)(self+0x64);
  *(int*)(self+0x330) = 0;
  if(*(unsigned short*)(self+0xc) != 0x53) r2 = 0;
  if(r2 != 0) *(unsigned char*)(self+0x342) = 1;
  _ZN13RaycastGroundD1Ev(&rc);
  return 1;
}
