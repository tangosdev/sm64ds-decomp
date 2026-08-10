//cpp
// @symbol _ZN18BowserFireSeaArena13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method
 *
 * The three `((char*)this)+0xNNN` offsets are named members now: 0x324 is mModel2,
 * 0x374 is mMovingMeshCollider2, and 0x2ec is mClsnMat -- inherited from Platform,
 * which is also where the collider's transform comes from. unk_08e is Actor::mAngleY.
 */
#include "BowserFireSeaArena.h"
#include "MeshColliderBase.h"
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void*);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void*, void*, int, int);
extern void* _ZN12MeshCollider8LoadFileER13SharedFilePtr(void*);
extern int _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void*, void*, void*, int, short, void*);
/* `int`, not a local `typedef int Fix12;`. That typedef used to sit above and now
   collides with the real Fix12<> template, which BowserFireSeaArena.h reaches
   through Actor.h. The parameter is passed in a register either way. */
extern int func_020393d4(void*, void*);
extern int _ZN16MeshColliderBase16UpdatePosAndAngsERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_(void);


int BowserFireSeaArena::InitResources()
{
  void* mdl;
  void* kcl;
  mdl = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov060_0211affc);
  _ZN9ModelBase7SetFileEP8BMD_Fileii(&mModel2, mdl, 1, -1);
  func_ov060_02117a64(((char*)this));
  kcl = _ZN12MeshCollider8LoadFileER13SharedFilePtr(&data_ov060_0211aff4);
  _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(&mMovingMeshCollider2, kcl, &mClsnMat, 0x1000, mAngleY, &func_021115bc);
  func_020393d4(&mMovingMeshCollider2, &_ZN16MeshColliderBase16UpdatePosAndAngsERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);
  ((MeshColliderBase *)&mMovingMeshCollider2)->Enable((Actor *)(((char*)this)));
  unk_31e = 0;
  unk_320 = 0;
  unk_322 = 0;
  unk_56c = 0;
  return 1;
}
