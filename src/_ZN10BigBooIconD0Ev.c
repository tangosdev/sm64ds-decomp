extern int VT[];
extern void _ZN5ActorD1Ev(void *);
extern void _ZN6Memory10DeallocateEPvP4Heap(void *, void *);
extern void *HEAP;
int *_ZN10BigBooIconD0Ev(int *t)
{
    t[0] = (int)VT;
    _ZN5ActorD1Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, HEAP);
    return t;
}
