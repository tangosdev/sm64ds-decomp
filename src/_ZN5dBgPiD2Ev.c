extern void _ZN5dBgPcD2Ev(void *);
extern int data_02099368[];
int *_ZN5dBgPiD2Ev(int *t)
{
    t[0] = (int)data_02099368;
    _ZN5dBgPcD2Ev((char *)t + 0x4);
    return t;
}
