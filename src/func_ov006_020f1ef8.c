extern void func_ov006_020f00a4(char *o);

void func_ov006_020f1ef8(char *o, int p1)
{
    int v;
    *(short *)(o + 0x5166) = (short)(((int *)(o + 0x47f8))[*(unsigned char *)(o + 0x5456) - 1] >> 0xc);
    *(short *)(o + 0x5168) = (short)(((int *)(o + 0x49d8))[*(unsigned char *)(o + 0x5456) - 1] >> 0xc);
    *(short *)(o + 0x5164) = 0x60;
    *(int *)(o + 0x4f78) = 3;
    *(short *)(o + 0x516a) = 0xc8;
    v = *(int *)(o + 0xbc);
    while (v >= 5) v -= 5;
    if (v != 4)
        *(unsigned short *)(((int)o + 0x516a) & 0xFFFFFFFFFFFFFFFF) += 8;
    if (p1 == 0)
        *(short *)(o + 0x516a) = 0x80;
    *(unsigned char *)(o + 0x5459) = (unsigned char)p1;
    func_ov006_020f00a4(o);
    *(unsigned char *)(o + 0x47f5) = 1;
}
