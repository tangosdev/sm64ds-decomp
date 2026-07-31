//cpp
// @symbol func_ov006_020ee8dc
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "dScMgJump_c.h"
// @emits dScMgJump_c_OnTurnIntoEgg
/* recovered: renamed to Class_Method */
/* dScMgJump_c::OnTurnIntoEgg - recovered from vtable slot identity */
extern "C" short _Z15ApproachLinear2Rsss(short &r, short b, short c);
extern "C" void func_02012790(int a0);

extern "C" int dScMgJump_c_OnTurnIntoEgg(char *thiz, int sel)
{
    struct dScMgJump_c *self = (struct dScMgJump_c *)(void *)thiz;
    if (sel == 0) {
        if (self->unk_0bc % 5 == 4) {
            if (_Z15ApproachLinear2Rsss(self->unk_5014, 0, 1) != 0) {
                if ((&data_ov006_02140428)[0] < 3) {
                    func_ov006_020c72dc();
                    func_02012790(0x26);
                    *(short*)(thiz + 0x5000 + 0x14) = 0x3c;
                    return 0;
                }
            } else {
                return 0;
            }
        }
    }
    func_ov006_020c8a9c(0, 0);
    return 1;
}
