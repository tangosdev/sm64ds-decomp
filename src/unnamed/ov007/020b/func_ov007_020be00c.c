extern int func_ov007_020c3df4(int a, void *b);
extern void func_0204f94c(int *p);

extern int data_ov007_02103218;
extern int data_ov007_02103220;
extern char *data_ov007_02104bbc;
extern int *data_ov007_02104bb8;

void func_ov007_020be00c(void)
{
    int i;
    int j;

    data_ov007_02104bbc = (char *)func_ov007_020c3df4((int)&data_ov007_02103218, (void *)0xe4);
    data_ov007_02104bb8 = (int *)func_ov007_020c3df4((int)&data_ov007_02103220, (void *)0x10);

    for (i = 0; i < 0x39; i++)
        func_0204f94c((int *)(data_ov007_02104bbc + i * 4));

    for (j = 0; j < 4; j++)
        data_ov007_02104bb8[j] = 0;
}
