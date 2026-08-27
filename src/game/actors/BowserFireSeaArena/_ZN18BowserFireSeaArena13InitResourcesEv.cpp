//cpp
// @symbol _ZN18BowserFireSeaArena13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method
 *
 * The three `((char*)this)+0xNNN` offsets are named members now: 0x324 is mModel2,
 * 0x374 is mMovingMeshCollider2, and 0x2ec is mClsnMat -- inherited from dBgActor_c,
 * which is also where the collider's transform comes from. unk_08e is dActor_c::mAngleY.
 */
#include "BowserFireSeaArena.h"
#include "dBgW.h"
/* Every _ZN... prototype below is already a mangled ROM name; without extern "C" a
   .cpp file re-mangles it (e.g. _ZN5Model8LoadFileER13SharedFilePtr ->
   _Z35_ZN5Model8LoadFileER13SharedFilePtrPv), which is a phantom no module defines.
   func_020393d4 is a plain C symbol and needs extern "C" for the same reason. */
extern "C" void* _ZN5Model8LoadFileER13SharedFilePtr(void*);
extern "C" int _ZN9ModelBase7SetFileEP8BMD_Fileii(void*, void*, int, int);
extern "C" void* _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void*);
extern "C" int _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void*, void*, void*, int, short, void*);
/* `int`, not a local `typedef int Fix12;`. That typedef used to sit above and now
   collides with the real Fix12<> template, which BowserFireSeaArena.h reaches
   through dActor_c.h. The parameter is passed in a register either way. */
extern "C" int func_020393d4(void*, void*);
extern "C" int _ZN4dBgW16UpdatePosAndAngsERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_(void);


int BowserFireSeaArena::InitResources()
{
  void* mdl;
  void* kcl;
  mdl = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov060_0211affc);
  _ZN9ModelBase7SetFileEP8BMD_Fileii(&mModel2, mdl, 1, -1);
  func_ov060_02117a64(((char*)this));
  kcl = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(&data_ov060_0211aff4);
  _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(&mMovingMeshCollider2, kcl, &mClsnMat, 0x1000, mAngleY, &func_021115bc);
  func_020393d4(&mMovingMeshCollider2, &_ZN4dBgW16UpdatePosAndAngsERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
  ((dBgW *)&mMovingMeshCollider2)->Enable((dActor_c *)(((char*)this)));
  mAngleXSpeed = 0;
  mAngleYSpeed = 0;
  mAngleZSpeed = 0;
  unk_56c = 0;
  return 1;
}
