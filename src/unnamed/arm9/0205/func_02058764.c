extern int func_02058690(void);
extern int func_020587dc(void);
extern int func_0205865c(void);
extern int data_0209a040;

int func_02058764(void) {
    if (data_0209a040 == -1) {
        int r4 = func_02058690();
        int flags;
        if (func_020587dc()) flags = r4 | 0x10000000;
        else if (func_0205865c()) flags = r4 | 0x40000000;
        else if (r4 & 0x1000000) flags = r4 | 0x20000000;
        else flags = r4 | 0x80000000;
        data_0209a040 = flags | *(unsigned short *)0x27ffffa;
    }
    return data_0209a040;
}
