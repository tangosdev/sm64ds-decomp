//cpp
extern "C" {
extern void *_ZN9ActorBasenwEj(unsigned int);
extern void _ZN5ActorC2Ev(void *);
extern void _ZN9ModelAnimC1Ev(void *);
extern void func_0203b9b4(int *p, int v);
extern void *_ZTV4Fish;
void *Fish_Spawn(void)
{
    char *p = (char *)_ZN9ActorBasenwEj(0x160);
    if (p)
    {
        _ZN5ActorC2Ev(p);
        *(void **)p = &_ZTV4Fish;
        _ZN9ModelAnimC1Ev(p + 0xd4);
        func_0203b9b4((int *)(p + 0x138), 1);
    }
    return p;
}
}
