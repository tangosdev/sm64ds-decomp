void func_0200d8ac(char *c, int r1, int r2, int r3)
{
    c = c + 0x100;
    short ip = *(short *)(c + 0x8e);
    if (r1 > ip) {
        *(short *)(c + 0x8e) = (short)r1;
        *(short *)(c + 0x90) = (short)r2;
        *(short *)(c + 0x94) = (short)r3;
    }
}
