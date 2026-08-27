extern int func_ov004_020ae1cc(void);
extern void _ZN5Sound7PlaySubEjjj5Fix12IiEb(unsigned int, unsigned int, unsigned int, int, int);

void func_ov004_020ae274(void* c) {
    extern int data_ov004_020beb68[];
    if (*data_ov004_020beb68 == 0) return;
    func_ov004_020ae1cc();
    _ZN5Sound7PlaySubEjjj5Fix12IiEb((unsigned int)c, 0x40, 0x7f, 0x7f000, 1);
}
