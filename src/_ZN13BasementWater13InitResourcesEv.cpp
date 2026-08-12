//cpp
// @symbol _ZN13BasementWater13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "BasementWater.h"
#include "MeshColliderBase.h"
extern "C" {
int _ZN5Model8LoadFileER13SharedFilePtr(void*);
int _ZN12MeshCollider8LoadFileER13SharedFilePtr(void*);
int _ZN9ModelBase7SetFileEP8BMD_Fileii(void*,int,int,int);
int _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(int,void*);
int _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(void*,void*,int,int,unsigned);
int _ZN8Platform21UpdateModelPosAndRotYEv(void*);
int _ZN8Platform19UpdateClsnPosAndRotEv(void*);
int _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void*,int,void*,int,int,void*);
extern int data_0209caa0[];
}

int BasementWater::InitResources()
{
  _ZN5Model8LoadFileER13SharedFilePtr(data_ov012_021124d0);
  _ZN12MeshCollider8LoadFileER13SharedFilePtr(data_ov012_021124c8);
  _ZN9ModelBase7SetFileEP8BMD_Fileii(&mModel, data_ov012_021124d0[1], 1, 0x14);
  _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(data_ov012_021124d0[1], data_ov012_02111c24);
  _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(((char*)this)+0x320, data_ov012_02111c24, 0, 0x1000, 0);
  _ZN8Platform21UpdateModelPosAndRotYEv(((char*)this));
  _ZN8Platform19UpdateClsnPosAndRotEv(((char*)this));
  _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(&mMeshCollider, data_ov012_021124c8[1], &mClsnMat, 0x1000, mAngleY, data_ov012_02111c90);
  ((MeshColliderBase *)(&mMeshCollider))->Enable((Actor *)(((char*)this)));
  int v = mPosY - 0x4b0000;
  mLoweredY = v;
  if(data_0209caa0[2] & 0x80000){
    mPosY = mLoweredY;
  }
  return 1;
}
