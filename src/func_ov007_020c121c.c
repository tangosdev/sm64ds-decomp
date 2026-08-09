extern void func_ov007_020c13f4(char* p);
extern void func_ov007_020c3d1c(void* p);
extern int func_ov007_020c1384(char* c);

void func_ov007_020c121c(char* c) {
    int i;
    int j;
    for (i = 0; i < *(int*)c; i++) {
        func_ov007_020c13f4(*(char**)(c + 4) + i * 8);
    }
    func_ov007_020c3d1c(*(void**)(c + 4));
    for (j = 0; j < *(int*)(c + 8); j++) {
        func_ov007_020c1384(*(char**)(c + 0xc) + j * 0x10);
    }
    func_ov007_020c3d1c(*(void**)(c + 0xc));
    func_ov007_020c3d1c(c);
}
