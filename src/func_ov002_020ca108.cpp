//cpp
extern "C" {
extern int data_ov002_0211013c[];
extern void _ZN6Player11ChangeStateERNS_5StateE(void *self, void *state);
void func_ov002_020ca108(char *r0) {
    if (*(unsigned char*)(r0 + 0x70b)) {
        *(unsigned char*)(r0 + 0x6e3) = 0x13;
        return;
    }
    _ZN6Player11ChangeStateERNS_5StateE(r0, data_ov002_0211013c);
}
}
