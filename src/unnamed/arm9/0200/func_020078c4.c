struct Camera;

extern short ReadUnalignedShort(unsigned char *p);
extern void ApproachAngle(int a, int b, int c, int d);

int func_020078c4(struct Camera *self, const unsigned char *data)
{
    short v = ReadUnalignedShort((unsigned char *)data);
    volatile int dummy = 0;               /* forces store of 0 on stack */
    unsigned char b = data[2];
    ApproachAngle((int)self + 0x17a, (int)v, b, 0x4000);
    return 1;
}
