//cpp
extern "C" {
extern int data_ov002_0211019c[];
extern void _ZN6Player11ChangeStateERNS_5StateE(void *self, void *state);
int func_ov002_020e2ba8(char *r0) {
    if (*(unsigned char*)(r0 + 0x703) == 0)
        return 0;
    _ZN6Player11ChangeStateERNS_5StateE(r0, data_ov002_0211019c);
    return 1;
}
}
