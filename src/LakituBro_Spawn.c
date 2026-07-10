extern void *_ZN9ActorBasenwEj(unsigned);
extern void _ZN5EnemyC2Ev(void *);
extern void _ZN9ModelAnimC1Ev(void *);
extern void _ZN15TextureSequenceC1Ev(void *);
extern void _ZN11ShadowModelC1Ev(void *);
extern int VT0[];
int *LakituBro_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(744);
    if (p) {
        _ZN5EnemyC2Ev(p);
        p[0] = (int)VT0;
        _ZN9ModelAnimC1Ev((char *)p + 0x110);
        _ZN9ModelAnimC1Ev((char *)p + 0x174);
        _ZN15TextureSequenceC1Ev((char *)p + 0x1d8);
        _ZN11ShadowModelC1Ev((char *)p + 0x1f0);
        _ZN11ShadowModelC1Ev((char *)p + 0x218);
    }
    return p;
}
