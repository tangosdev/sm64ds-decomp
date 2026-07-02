extern int RandomIntInternal(int *seed);
extern int data_0209e650;
void func_ov091_02133020(char *c)
{
    (*(int *)(((int)c + 0x60) & 0xFFFFFFFFFFFFFFFF)) += 0xa000;
    if (*(int *)(c + 0x60) < *(int *)(c + 0x390))
        return;
    *(int *)(c + 0x60) = *(int *)(c + 0x390);
    *(int *)(c + 0x398) = 1;
    *(unsigned char *)(c + 0x39e) =
        (unsigned char)(((unsigned int)RandomIntInternal(&data_0209e650) >> 0x10) % 0x1e + 0xa);
}
