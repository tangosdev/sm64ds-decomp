void func_02037b1c(char* c);
void func_020380c0(char* c);

void func_02037b5c(char* c)
{
    unsigned char* f;
    func_02037b1c(c);
    f = (unsigned char*)(((int)c + 0x70) & 0xFFFFFFFFFFFFFFFF);
    *f &= ~1;
    *f &= ~4;
    *f &= ~8;
    *f &= ~0x10;
    *f &= ~2;
    *f &= ~0x20;
    *f &= ~0x40;
    func_020380c0(c + 0x10);
    func_020380c0(c + 0x74);
    func_020380c0(c + 0x9c);
    func_020380c0(c + 0xc4);
    *(int*)(c + 0xfc) = 0;
    *(int*)(c + 0x100) = -0x1000;
    *(int*)(c + 0x104) = 0;
}
