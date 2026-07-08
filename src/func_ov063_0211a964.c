typedef unsigned short u16;
typedef unsigned char u8;
extern void func_ov063_0211a960(void *p);
extern char SINE_TABLE[];

void func_ov063_0211a964(char *c, int arg1)
{
    int t;
    if (*(u8 *)(c + 0x5c8) != 0xff) {
        if (arg1 == 0)
            return;
    }
    t = *(u16 *)(c + 0x5b8) >> 4;
    *(int *)(c + 0x80) = *(int *)(c + 0x584) +
        ((*(short *)(SINE_TABLE + t * 4) << 3) / 100);
    t = *(u16 *)(c + 0x5b8) >> 4;
    *(int *)(c + 0x84) = *(int *)(c + 0x584) +
        ((-(*(short *)(SINE_TABLE + t * 4) << 3)) / 100);
    *(int *)(c + 0x88) = *(int *)(c + 0x80);
    func_ov063_0211a960(c);
    *(int *)(c + 0x188) = *(int *)(c + 0x590) * *(int *)(c + 0x80);
    *(int *)(c + 0x18c) = *(int *)(c + 0x594) * *(int *)(c + 0x84);
}
