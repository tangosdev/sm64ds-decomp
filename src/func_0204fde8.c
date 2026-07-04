extern void func_0205abb8(int a, int b, int c, int d);
extern void* func_0205afb4(void);
extern void func_0205b070(int a);
extern void func_0205afe0(void* p);
extern void func_0204fda4(char* obj);
#pragma optimize_for_size on
void func_0204fde8(char* obj) {
    if ((int)(*(int*)(obj + 0xc) << 0x1e) >> 0x1f) {
        void* p;
        int* flags = (int*)(((int)obj + 0xc) & 0xFFFFFFFFFFFFFFFF);
        func_0205abb8(*(int*)(obj + 0x2c), 0, 1 << *(int*)(obj + 0x28), 0);
        *flags &= ~2;
        p = func_0205afb4();
        func_0205b070(1);
        func_0205afe0(p);
    }
    func_0204fda4(obj);
}
