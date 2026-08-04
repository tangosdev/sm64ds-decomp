extern void *_ZN9ActorBasenwEj(unsigned);
extern void _ZN5ActorC2Ev(void *);
extern int _ZTV11SoundObject[];
int *SoundObject_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(228);
    if (p) { _ZN5ActorC2Ev(p); p[0] = (int)_ZTV11SoundObject; }
    return p;
}
