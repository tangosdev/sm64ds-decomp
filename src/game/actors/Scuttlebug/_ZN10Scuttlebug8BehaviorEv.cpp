//cpp
// @symbol _ZN10Scuttlebug8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Scuttlebug.h"
extern "C" {
extern void DecIfAbove0_Short(void* p);
extern void _ZN8dActor_c19MakeVanishLuigiWorkER5dCc_c(char* self, void* clsn);
extern int _ZNK10dBgCh_Actr14GetResultFlag1Ev(void* self);
extern int _ZNK10dBgCh_Actr12TouchesWaterEv(void* self);
extern void func_ov071_0211f524(char* c);
}

int Scuttlebug::Behavior()
{
  DecIfAbove0_Short((char*)&mTimer);
  func_ov071_02120278(((char*)this));
  _ZN8dActor_c19MakeVanishLuigiWorkER5dCc_c(((char*)this), ((char*)this)+0x160);
  if(_ZNK10dBgCh_Actr14GetResultFlag1Ev((char*)&mWithMeshClsn) && _ZNK10dBgCh_Actr12TouchesWaterEv((char*)&mWithMeshClsn)){
    func_ov071_0211f498(((char*)this));
  }
  func_ov071_0211f524(((char*)this));
  return 1;
}
