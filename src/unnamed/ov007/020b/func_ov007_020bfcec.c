extern int func_ov007_020c3df4(int a, void *b);
extern void *func_ov007_020c8b10(void);
extern void InitialiseVramGlobals(void);
extern int func_ov007_020b3f20(int n);
extern int func_ov007_020bc6d4(int a, int b);

extern char *data_ov007_02104bd8;

void func_ov007_020bfcec(void)
{
    int r;
    char *o;

    data_ov007_02104bd8 = (char *)func_ov007_020c3df4(0, (void *)0x14);
    *(void **)data_ov007_02104bd8 = func_ov007_020c8b10();
    InitialiseVramGlobals();
    r = func_ov007_020b3f20(0x24);
    r = func_ov007_020bc6d4(r, 0);
    *(int *)(data_ov007_02104bd8 + 4) = r;
    o = *(char **)(data_ov007_02104bd8 + 4);
    *(int *)(o + 0x94) = 0;
    *(int *)(data_ov007_02104bd8 + 0x10) = 0;
    *(int *)(data_ov007_02104bd8 + 8) = 0;
    *(int *)(data_ov007_02104bd8 + 0xc) = 0;
}
