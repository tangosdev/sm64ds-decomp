extern void *_ZN9ActorBasenwEj(unsigned);
extern void _ZN5ActorC2Ev(void *);
extern void _ZN18MovingCylinderClsnC1Ev(void *);
extern void _ZN9ModelAnimC1Ev(void *);
extern void _ZN11ShadowModelC1Ev(void *);
extern int VT0[];
int *BobOmbBuddy_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(496);
    if (p) {
        _ZN5ActorC2Ev(p);
        p[0] = (int)VT0;
        _ZN18MovingCylinderClsnC1Ev((char *)p + 0xd4);
        _ZN9ModelAnimC1Ev((char *)p + 0x108);
        _ZN11ShadowModelC1Ev((char *)p + 0x16c);
    }
    return p;
}
