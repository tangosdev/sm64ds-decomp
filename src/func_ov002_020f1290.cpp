//cpp
// @symbol func_ov002_020f1290
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "daObjBC_Switch_c.h"
// recovered name: daObjBC_Switch_c_Render
/* recovered: renamed to Class_Method */
/* daObjBC_Switch_c::Render - recovered from vtable slot identity */
struct Base {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void m(int);
};

extern "C" int func_ov002_020f1290(char *o) {
    struct daObjBC_Switch_c *self = (struct daObjBC_Switch_c *)(void *)o;
    if (self->unk_060 > self->unk_320) {
        Base *bp = (Base *)(o + 0xd4);
        bp->m(0);
    }
    return 1;
}
