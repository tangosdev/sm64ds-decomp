//cpp
// @symbol _ZN15ChainChompFence13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "ChainChompFence.h"
extern int _ZN5Model8LoadFileER13SharedFilePtr(void*);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void*,int,int,int);
extern int _ZN8Platform21UpdateModelPosAndRotYEv(void*);
extern int _ZN8Platform19UpdateClsnPosAndRotEv(void*);
extern int _ZN12MeshCollider8LoadFileER13SharedFilePtr(void*);
extern int _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void*,int,void*,int,int,void*);
extern int data_ov021_021149b8[];
extern int data_ov022_02114558[];

int ChainChompFence::InitResources()
{
  int m = _ZN5Model8LoadFileER13SharedFilePtr(data_ov021_021149c0);
  _ZN9ModelBase7SetFileEP8BMD_Fileii((char*)((char*)this)+0xd4, m, 1, -1);
  _ZN8Platform21UpdateModelPosAndRotYEv(((char*)this));
  _ZN8Platform19UpdateClsnPosAndRotEv(((char*)this));
  int k = _ZN12MeshCollider8LoadFileER13SharedFilePtr(data_ov021_021149b8);
  _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block((char*)((char*)this)+0x124, k, (char*)((char*)this)+0x2ec, 0x1000, unk_08e, (void*)data_ov022_02114558);
  return 1;
}
