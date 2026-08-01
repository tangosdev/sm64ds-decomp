//cpp
// @symbol _ZN6Cannon8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "Cannon.h"
extern "C" {
int func_ov098_0213a984(void* c);
void _ZN12CylinderClsn5ClearEv(void* self);
void _ZN12CylinderClsn6UpdateEv(void* self);
}

struct C;
typedef void (C::*PMF)();
struct Entry { PMF pmf; };
extern Entry data_ov098_0213c8fc[];

struct C {
    char pad[0x180];
    unsigned int idx;   // 0x180
    unsigned char flag; // 0x184
};

int Cannon::Behavior()
{
    if (((C*)this)->flag != 1) {
        (((C*)this)->*data_ov098_0213c8fc[((C*)this)->idx].pmf)();
    }
    func_ov098_0213a984(((C*)this));
    _ZN12CylinderClsn5ClearEv((char*)&mMovingCylinderClsn);
    _ZN12CylinderClsn6UpdateEv((char*)&mMovingCylinderClsn);
    return 1;
}
