extern void *_ZN9ActorBasenwEj(unsigned);
extern void _ZN5EnemyC2Ev(void *);
extern void _ZN25MovingCylinderClsnWithPosC1Ev(void *);
extern void _ZN12WithMeshClsnC1Ev(void *);
extern void _ZN9ModelAnimC1Ev(void *);
extern int VT0[];
int *CheepCheep_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(904);
    if (p) {
        _ZN5EnemyC2Ev(p);
        p[0] = (int)VT0;
        _ZN25MovingCylinderClsnWithPosC1Ev((char *)p + 0x110);
        _ZN12WithMeshClsnC1Ev((char *)p + 0x150);
        _ZN9ModelAnimC1Ev((char *)p + 0x30c);
    }
    return p;
}
