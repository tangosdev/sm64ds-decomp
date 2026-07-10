extern void *_ZN9ActorBasenwEj(unsigned);
extern void _ZN5ActorC2Ev(void *);
extern void _ZN11CommonModelC1Ev(void *);
extern int VT0[];
int *StarDoor_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(280);
    if (p) {
        _ZN5ActorC2Ev(p);
        p[0] = (int)VT0;
        _ZN11CommonModelC1Ev((char *)p + 0xd4);
    }
    return p;
}
