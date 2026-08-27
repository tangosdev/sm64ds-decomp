extern void _ZN6Player16St_Shell_CleanupEv(void *this);
extern void _ZN6Player11ChangeStateERNS_5StateE(void *this, void *state);
extern char data_ov002_0211013c[];
void func_ov002_020cc1f4(char *c) {
    if (*(int*)(c + 0x354) == 0) return;
    _ZN6Player16St_Shell_CleanupEv(c);
    _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_0211013c);
}
