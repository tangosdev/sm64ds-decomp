struct dActor_c;

extern int data_02086ed8[3];
extern int func_02007cec(int a, int b, int c);
extern void _Z14ApproachLinearRiii(int *p, int value, int speed);

int func_020073a0(struct dActor_c *actor)
{
    int stackbuf[3];
    stackbuf[0] = data_02086ed8[0];
    stackbuf[1] = data_02086ed8[1];
    stackbuf[2] = data_02086ed8[2];

    func_02007cec((int)((char *)actor + 0xb0), (int)stackbuf, 0xcc);
    _Z14ApproachLinearRiii((int *)((char *)actor + 0xc0), 0, 0x2000);
    _Z14ApproachLinearRiii((int *)((char *)actor + 0xc4), -0xc8000, 0x6aaa);
    return 1;
}
