extern void *_ZN9ActorBasenwEj(unsigned);
extern void _ZN5ActorC2Ev(void *);
extern int data_ov002_0210c0dc[];
int *SoundObject_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(228);
    if (p) { _ZN5ActorC2Ev(p); p[0] = (int)data_ov002_0210c0dc; }
    return p;
}
