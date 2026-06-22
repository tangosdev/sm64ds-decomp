typedef unsigned char u8;

extern int func_020092c4(void *a, void *b, int c);
extern void func_0200928c(void *cam);
extern int ApproachAngle(void *p, int target, int a, int b, int c);

int func_020091f8(void *a, int b, int c, int d)
{
    u8 *f = (u8 *)a;
    int r4;
    int t;
    r4 = (func_020092c4(a, f + 0x80, b) != 1);
    func_0200928c(a);
    t = ApproachAngle(f + 0x17c, c, 8, 0x4000, 0x80);
    if (t != 0) r4 = 1;
    t = ApproachAngle(f + 0x17e, d, 8, 0x4000, 0x80);
    return (r4 | t) == 0;
}
