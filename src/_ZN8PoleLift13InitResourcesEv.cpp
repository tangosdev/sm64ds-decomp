//cpp
// @symbol _ZN8PoleLift13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "PoleLift.h"
#include "MeshColliderBase.h"
typedef int Fix12;
extern "C" {
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void*);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void*, void*, int, int);
extern void* _ZN12MeshCollider8LoadFileER13SharedFilePtr(void*);
extern int _ZN21ExtendingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void*, void*, void*, Fix12, short, void*);
}

int PoleLift::InitResources()
{
  void* mdl;
  void* kcl;
  mdl = _ZN5Model8LoadFileER13SharedFilePtr(data_ov045_021131d8);
  _ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this)+0xd8, mdl, 1, -1);
  func_ov045_021118f8(((char*)this));
  func_ov045_021118c4(((char*)this));
  kcl = _ZN12MeshCollider8LoadFileER13SharedFilePtr(data_ov045_021131d0);
  _ZN21ExtendingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(((char*)this)+0x158, kcl, ((char*)this)+0x128, 0x1000, unk_08e, data_ov045_021125b0);
  func_020396c0(((char*)this)+0x158, 4);
  unk_1a5 = 1;
  ((MeshColliderBase *)(((char*)this)+0x158))->Enable((Actor *)(((char*)this)));
  unk_0d4 = 1;
  return 1;
}
