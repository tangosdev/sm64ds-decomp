extern void *_ZN9ActorBasenwEj(unsigned);
extern void _ZN5EnemyC2Ev(void *);
extern void _ZN9ModelAnimC1Ev(void *);
extern void _ZN18MovingCylinderClsnC1Ev(void *);
extern void _ZN25MovingCylinderClsnWithPosC1Ev(void *);
extern int VT0[];
int *FirePiranhaPlant_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(556);
    if (p) {
        _ZN5EnemyC2Ev(p);
        p[0] = (int)VT0;
        _ZN9ModelAnimC1Ev((char *)p + 0x110);
        _ZN18MovingCylinderClsnC1Ev((char *)p + 0x174);
        _ZN25MovingCylinderClsnWithPosC1Ev((char *)p + 0x1a8);
    }
    return p;
}
