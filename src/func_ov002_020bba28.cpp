//cpp
#include "dBgW.h"
extern "C" void _Z14ApproachLinearRsss(short* cur, short to, short step);
extern "C" void _ZN8dActor_c9UpdatePosEP5dCc_c(void* self, void* c);
extern "C" void dBgCh_Actr_UpdateContinuous_Veneer(void* p);
extern "C" int _ZNK10dBgCh_Actr10IsOnGroundEv(void* p);
extern "C" int _ZNK10dBgCh_Actr8IsOnWallEv(void* p);
extern "C" int _ZNK10dBgCh_Actr12TouchesWaterEv(void* p);
extern "C" void func_ov002_020bafc0(void* self);

struct Obj {
    virtual void v00(); virtual void v01(); virtual void v02(); virtual void v03();
    virtual void v04(); virtual void v05(); virtual void v06(); virtual void v07();
    virtual void v08(); virtual void v09(); virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
    virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23();
    virtual void v24(); virtual void v25(); virtual void v26(); virtual void v27();
    virtual void v28(); virtual void v29(); virtual void v30();
    virtual void m();   /* slot 31 = 0x7c */
};

extern "C" void func_ov002_020bba28(char* self){
    _Z14ApproachLinearRsss((short*)(self + 0x8c), 0x4000, 0x1000);
    _ZN8dActor_c9UpdatePosEP5dCc_c(self, 0);
    dBgCh_Actr_UpdateContinuous_Veneer(self + 0x3c8);
    if (_ZNK10dBgCh_Actr10IsOnGroundEv(self + 0x3c8)
        || _ZNK10dBgCh_Actr8IsOnWallEv(self + 0x3c8)
        || _ZNK10dBgCh_Actr12TouchesWaterEv(self + 0x3c8)) {
        ((struct Obj*)self)->m();
    } else {
        func_ov002_020bafc0(self);
        if (((dBgW *)(self + 0x124))->IsEnabled())
            ((dBgW *)(self + 0x124))->Disable();
    }
}
