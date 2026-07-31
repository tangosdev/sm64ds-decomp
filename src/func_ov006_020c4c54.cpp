//cpp
// @symbol func_ov006_020c4c54
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"

extern "C" void Vec3_MulScalar(Vector3 *res, Vector3 *v, int scalar);
struct Sub {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void m5(Vector3 *p);
};
extern "C" void func_ov006_020c4c54(int this_) {
    Vector3 v;
    int *p = (int*)(((int)this_ + 0x30));
    Vector3 *d = &data_ov006_0213af98;
    if (p[0] == d->x) {
        if (p[1] == d->y)
            return;
        if (((volatile int*)this_)[0xc] == 0)
            return;
    }
    Vec3_MulScalar(&v, (Vector3*)(this_ + 0xcc), *(int*)(this_ + 0xd8));
    Sub *s = (Sub*)(this_ + 0x38);
    s->m5(&v);
}
