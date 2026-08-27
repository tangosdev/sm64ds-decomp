//cpp
// @symbol func_ov006_020c35a8
/* recovered: shared common types */
#include "common.h"
extern "C" {
extern void func_ov006_020c3528(char* c);
}

struct Obj {
    virtual void m0();
    virtual void m1();
    virtual void m2();
    virtual void m3();
    virtual void m4();
    virtual void m5(Vector3* v);
};
extern "C" void func_ov006_020c35a8(char* c) {
    func_ov006_020c3528(c);
    Vector3 v; v.z=0x1000; v.y=0x1000; v.x=0x1000;
    ((Obj*)(c + 0x44))->m5(&v);
}
