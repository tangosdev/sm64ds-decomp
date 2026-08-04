//cpp
#include "types.h"
// @symbol _ZN9RabbitKey6RenderEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "RabbitKey.h"
/* _ZN9RabbitKey6RenderEv — if the pointer at this+0x188 equals &data_ov085_0213072c,
 * bump the angle at this+0x8e by 0x500; run func_ov085_0212d2b8(this); then
 * virtual-call slot +0x14 on the sub-object at this+0x110 with arg 0. Returns 1.
 */
struct SubVt {
    char _pad0[0x14];
    void (*fn14)(void* self, int a); /* 0x14 */
};

struct Sub {
    struct SubVt* vt;     /* 0x0 */
};

struct Obj {
    char _pad0[0x8e];
    s16 angle;            /* 0x8e */
    char _pad1[0x110 - 0x90];
    struct Sub sub;       /* 0x110 */
    char _pad2[0x188 - 0x114];
    void* unk188;         /* 0x188 */
};

extern "C" {
extern void func_ov085_0212d2b8(struct Obj* self);
}

int RabbitKey::Render()
{
    if (((struct Obj*)this)->unk188 == (void*)&data_ov085_0213072c) {
        *(s16*)(((long long)(int)((char*)&unk_08e))) += 0x500;
    }
    func_ov085_0212d2b8(((struct Obj*)this));
    {
        struct Sub* s = (struct Sub*)(((long long)(int)((char*)&mModel)));
        s->vt->fn14(s, 0);
    }
    return 1;
}
