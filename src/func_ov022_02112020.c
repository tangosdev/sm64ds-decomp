extern int func_ov002_020b6424(char *t, void **f);
extern char data_ov022_021140d4[];

int func_ov022_02112020(char *obj)
{
    return func_ov002_020b6424(obj, (void **)(data_ov022_021140d4 + *(unsigned char *)(obj + 0x32c) * 0xc));
}
