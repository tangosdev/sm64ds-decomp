void func_ov007_020c90c0(char *c)
{
    short value = *(short *)(c + 0xc);

    if (value >= 0x1000) {
        *(short *)(c + 0xc) = 0x1000;
        *(int *)c = 2;
        return;
    }

    {
        int speed = *(int *)(c + 8);
        short *pos = (short *)(((int)c + 0xc));
        short old = *pos;

        speed <<= 16;
        *pos = old + (speed >> 16);
    }
}
