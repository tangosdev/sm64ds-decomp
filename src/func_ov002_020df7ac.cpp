//cpp
extern "C" {
extern int data_ov002_02110244[];
extern int data_ov002_021101b4[];
extern int _ZN6Player7IsStateERNS_5StateE(void *self, void *state);
extern void _ZN6Player14St_Owl_CleanupEv(void *self);
extern void _ZN6Player11ChangeStateERNS_5StateE(void *self, void *state);
int func_ov002_020df7ac(char *r0) {
    char *r4 = r0;
    if (_ZN6Player7IsStateERNS_5StateE(r0, data_ov002_02110244) == 0)
        return 0;
    _ZN6Player14St_Owl_CleanupEv(r4);
    _ZN6Player11ChangeStateERNS_5StateE(r4, data_ov002_021101b4);
    return 1;
}
}
