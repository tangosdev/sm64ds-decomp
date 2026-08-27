//cpp
// @symbol _ZN15ChainChompFence13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "ChainChompFence.h"
extern "C" {
extern int _ZN5Model8LoadFileER13SharedFilePtr(void*);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void*,int,int,int);
extern int _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void*);
extern int _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void*);
extern int _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void*);
extern int _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void*,int,void*,int,int,void*);
}
/* These three live in ov014, this file's own overlay. They were named off ov021 and
 * ov022, which share the same load window and so define their own symbols at the same
 * addresses -- but never at the same time as ov014, so those names cannot be what this
 * code reaches. Same address either way, so the bytes never noticed. */
extern int data_ov014_021149b8[];
extern int data_ov014_02114558[];
extern int data_ov014_021149c0[];

int ChainChompFence::InitResources()
{
  int m = _ZN5Model8LoadFileER13SharedFilePtr(data_ov014_021149c0);
  _ZN9ModelBase7SetFileEP8BMD_Fileii((char*)&mModel, m, 1, -1);
  _ZN10dBgActor_c21UpdateModelPosAndRotYEv(((char*)this));
  _ZN10dBgActor_c19UpdateClsnPosAndRotEv(((char*)this));
  int k = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(data_ov014_021149b8);
  _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block((char*)&mMeshCollider, k, (char*)&mClsnMat, 0x1000, mAngleY, (void*)data_ov014_02114558);
  return 1;
}
