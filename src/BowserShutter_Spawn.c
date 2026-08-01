extern void *_ZN9ActorBasenwEj(unsigned);
extern void _ZN8PlatformC2Ev(void *);
extern int data_ov026_02113ba4[];
int *BowserShutter_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(800);
    if (p) { _ZN8PlatformC2Ev(p); p[0] = (int)data_ov026_02113ba4; }
    return p;
}
