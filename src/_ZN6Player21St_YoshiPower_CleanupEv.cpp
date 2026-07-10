//cpp
extern "C" {
extern void func_ov002_020d718c(char*);
extern void func_ov002_020d6790(char*);
extern void func_ov002_020d71a0(char*);

int _ZN6Player21St_YoshiPower_CleanupEv(char* c)
{
    char* r2 = *(char**)(c+0x360);
    if (r2 != 0) {
        int t = (int)((*(int*)(r2+0xb0) & 0x20000) != 0);
        if (t != 0) {
            *(int*)(((int)r2 + 0xb0) & 0xFFFFFFFFFFFFFFFFLL) &= ~0xe0000;
            func_ov002_020d718c(c);
            return 1;
        }
        *(int*)(((int)r2 + 0xb0) & 0xFFFFFFFFFFFFFFFFLL) &= ~0x20000;
    }
    if (*(unsigned char*)(c+0x714) == 0) {
        char* q = *(char**)(c+0x360);
        if (q != 0) {
            int eq = (int)(*(unsigned short*)(q+0xc) == 0xbf);
            if (eq == 0) {
                func_ov002_020d6790(c);
                goto end;
            }
        }
        func_ov002_020d718c(c);
    } else {
        if (*(char**)(c+0x360) == 0 && *(unsigned char*)(c+0x6f4) == 0)
            func_ov002_020d71a0(c);
    }
end:
    return 1;
}
}
