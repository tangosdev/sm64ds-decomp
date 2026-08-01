extern void *_ZN9ActorBasenwEj(unsigned);
extern void _ZN5ActorC2Ev(void *);
extern int data_ov018_02113a74[];
int *PowerStarCreate_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(212);
    if (p) { _ZN5ActorC2Ev(p); p[0] = (int)data_ov018_02113a74; }
    return p;
}
