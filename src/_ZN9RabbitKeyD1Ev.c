extern void _ZN11ShadowModelD1Ev(void *);
extern void _ZN5ModelD1Ev(void *);
extern void func_ov002_020aed18(void *);
extern int VT0[];
int *_ZN9RabbitKeyD1Ev(int *t)
{
    t[0] = (int)VT0;
    _ZN11ShadowModelD1Ev((char *)t + 0x160);
    _ZN5ModelD1Ev((char *)t + 0x110);
    func_ov002_020aed18(t);
    return t;
}
