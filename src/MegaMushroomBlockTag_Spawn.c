extern void *_ZN9ActorBasenwEj(unsigned);
extern void _ZN5ActorC2Ev(void *);
extern int VT[];
int *MegaMushroomBlockTag_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(220);
    if (p) { _ZN5ActorC2Ev(p); p[0] = (int)VT; }
    return p;
}
