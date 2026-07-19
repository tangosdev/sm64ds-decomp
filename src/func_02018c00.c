extern int data_020a8064[2];
extern int data_020a806c[2];
extern int data_020a8074;
extern void func_0205a61c(void *dst, void *src, int n);
extern int func_02018cbc(char *self, int flag, int shift, int a3, int a5, int a6, int a7, int a8);

int func_02018c00(int *thiz, int flag, int index)
{
    int *base = (flag == 0) ? data_020a8064 : data_020a806c;
    int ptr = base[0];
    if (ptr != 0) {
        int off = index << 5;
        if ((unsigned int)off >= (unsigned int)base[1]) return 0;
        func_0205a61c((void*)(ptr + off), thiz, 0x20);
        thiz[8] = flag;
        return 1;
    }
    {
        int *m = (int*)0x27ffe50;
        int *n = (int*)0x27ffe58;
        return func_02018cbc((char*)thiz, flag, index, (int)&data_020a8074, m[0], m[1], n[0], n[1]);
    }
}
