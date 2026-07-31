//cpp
// @symbol _ZN14UnchainedChompD1Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_Model.h"
#include "decl_ModelAnim.h"
#include "decl_ShadowModel.h"
#include "decl_WithMeshClsn.h"
/* recovered: named members + shared header */
#include "UnchainedChomp.h"
extern "C" void __destroy_arr(void* p, int n, int sz, void* dtor);
extern "C" void _ZN25MovingCylinderClsnWithPosD1Ev(void*);
extern "C" int func_ov002_020aed18(int* x);

extern int _ZTV14UnchainedChomp;
extern "C" void func_02011508(void);
extern "C" void func_020072c0(void);

extern "C" void* _ZN14UnchainedChompD1Ev(char* thiz)
{
    char* c = thiz;
    *(void**)c = &_ZTV14UnchainedChomp;
    __destroy_arr(c + 0x768, 6, 6, (void*)&func_02011508);
    __destroy_arr(c + 0x720, 6, 0xc, (void*)&func_020072c0);
    __destroy_arr(c + 0x6d8, 6, 0xc, (void*)&func_020072c0);
    _ZN11ShadowModelD1Ev(c + 0x640);
    __destroy_arr(c + 0x550, 6, 0x28, (void*)&_ZN11ShadowModelD1Ev);
    __destroy_arr(c + 0x370, 6, 0x50, (void*)&_ZN5ModelD1Ev);
    _ZN9ModelAnimD1Ev(c + 0x30c);
    _ZN12WithMeshClsnD1Ev(c + 0x150);
    _ZN25MovingCylinderClsnWithPosD1Ev(c + 0x110);
    func_ov002_020aed18((int*)c);
    return c;
}
