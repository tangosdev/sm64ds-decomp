//cpp
// @symbol func_ov002_020efc74
/* recovered: shared common types */
#include "common.h"

struct Obj {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5(Vector3* p);
};
extern "C" int func_ov002_020efe9c(int* self);
extern Vector3 data_ov002_0210af00;
extern "C" void func_ov002_020efc74(void* self) {
    char* p = (char*)self;
    if (func_ov002_020efe9c((int*)self) == 0) return;
    if (*(int*)(p + 0x44c) == 2) return;
    int i = 0;
    Obj* obj = (Obj*)(p + 0x320);
    do {
        Vector3 local = data_ov002_0210af00;
        obj->v5(&local);
        i++;
        obj = (Obj*)((char*)obj + 0x50);
    } while (i < 3);
}
