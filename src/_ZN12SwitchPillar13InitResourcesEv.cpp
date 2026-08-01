//cpp
// @symbol _ZN12SwitchPillar13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "SwitchPillar.h"
extern "C" {
int _ZN5Model8LoadFileER13SharedFilePtr(void*);
int _ZN12MeshCollider8LoadFileER13SharedFilePtr(void*);
int _ZN9ModelBase7SetFileEP8BMD_Fileii(void*,int,int,int);
int _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(int,void*);
int _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(void*,void*,int,int,unsigned);
int _ZN8Platform21UpdateModelPosAndRotYEv(void*);
int _ZN8Platform19UpdateClsnPosAndRotEv(void*);
int _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void*,int,void*,int,int,void*);
int _ZN16MeshColliderBase6EnableEP5Actor(void*,void*);
extern int data_0209caa0[];
}

int SwitchPillar::InitResources()
{
  _ZN5Model8LoadFileER13SharedFilePtr(data_ov012_021124d0);
  _ZN12MeshCollider8LoadFileER13SharedFilePtr(data_ov012_021124c8);
  _ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this)+0xd4, data_ov012_021124d0[1], 1, 0x14);
  _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(data_ov012_021124d0[1], data_ov012_02111c24);
  _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(((char*)this)+0x320, data_ov012_02111c24, 0, 0x1000, 0);
  _ZN8Platform21UpdateModelPosAndRotYEv(((char*)this));
  _ZN8Platform19UpdateClsnPosAndRotEv(((char*)this));
  _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(((char*)this)+0x124, data_ov012_021124c8[1], ((char*)this)+0x2ec, 0x1000, unk_08e, data_ov012_02111c90);
  _ZN16MeshColliderBase6EnableEP5Actor(((char*)this)+0x124, ((char*)this));
  int v = mPosY - 0x4b0000;
  mLoweredY = v;
  if(data_0209caa0[2] & 0x80000){
    mPosY = mLoweredY;
  }
  return 1;
}
