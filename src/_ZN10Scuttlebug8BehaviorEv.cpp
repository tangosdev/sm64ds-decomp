//cpp
// @symbol _ZN10Scuttlebug8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Scuttlebug.h"
extern "C" {
extern void DecIfAbove0_Short(void* p);
extern void _ZN5Actor19MakeVanishLuigiWorkER12CylinderClsn(char* self, void* clsn);
extern int _ZNK12WithMeshClsn14GetResultFlag1Ev(void* self);
extern int _ZNK12WithMeshClsn12TouchesWaterEv(void* self);
extern void func_ov071_0211f524(char* c);
}

int Scuttlebug::Behavior()
{
  DecIfAbove0_Short((char*)&unk_3a8);
  func_ov071_02120278(((char*)this));
  _ZN5Actor19MakeVanishLuigiWorkER12CylinderClsn(((char*)this), ((char*)this)+0x160);
  if(_ZNK12WithMeshClsn14GetResultFlag1Ev((char*)&mWithMeshClsn) && _ZNK12WithMeshClsn12TouchesWaterEv((char*)&mWithMeshClsn)){
    func_ov071_0211f498(((char*)this));
  }
  func_ov071_0211f524(((char*)this));
  return 1;
}
