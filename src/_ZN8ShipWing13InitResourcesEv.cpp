//cpp
// @symbol _ZN8ShipWing13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "ShipWing.h"
extern "C" {
int _ZN5Model8LoadFileER13SharedFilePtr(void*);
int _ZN9ModelBase7SetFileEP8BMD_Fileii(void*,int,int,int);
int _ZN8Platform21UpdateModelPosAndRotYEv(void*);
int _ZN8Platform19UpdateClsnPosAndRotEv(void*);
int _ZN12MeshCollider8LoadFileER13SharedFilePtr(void*);
int _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void*,int,void*,int,int,void*);
void func_020393d4(void* p, void* v);
void func_020393c4(void* p, void* v);
int _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void*,void*,int,int,int,int);
extern void _ZN16MeshColliderBase21UpdatePosWithVelocityERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_();
}

int ShipWing::InitResources()
{
  int m = _ZN5Model8LoadFileER13SharedFilePtr(data_ov036_0211408c);
  _ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this)+0xd4, m, 1, -1);
  _ZN8Platform21UpdateModelPosAndRotYEv(((char*)this));
  _ZN8Platform19UpdateClsnPosAndRotEv(((char*)this));
  int k = _ZN12MeshCollider8LoadFileER13SharedFilePtr(data_ov036_02114084);
  _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(((char*)this)+0x124, k, ((char*)this)+0x2ec, 0x1000, mAngleY, data_ov036_02112b48);
  func_020393d4(((char*)this)+0x124, (void*)_ZN16MeshColliderBase21UpdatePosWithVelocityERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);
  func_020393c4(((char*)this)+0x124, (void*)func_ov036_02111cc4);
  _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(((char*)this)+0x320, ((char*)this), 0x32000, 0x64000, 0, 0);
  unk_0a0 = -0x1e000;
  unk_09c = ~0x198;
  unk_4dc = mPosX;
  unk_4e0 = mPosY;
  unk_4e4 = mPosZ;
  return 1;
}
