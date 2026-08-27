extern void _ZN15ModelComponents11UpdateBonesEP8BCA_Filei(void *self, void *bca, int n);

void func_020167a4(char *p)
{
    _ZN15ModelComponents11UpdateBonesEP8BCA_Filei(
        p + 8,
        *(void **)(p + 0x60),
        (int)((*(unsigned int *)(p + 0x58) << 4) >> 0x10));
}
