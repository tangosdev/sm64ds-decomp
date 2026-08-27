#include "types.h"
extern int func_02071698(void *thiz, int val);
extern void func_02071644(void *thiz, int val);

struct T { char pad[4]; unsigned char field4; };

void func_020715e0(struct T *thiz, int val)
{
    int r;
    if (val <= 0) return;
    if (val >= thiz->field4) return;
    r = func_02071698(thiz, val);
    thiz->field4 = val;
    if (r < 0) return;
    func_02071644(thiz, val);
}
