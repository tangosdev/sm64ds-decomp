extern void *_ZN9ActorBasenwEj(unsigned);
extern void _ZN5EnemyC2Ev(void *);
extern void _ZN25MovingCylinderClsnWithPosC1Ev(void *);
extern void _ZN12WithMeshClsnC1Ev(void *);
extern void _ZN14BlendModelAnimC1Ev(void *);
extern void func_020733a8(void*, int, int, void*, void*);
extern int data_ov016_02114958[];
extern int func_020072c0[];
extern int func_0203d384[];
int *func_ov016_02112588(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(0x4b0);
    if (p) {
        _ZN5EnemyC2Ev(p);
        p[0] = (int)data_ov016_02114958;
        _ZN25MovingCylinderClsnWithPosC1Ev((char *)p + 0x110);
        _ZN25MovingCylinderClsnWithPosC1Ev((char *)p + 0x150);
        _ZN12WithMeshClsnC1Ev((char *)p + 0x190);
        _ZN14BlendModelAnimC1Ev((char *)p + 0x350);
        func_020733a8((char *)p + 0x448, 7, 0xc, (void*)func_0203d384, (void*)func_020072c0);
    }
    return p;
}
