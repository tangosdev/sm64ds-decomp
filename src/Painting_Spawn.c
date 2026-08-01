extern void *_ZN9ActorBasenwEj(unsigned);
extern void _ZN5ActorC2Ev(void *);
extern int data_ov080_021282b4[];
int *Painting_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(444);
    if (p) { _ZN5ActorC2Ev(p); p[0] = (int)data_ov080_021282b4; }
    return p;
}
