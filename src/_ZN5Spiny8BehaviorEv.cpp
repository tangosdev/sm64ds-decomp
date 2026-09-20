//cpp
// @symbol _ZN5Spiny8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "Spiny.h"
extern "C" {
int _ZN8dActor_c22IsTooFarAwayFromPlayerE5Fix12IiE(void* c, int d);
unsigned char DecIfAbove0_Byte(unsigned char* p);
int func_ov077_02124c28(char* c);
void func_ov077_02125e20(void* c);
void func_ov077_02125304(char* c);
void func_02012694(unsigned int soundID, const Vector3* cameraPosition);

extern signed char data_0209f2f8;
}

int Spiny::Behavior()
{
    int s = mState;
    if (s != 1 || mWithMeshClsn.IsOnGround()) {
        s = mState;
        if (s != 4 && s != 5 && _ZN8dActor_c22IsTooFarAwayFromPlayerE5Fix12IiE(((char*)this), 0x5dc000)) {
            if (DecIfAbove0_Byte(&mDespawnTimer) == 0) {
                MarkForDestruction();
                return 1;
            }
            goto done;
        }
    }
    func_ov077_02124c28(((char*)this));
    func_ov077_02125e20(((char*)this));
    MakeVanishLuigiWork(mdCcAc_c);
    func_ov077_02125304(((char*)this));
    if (data_0209f2f8 == 0x1c && mPosY <= -0x1600000) {
        PoofDust();
        func_02012694(0xc4, (const Vector3*)&mCamSpacePosX);
        MarkForDestruction();
    }
done:
    return 1;
}
