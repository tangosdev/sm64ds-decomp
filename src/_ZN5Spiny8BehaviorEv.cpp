//cpp
// @symbol _ZN5Spiny8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "Spiny.h"
extern "C" {
int _ZNK10dBgCh_Actr10IsOnGroundEv(void* c);
int _ZN8dActor_c22IsTooFarAwayFromPlayerE5Fix12IiE(void* c, int d);
unsigned char DecIfAbove0_Byte(unsigned char* p);
void _ZN7fBase_c18MarkForDestructionEv(void* c);
void func_ov077_02124c28(void* c);
void func_ov077_02125e20(void* c);
void _ZN8dActor_c19MakeVanishLuigiWorkER5dCc_c(void* c, void* cyl);
void func_ov077_02125304(char* c);
void _ZN8dActor_c8PoofDustEv(void* c);
void func_02012694(int a, void* p);

extern signed char data_0209f2f8;
}

int Spiny::Behavior()
{
    int s = mState;
    if (s != 1 || _ZNK10dBgCh_Actr10IsOnGroundEv((char*)&mWithMeshClsn)) {
        s = mState;
        if (s != 4 && s != 5 && _ZN8dActor_c22IsTooFarAwayFromPlayerE5Fix12IiE(((char*)this), 0x5dc000)) {
            if (DecIfAbove0_Byte((unsigned char*)((char*)&mDespawnTimer)) == 0) {
                _ZN7fBase_c18MarkForDestructionEv(((char*)this));
                return 1;
            }
            goto done;
        }
    }
    func_ov077_02124c28(((char*)this));
    func_ov077_02125e20(((char*)this));
    _ZN8dActor_c19MakeVanishLuigiWorkER5dCc_c(((char*)this), ((char*)this) + 0x1b0);
    func_ov077_02125304(((char*)this));
    if (data_0209f2f8 == 0x1c && mPosY <= -0x1600000) {
        _ZN8dActor_c8PoofDustEv(((char*)this));
        func_02012694(0xc4, ((char*)this) + 0x74);
        _ZN7fBase_c18MarkForDestructionEv(((char*)this));
    }
done:
    return 1;
}
