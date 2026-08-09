extern char* data_ov007_02104bc0;
extern void func_ov007_020c3d1c(void* p);
extern void func_ov007_020c3e7c(void* p);

void func_ov007_020be47c(void) {
    char* base = (char*)data_ov007_02104bc0;
    if (base == 0) return;
    if (*(void**)(base + 4) != 0) {
        func_ov007_020c3d1c(*(void**)(base + 4));
        *(int*)((char*)data_ov007_02104bc0 + 4) = 0;
    }
    if (*(void**)data_ov007_02104bc0 != 0) {
        func_ov007_020c3e7c(*(void**)data_ov007_02104bc0);
        *(int*)data_ov007_02104bc0 = 0;
    }
    func_ov007_020c3d1c((char*)data_ov007_02104bc0);
    data_ov007_02104bc0 = 0;
}
