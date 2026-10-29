//cpp
#include "IRQ.h"

extern "C" {

struct Obj {
    int field0;
    int field4;
    int count;
};
extern struct Obj data_020a6134;
int func_020589d4(struct Obj *o) {
    unsigned int saved = IRQ::Disable();
    int g = (&data_020a6134)->count;
    int ret;
    if (o->field4 == 0) {
        o->field4 = g;
        *(int *)(((int)o + 8)) = *(int *)(((int)o + 8)) + 1;
        ret = 1;
    } else if (o->field4 == g) {
        *(int *)(((int)o + 8)) = *(int *)(((int)o + 8)) + 1;
        ret = 1;
    } else {
        ret = 0;
    }
    IRQ::Restore(saved);
    return ret;
}
}
