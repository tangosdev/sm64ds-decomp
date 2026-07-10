extern int func_ov002_020b6424(char *t, void **f);
extern char data_ov022_021140d4[];

int _ZN21FloatingFloorLllSmall16CleanupResourcesEv(char *obj)
{
    return func_ov002_020b6424(obj, (void **)(data_ov022_021140d4 + *(unsigned char *)(obj + 0x32c) * 0xc));
}
