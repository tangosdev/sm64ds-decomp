struct Entry { int f0, f4; };
extern struct Entry data_02142840[];
extern void func_020b0a54(int a);
extern void func_02104ea8(void *c);

void func_ov006_02106bc0(char *c)
{
    int i;
    for (i = 0; i < *(int *)(c + 0x4cb8); i++) {
        struct Entry *e = &data_02142840[*(unsigned char *)(c + i + 0x4000 + 0xefa)];
        int f4 = e->f4;
        char *self = c + (f4 >> 1);
        void (*fn)(void *, int);
        if (f4 & 1)
            fn = *(void (**)(void *, int))(*(char **)self + e->f0);
        else
            fn = (void (*)(void *, int))e->f0;
        fn(self, i);
    }
    if (*(unsigned short *)(c + 0x4ec0) == 0)
        return;
    *(unsigned short *)(((long long)(int)(c + 0x4ec0)) & 0xFFFFFFFFFFFFFFFFLL) -= 1;
    if (*(short *)(c + 0x4ec0) > 0)
        return;
    *(short *)(c + 0x4ec0) = 0;
    func_020b0a54(0x12);
    *(unsigned char *)(c + 0xc3) = 0;
    *(unsigned char *)(c + 0x4fe3) = 0;
    func_02104ea8(c);
}
