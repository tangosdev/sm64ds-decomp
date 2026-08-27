extern void func_ov006_020d3624(char *self);

void func_ov006_020d1450(char *self)
{
    int row = 0;
    int col;
    int rowoff = 0;
    int one = 0;
    int val = 0;
    for (; row < 0x100; row++) {
        for (col = 0; col < 0x158; col++) {
            unsigned char *base = *(unsigned char **)(self + 0x4000 + 0x70c);
            unsigned char *p = (unsigned char *)(rowoff + (int)base) + col;
            if (*p != 0) *p = val;
        }
        rowoff += 0x158;
    }
    func_ov006_020d3624(self);
    *(unsigned char *)(self + 0x4704) = 0;
}
