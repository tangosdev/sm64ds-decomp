// @symbol _ZN11dScMiniGm_c8BehaviorEv
/* dScMiniGm_c::Behavior() -- vtable slot 6. See include/dScMiniGm_c.h.
 * Plain C, so it cannot include that header (dScene_c-derived, C++-only) --
 * a local, non-inheriting offset struct stands in instead, matching the
 * convention _ZN11dScMiniGm_c6RenderEv.c already uses. */
#include "decl_Scene.h"
#include "decl_common.h"

extern int func_02012790(int);
extern int RandomIntInternal(int* seed);
extern void _ZN5Sound22StopLoadedMusic_Layer1Ej(unsigned int);

extern unsigned char data_020a0e40;
extern unsigned short data_020a0e5a[];
extern unsigned char data_0209b300;
extern int data_0209e650;
extern unsigned char data_0209b304;

struct dScMiniGm_c_shadow {
    char pad_000[0x54];
    unsigned char unk_054;
    char pad_055[0x8c - 0x55];
    int unk_08c;
    int unk_090;
    int unk_094;
    int unk_098;
    int unk_09c;
    char pad_0a0[0xac - 0xa0];
    unsigned char unk_0ac;
};

int _ZN11dScMiniGm_c8BehaviorEv(char* c) {
    struct dScMiniGm_c_shadow *self = (struct dScMiniGm_c_shadow *)(void *)c;
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
            _ZN8dScene_c20SetAndStopColorFaderEv();
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
