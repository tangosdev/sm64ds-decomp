extern void *_ZN9ActorBasenwEj(unsigned);
extern void _ZN5ActorC2Ev(void *);
extern int _ZTV18PoppingLavaBubbles[];
int *PoppingLavaBubbles_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(216);
    if (p) { _ZN5ActorC2Ev(p); p[0] = (int)_ZTV18PoppingLavaBubbles; }
    return p;
}
