//cpp
// @symbol _ZN18BowserFireSeaArena13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "BowserFireSeaArena.h"
#include "MeshColliderBase.h"
typedef int Fix12;
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void*);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void*, void*, int, int);
extern void* _ZN12MeshCollider8LoadFileER13SharedFilePtr(void*);
extern int _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void*, void*, void*, Fix12, short, void*);
extern int func_020393d4(void*, void*);
extern int _ZN16MeshColliderBase16UpdatePosAndAngsERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_(void);


int BowserFireSeaArena::InitResources()
{
  void* mdl;
  void* kcl;
  mdl = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov060_0211affc);
  _ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this)+0x324, mdl, 1, -1);
  func_ov060_02117a64(((char*)this));
  kcl = _ZN12MeshCollider8LoadFileER13SharedFilePtr(&data_ov060_0211aff4);
  _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(((char*)this)+0x374, kcl, ((char*)this)+0x2ec, 0x1000, unk_08e, &func_021115bc);
  func_020393d4(((char*)this)+0x374, &_ZN16MeshColliderBase16UpdatePosAndAngsERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);
  ((MeshColliderBase *)(((char*)this)+0x374))->Enable((Actor *)(((char*)this)));
  unk_31e = 0;
  unk_320 = 0;
  unk_322 = 0;
  unk_56c = 0;
  return 1;
}
