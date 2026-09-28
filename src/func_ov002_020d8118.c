extern void _ZN6Player19func_ov002_020bda48Ev(char* c);
extern void func_ov002_020bd9ec(char* c, unsigned int v);
extern void func_ov002_020c43c4(char* c, int v);

void func_ov002_020d8118(char* c)
{
    _ZN6Player19func_ov002_020bda48Ev(c);
    *(short*)(c + 0x600 + 0xbe) = 0x258;
    func_ov002_020bd9ec(c, 0x32);
    func_ov002_020c43c4(c, 5);
    *(unsigned char*)(c + 0x6f8) = 1;
}
