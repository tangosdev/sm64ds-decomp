// @symbol func_ov005_020c14a0
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_Scene.h"
#include "decl_common.h"
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "dScMiniGm_c.h"
// recovered name: dScMiniGm_c_Behavior
/* recovered: renamed to Class_Method */
/* dScMiniGm_c::Behavior - recovered from vtable slot identity */

extern int func_02012790(int);
extern int RandomIntInternal(int* seed);
extern void _ZN5Sound22StopLoadedMusic_Layer1Ej(unsigned int);

extern unsigned char data_020a0e40;
extern unsigned short data_020a0e5a[];
extern unsigned char data_0209b300;
extern int data_0209e650;
extern unsigned char data_0209b304;

int func_ov005_020c14a0(char* c) {
    struct dScMiniGm_c *self = (struct dScMiniGm_c *)(void *)c;
    if ((data_020a0e5a[data_020a0e40 << 1] & 0xfff) != 0) {
        func_02012790(0xe);
    }
    if (data_0209b300 == 1) {
        func_ov005_020bff4c(c);
        return 1;
    }
    RandomIntInternal(&data_0209d4b8);
    RandomIntInternal(&data_0209e650);

    if (self->unk_090 > 0) {
        *(int*)((int)c + 0x90) -= 1;
        if (self->unk_090 == 0) {
            data_0209b304 = 0;
            self->unk_054 = 1;
        }
    } else if (self->unk_094 > 0) {
        *(int*)((int)c + 0x94) -= 1;
        if (self->unk_094 == 0) {
            data_0209b304 = 1;
            self->unk_054 = 1;
        }
    } else if (self->unk_098 > 1) {
        *(int*)((int)c + 0x98) -= 1;
        if (self->unk_098 == 1) {
            _ZN5Scene20SetAndStopColorFaderEv();
            ExitMinigameMenu();
            _ZN5Sound22StopLoadedMusic_Layer1Ej(0x1e);
            self->unk_0ac = 1;
            return 1;
        }
    } else {
        *(int*)((int)c + 0x8c) += 1;
        if (self->unk_08c >= 0x40) self->unk_08c = 0;
        *(int*)((int)c + 0x9c) += 1;
        if (self->unk_09c >= 0x40) self->unk_09c = 0;
    }
    func_ov005_020c0878(c);
    func_ov005_020c06cc(c);
    func_ov005_020c0378(c);
    func_ov005_020c0250(c);
    func_ov005_020c0140(c);
    return 1;
}
