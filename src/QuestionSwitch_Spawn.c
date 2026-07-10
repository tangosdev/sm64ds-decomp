extern void *_ZN9ActorBasenwEj(unsigned);
extern void _ZN8PlatformC2Ev(void *);
extern void _ZN18MovingMeshColliderC1Ev(void *);
extern void _ZN9ModelAnimC1Ev(void *);
extern int _ZTV14QuestionSwitch[];
int *QuestionSwitch_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(0x724);
    if (p) {
        _ZN8PlatformC2Ev(p);
        p[0] = (int)_ZTV14QuestionSwitch;
        _ZN18MovingMeshColliderC1Ev((char *)p + 0x324);
        _ZN18MovingMeshColliderC1Ev((char *)p + 0x4ec);
        _ZN9ModelAnimC1Ev((char *)p + 0x6b4);
    }
    return p;
}
