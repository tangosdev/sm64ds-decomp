extern int RandomIntInternal(void *);
extern int data_0209e650[];
void func_ov060_021171e8(char *c)
{
    unsigned int r = RandomIntInternal(data_0209e650);
    *(short *)(c + 0x94) = r >> 0x10;
    *(int *)(c + 0xa8) = 0xa000;
    *(int *)(c + 0x98) = 0;
    *(int *)(c + 0x360) = 0x6000;
    *(int *)(c + 0x368) = 0x10;
}
