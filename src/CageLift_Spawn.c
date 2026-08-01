extern void *_ZN9ActorBasenwEj(unsigned);
extern void _ZN8PlatformC2Ev(void *);
extern int data_ov029_02113e74[];
int *CageLift_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(816);
    if (p) { _ZN8PlatformC2Ev(p); p[0] = (int)data_ov029_02113e74; }
    return p;
}
