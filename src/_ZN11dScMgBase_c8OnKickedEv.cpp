//cpp
// @symbol _ZN11dScMgBase_c8OnKickedEv
#include "dScMgBase_c.h"
// recovered name: dScMgBase_c_OnKicked
/* recovered: renamed to Class_Method */
/* dScMgBase_c::OnKicked - recovered from vtable slot identity */
struct Obj {
    virtual void v00(); virtual void v04(); virtual void v08(); virtual void v0c();
    virtual void v10(); virtual void v14(); virtual void v18(); virtual void v1c();
    virtual void v20(); virtual void v24(); virtual void v28(); virtual void v2c();
    virtual void v30(); virtual void v34(); virtual void v38(); virtual void v3c();
    virtual void v40(); virtual void v44(); virtual void v48(); virtual void v4c();
    virtual void v50(); virtual void v54(); virtual void v58(); virtual void v5c();
    virtual void v60(); virtual void v64(); virtual void v68(); virtual void v6c();
    virtual void v70();
    virtual void method74();
    virtual void method78();
};

int dScMgBase_c::OnKicked()
{
    Obj *self = (Obj *)this;

    void* base = (char*)self + 0x4000;
    int v2 = *(int*)((char*)base + 0x628);
    int v1 = *(int*)((char*)base + 0x62c);
    if (v1 == v2) goto done;
    if (v2 == 0) {
        self->method78();
    } else {
        self->method74();
    }
    *(int*)((char*)self + 0x462c) = *(int*)((char*)self + 0x4628);
done:
    return 1;
}
