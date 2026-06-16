typedef unsigned char u8;
extern void _ZN6Player11OpenBigDoorEv(void *player);
extern void func_ov100_02144950(void *a, void *b, int c);
void func_ov100_021446f8(void *r0, void *r1) {
    void *r4 = r1;
    void *r5 = r0;
    _ZN6Player11OpenBigDoorEv(r4);
    *(u8*)((char*)r5 + 0x145) = 0;
    func_ov100_02144950(r5, r4, 0);
}
