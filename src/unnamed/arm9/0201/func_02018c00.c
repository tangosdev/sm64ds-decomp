extern void FS_InitFile(void *s);
extern int func_02018e00(void *s, int b, int c, int d, int e);
extern int func_02018d48(void *s, void *b, int c);
extern int FS_CloseFile(void *self);
extern int data_020a8064[2];
extern int data_020a806c[2];
extern int data_020a8074;
extern void CpuCopy8(void *dst, void *src, int n);

int func_02018cbc(char *self, int flag, int shift, int a3, int a5, int a6, int a7, int a8) {
    char local[0x48];
    int off;
    int base;
    int hi;
    if (flag == 0) {
        hi = a6;
        base = a5;
    } else {
        hi = a8;
        base = a7;
    }
    off = shift << 5;
    if ((unsigned int)off >= (unsigned int)hi)
        return 0;
    FS_InitFile(local);
    func_02018e00(local, a3, base + off, base + hi, -1);
    func_02018d48(local, self, 0x20);
    FS_CloseFile(local);
    *(int *)(self + 0x20) = flag;
    return 1;
}

int func_02018c00(int *thiz, int flag, int index) {
    int *base = (flag == 0) ? data_020a8064 : data_020a806c;
    int ptr = base[0];
    if (ptr != 0) {
        int off = index << 5;
        if ((unsigned int)off >= (unsigned int)base[1])
            return 0;
        CpuCopy8((void *)(ptr + off), thiz, 0x20);
        thiz[8] = flag;
        return 1;
    }
    {
        int *m = (int *)0x27ffe50;
        int *n = (int *)0x27ffe58;
        return func_02018cbc((char *)thiz, flag, index, (int)&data_020a8074, m[0], m[1], n[0],
                             n[1]);
    }
}
