//cpp
// @symbol _ZN15dScMgRoulette_cD0Ev
/* Real out-of-line definition, identical body to
   _ZN15dScMgRoulette_cD1Ev.cpp -- see that file's note. */
#include "dScMgRoulette_c.h"
dScMgRoulette_c::~dScMgRoulette_c()
{
    _ZN5ModelD1Ev(mModel2);
    _ZN5ModelD1Ev(mModel1);
    __destroy_arr(mArray, 5, 0x34, (void *)func_ov006_021079c8);
    func_ov006_020c1c64((char *)mTable);
}
