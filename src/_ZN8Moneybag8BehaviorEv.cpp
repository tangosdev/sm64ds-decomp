//cpp
// @symbol _ZN8Moneybag8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Moneybag.h"
extern "C" {
extern void _ZN8dActor_c19MakeVanishLuigiWorkER5dCc_c(char* c, char* clsn);
extern int _ZNK10dBgCh_Actr14GetResultFlag1Ev(char* clsn);
extern int _ZNK10dBgCh_Actr12TouchesWaterEv(char* clsn);
}

int Moneybag::Behavior()
{
    func_ov081_02127708(((char*)this));
    _ZN8dActor_c19MakeVanishLuigiWorkER5dCc_c(((char*)this), ((char*)this) + 0x1b0);
    if (_ZNK10dBgCh_Actr14GetResultFlag1Ev((char*)&mWithMeshClsn) != 0) {
        if (_ZNK10dBgCh_Actr12TouchesWaterEv((char*)&mWithMeshClsn) != 0) {
            func_ov081_02126700(((char*)this));
        }
    }
    func_ov081_02126a20(((char*)this));
    return 1;
}
