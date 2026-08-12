extern void *_ZN9ActorBasenwEj(unsigned);
extern void _ZN8PlatformC2Ev(void *);
extern int _ZTV15FireSeaElevator[];
int *FireSeaElevator_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(808);
    if (p) { _ZN8PlatformC2Ev(p); p[0] = (int)_ZTV15FireSeaElevator; }
    return p;
}
