extern void func_ov007_020c7240(void* a, int b, int c);
extern void func_ov007_020c7a84(void* a, int b, int c);
extern void func_ov007_020c71c0(void* a, int b, int c);

void func_ov007_020cba78(char* c, int arg1) {
    int i;
    int n;
    func_ov007_020c7240(*(void**)(c + 4), *(int*)(c + 0x10), *(int*)(c + 0x14));
    for (i = 0;
         (n = (*(int*)(*(int**)(c + 4) + 1) - 1) * (*(int*)*(int**)(c + 4) - 1)), i < n;
         i++) {
        func_ov007_020c7a84(((void**)*(char**)(c + 8))[i], *(int*)(c + 0x10), *(int*)(c + 0x14));
    }
    func_ov007_020c71c0(*(void**)(c + 4), 0, arg1);
}
