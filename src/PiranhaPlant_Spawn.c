extern void *_ZN9ActorBasenwEj(unsigned);
extern void _ZN5EnemyC2Ev(void *);
extern void _ZN9ModelAnimC1Ev(void *);
extern void _ZN5ModelC1Ev(void *);
extern void _ZN12WithMeshClsnC1Ev(void *);
extern void _ZN18MovingCylinderClsnC1Ev(void *);
extern void _ZN25MovingCylinderClsnWithPosC1Ev(void *);
extern int VT0[];
int *PiranhaPlant_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(1148);
    if (p) {
        _ZN5EnemyC2Ev(p);
        p[0] = (int)VT0;
        _ZN9ModelAnimC1Ev((char *)p + 0x110);
        _ZN5ModelC1Ev((char *)p + 0x174);
        _ZN12WithMeshClsnC1Ev((char *)p + 0x1c4);
        _ZN18MovingCylinderClsnC1Ev((char *)p + 0x380);
        _ZN18MovingCylinderClsnC1Ev((char *)p + 0x3b4);
        _ZN25MovingCylinderClsnWithPosC1Ev((char *)p + 0x3e8);
    }
    return p;
}
