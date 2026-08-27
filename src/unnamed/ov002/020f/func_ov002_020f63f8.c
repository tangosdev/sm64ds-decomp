extern void func_ov002_020f5dd8(void *p, unsigned char v);

int func_ov002_020f63f8(char *c, unsigned char *src)
{
    *(unsigned char*)(c + 0x100) = *src;
    func_ov002_020f5dd8(*(void**)(c + 0xd8), *(unsigned char*)(c + 0x100));
    return 1;
}
