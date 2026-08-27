extern int _ZTV10BigBooIcon[];
extern void _ZN8dActor_cD2Ev(void *);
extern void _ZN6Memory10DeallocateEPvP4Heap(void *, void *);
extern void *data_020a0eac;
int *_ZN10BigBooIconD0Ev(int *t)
{
    t[0] = (int)_ZTV10BigBooIcon;
    _ZN8dActor_cD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
