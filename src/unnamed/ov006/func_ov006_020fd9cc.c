extern int RandomIntInternal(int *seed);
extern int data_0209d4b8;

void func_ov006_020fd9cc(char *base, int idx)
{
    int off = idx * 0x38;
    unsigned short *timer = (unsigned short *)(base + 0x4688 + off);
    unsigned int r;
    char *o;
    if (*timer != 0) {
        *timer = *timer - 1;
        return;
    }
    r = RandomIntInternal(&data_0209d4b8);
    o = base + off;
    *(unsigned char *)(o + 0x468f) = ((((r >> 16) & 0x7fff) << 1) >> 0xf) + 1;
    *(unsigned char *)(o + 0x4690) = 0;
    *(unsigned char *)(o + 0x468d) = 1;
    *(unsigned char *)(o + 0x4693) = 0;
    *(unsigned short *)(o + 0x4684) = 0;
    *(int *)(o + 0x4678) = 0;
    *(int *)(o + 0x4670) = 0;
    *(int *)(o + 0x4674) = 0;
    *(int *)(o + 0x4680) = 0x1000;
    *(unsigned char *)(o + 0x4695) = 0;
}
