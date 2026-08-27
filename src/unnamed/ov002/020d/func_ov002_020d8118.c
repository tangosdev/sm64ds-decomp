extern void func_ov002_020bda48(char* c);
extern void func_ov002_020bd9ec(char* c, unsigned int v);
extern void func_ov002_020c43c4(char* c, int v);

void func_ov002_020d8118(char* c)
{
    func_ov002_020bda48(c);
    *(short*)(c + 0x600 + 0xbe) = 0x258;
    func_ov002_020bd9ec(c, 0x32);
    func_ov002_020c43c4(c, 5);
    *(unsigned char*)(c + 0x6f8) = 1;
}
