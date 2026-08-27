
extern void func_02037de8(int* dst, int* src);
void func_02037fd4(int* dst, short h, int* src) {
    *(short*)((char*)dst + 0x18) = h;
    func_02037de8(dst + 1, src);
}