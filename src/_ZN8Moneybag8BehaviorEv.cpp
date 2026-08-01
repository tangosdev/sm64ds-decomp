//cpp
// @symbol _ZN8Moneybag8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Moneybag.h"
extern void _ZN5Actor19MakeVanishLuigiWorkER12CylinderClsn(char* c, char* clsn);
extern int _ZNK12WithMeshClsn14GetResultFlag1Ev(char* clsn);
extern int _ZNK12WithMeshClsn12TouchesWaterEv(char* clsn);

int Moneybag::Behavior()
{
    func_ov081_02127708(((char*)this));
    _ZN5Actor19MakeVanishLuigiWorkER12CylinderClsn(((char*)this), ((char*)this) + 0x1b0);
    if (_ZNK12WithMeshClsn14GetResultFlag1Ev((char*)&mWithMeshClsn) != 0) {
        if (_ZNK12WithMeshClsn12TouchesWaterEv((char*)&mWithMeshClsn) != 0) {
            func_ov081_02126700(((char*)this));
        }
    }
    func_ov081_02126a20(((char*)this));
    return 1;
}
