//cpp
// @symbol _ZN14dScMgMCarlo2_c6RenderEv
#include "decl_common.h"
#include "dScMgMCarlo2_c.h"
/* dScMgMCarlo2_c::Render -- vtable slot 9.
 *
 * Attributed by tools/rtti_vtables.py --own dScMgMCarlo2_c, this class's own
 * slot 9 (dScMgBase_c::Render). The old file's `recovered name:
 * dScMgMCarlo2_c_Render` agreed. */
struct Node {
    virtual void m0();
    Node *next;
    char pad[0x18];
    int f20;
};

extern "C" void func_ov006_020c0aa8(void *c);
extern "C" int  RenderOamMainScreen(int a0, int a1, int a2, int a3, int a4);
extern "C" void func_ov006_020c1804(void *c);

extern Node *data_ov006_02142578;

s32 dScMgMCarlo2_c::Render()
{
    char *c = (char *)this;
    short v;
    Node *n6;
    int i5;
    int i6;
    Node *n5;

    func_ov006_020c0aa8(c + 0x4660);

    v = data_ov006_02142564;
    if (v > 0x270f)
        v = 0x270f;
    func_ov004_020b1ea4(0xe8, 0x28, v, 1, -1, 0, 0);

    RenderOamMainScreen(data_ov006_02133f18, 0xe8, 0x18, -1, -1);

    if (this->unk_5928 == 5)
        func_ov004_020b0d8c(c, 0xe0, 0xa0);

    n6 = data_ov006_02142578;
    for (i5 = 0; i5 < 0x14; i5++) {
        if (n6 == 0)
            break;
        if (n6->f20 > 0)
            n6->m0();
        n6 = n6->next;
    }

    n5 = data_ov006_02142578;
    for (i6 = 0; i6 < 0x14; i6++) {
        if (n5 == 0)
            break;
        if (n5->f20 == 0)
            n5->m0();
        n5 = n5->next;
    }

    func_ov006_020c1804(c + 0x4f38);
    return 1;
}
