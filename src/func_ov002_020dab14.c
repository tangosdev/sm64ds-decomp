extern void func_ov002_020db54c(char* p, int a, int b, int d);

int func_ov002_020dab14(char* c)
{
    char* p = *(char**)(c + 0x358);
    int ok = (int)(p != 0);
    int is_bf;
    if (ok == 0) return 0;
    is_bf = (int)(*(unsigned short*)(p + 0xc) == 0xbf);
    if (is_bf != 0) {
        func_ov002_020db54c(p, 0x10000, 0x10000, *(short*)(c + 0x8e));
    }
    *(int*)(((int)*(char**)(c + 0x358) + 0xb0) & 0xFFFFFFFFFFFFFFFF) &= ~0x4000;
    *(int*)(((int)*(char**)(c + 0x358) + 0xb0) & 0xFFFFFFFFFFFFFFFF) &= ~0x100;
    *(char**)(c + 0x358) = 0;
    return 1;
}
