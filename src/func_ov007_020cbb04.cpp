//cpp
extern "C" void func_ov007_020c8098(void* p);
extern "C" void func_ov007_020c7368(void* s);
extern "C" void func_ov007_020c3d1c(void* self);

extern "C" void func_ov007_020cbb04(char* c)
{
    func_ov007_020c3d1c(*(void**)(c));
    func_ov007_020c3d1c(*(void**)(c + 0x28));
    func_ov007_020c3d1c(*(void**)(c + 0x2c));
    func_ov007_020c3d1c(*(void**)(c + 0x30));
    func_ov007_020c3d1c(*(void**)(c + 0x34));

    int i = 0;
    int* a = *(int**)(c + 4);
    if ((a[1] - 1) * (a[0] - 1) > 0) {
        do {
            func_ov007_020c8098(((void**)*(void**)(c + 8))[i]);
            i++;
            a = *(int**)(c + 4);
        } while (i < (a[1] - 1) * (a[0] - 1));
    }
    func_ov007_020c3d1c(*(void**)(c + 8));
    func_ov007_020c7368(*(void**)(c + 4));
    func_ov007_020c3d1c((void*)c);
}
