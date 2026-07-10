extern void _ZN18MovingCylinderClsnD1Ev(void *);
extern void _ZN5ModelD1Ev(void *);
extern void func_ov001_020ab3a0(void *);
extern void _ZN5ActorD1Ev(void *);
extern int VT0[];
int *_ZN9DorrieCapD1Ev(int *t)
{
    t[0] = (int)VT0;
    _ZN18MovingCylinderClsnD1Ev((char *)t + 0x140);
    _ZN5ModelD1Ev((char *)t + 0xf0);
    func_ov001_020ab3a0((char *)t + 0xd4);
    _ZN5ActorD1Ev(t);
    return t;
}
