extern void MultiCopyHalf(const void *src, void *dst, unsigned int count);

struct H { unsigned short f : 3; };
struct B { unsigned char f : 4; };

void func_02059eb0(unsigned char *dst)
{
    unsigned char *base = (unsigned char *)0x27ffc80;
    dst[0] = (unsigned char)(((struct H *)(base + 0x64))->f);
    dst[1] = (unsigned char)(((struct B *)(base + 2))->f);
    dst[2] = base[3];
    dst[3] = base[4];
    *(unsigned short *)(dst + 0x18) = base[0x1a];
    *(unsigned short *)(dst + 0x4e) = base[0x50];
    MultiCopyHalf(base + 6, dst + 4, 0x14);
    MultiCopyHalf(base + 0x1c, dst + 0x1a, 0x34);
}
