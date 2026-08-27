//cpp
extern "C" void func_ov002_020db54c(void* s, int a, int b, int c);

extern "C" int func_ov002_020daa74(char* self){
    char* s = *(char**)(self + 0x358);
    int b = (s != 0);
    if (!b)
        return 0;
    *(int*)(((int)s + 0xb0)) &= ~0x4000;
    *(int*)(((int)(*(char**)(self + 0x358)) + 0xb0)) |= 0x400;
    *(int*)(((int)(*(char**)(self + 0x358)) + 0xb0)) &= ~0x100;
    {
        char* s2 = *(char**)(self + 0x358);
        int e = (*(unsigned short*)(s2 + 0xc) == 0xbf);
        if (e) {
            int c_val = *(short*)(self + 0x8e);
            func_ov002_020db54c(s2, 0x30000, 0x10000, c_val);
        }
    }
    *(char**)(self + 0x358) = 0;
    return 1;
}
