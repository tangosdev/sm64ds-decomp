// @symbol RollingRock_Spawn
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV11RollingRock */
extern void *_ZN9ActorBasenwEj(unsigned);
extern void _ZN5EnemyC2Ev(void *);
extern void _ZN5ModelC1Ev(void *);
extern void _ZN11ShadowModelC1Ev(void *);
extern void _ZN25MovingCylinderClsnWithPosC1Ev(void *);
extern void _ZN12WithMeshClsnC1Ev(void *);
extern int _ZTV11RollingRock[];
int *RollingRock_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(968);
    if (p) {
        _ZN5EnemyC2Ev(p);
        p[0] = (int)_ZTV11RollingRock;
        _ZN5ModelC1Ev((char *)p + 0x110);
        _ZN11ShadowModelC1Ev((char *)p + 0x160);
        _ZN25MovingCylinderClsnWithPosC1Ev((char *)p + 0x1b8);
        _ZN12WithMeshClsnC1Ev((char *)p + 0x1f8);
    }
    return p;
}
