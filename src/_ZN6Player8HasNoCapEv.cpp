//cpp
// @symbol _ZN6Player8HasNoCapEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern signed char data_0209f2f8;
extern unsigned char data_0209f2d8;
int _ZN6Player7IsStateERNS_5StateE(void* c, void* s);
int _ZN8SaveData16HasPlayerLostCapEv(void);
}

int Player::HasNoCap()
{
    if(data_0209f2f8 == 0x1f) return 0;
    int r = (data_0209f2d8 == 2);
    if(!r){
        if(!_ZN6Player7IsStateERNS_5StateE(((char*)this), &data_ov002_0211058c)){
            r = (data_0209f2d8 == 1);
            if(!r){
                if(_ZN8SaveData16HasPlayerLostCapEv()) return 1;
            }
        }
    }
    if(param1 == 3) return 0;
    r = mHasNoCap != 0;
    return r;
}
