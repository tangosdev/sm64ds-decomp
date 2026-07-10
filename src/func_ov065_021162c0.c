extern void ApproachAngle(short *a, short b, short c, short d, short e);
extern void _Z14ApproachLinearRsss(short *a, short b, short c);
extern int _ZN6EyerokD0Ev(char *t);
int func_ov065_021162c0(char *t)
{
    ApproachAngle((short *)(t + 0x8c), -0x4000, 0xa, 0x200, 0x100);
    _Z14ApproachLinearRsss((short *)(t + 0x8c), -0x4000, 0x200);
    if (*(unsigned short *)(t + 0x100) == 0)
        _ZN6EyerokD0Ev(t);
    return 1;
}
