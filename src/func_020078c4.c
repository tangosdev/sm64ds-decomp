struct Camera;

extern short ReadUnalignedShort(unsigned char *p);
extern int ApproachAngle(short *cur, short target, int divisor, int band, int maxStep);

int func_020078c4(struct Camera *self, const unsigned char *data)
{
    short v = ReadUnalignedShort((unsigned char *)data);
    unsigned char b = data[2];
    ApproachAngle((short *)((char *)self + 0x17a), v, b, 0x4000, 0);
    return 1;
}
