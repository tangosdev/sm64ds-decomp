extern void *_ZN9ActorBasenwEj(unsigned);
extern void _ZN5EnemyC2Ev(void *);
extern void _ZN12WithMeshClsnC1Ev(void *);
extern void _ZN9ModelAnimC1Ev(void *);
extern void _ZN15TextureSequenceC1Ev(void *);
extern void _ZN11ShadowModelC1Ev(void *);
extern void _ZN18MovingMeshColliderC1Ev(void *);
extern int _ZTV5Whomp[];
int *WhompKing_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(0x610);
    if (p) {
        _ZN5EnemyC2Ev(p);
        p[0] = (int)_ZTV5Whomp;
        _ZN12WithMeshClsnC1Ev((char *)p + 0x110);
        _ZN9ModelAnimC1Ev((char *)p + 0x2cc);
        _ZN15TextureSequenceC1Ev((char *)p + 0x330);
        _ZN11ShadowModelC1Ev((char *)p + 0x344);
        _ZN18MovingMeshColliderC1Ev((char *)p + 0x418);
    }
    return p;
}
