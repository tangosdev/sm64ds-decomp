extern void *_ZN9ActorBasenwEj(unsigned);
extern void _ZN8PlatformC2Ev(void *);
extern int data_ov079_02127fb8[];
int *BillBlaster_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(804);
    if (p) { _ZN8PlatformC2Ev(p); p[0] = (int)data_ov079_02127fb8; }
    return p;
}
