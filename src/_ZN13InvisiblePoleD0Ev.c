extern int _ZTV13InvisiblePole[];
extern void _ZN5ActorD2Ev(void *);
extern void _ZN6Memory10DeallocateEPvP4Heap(void *, void *);
extern void *data_020a0eac;
int *_ZN13InvisiblePoleD0Ev(int *t)
{
    t[0] = (int)_ZTV13InvisiblePole;
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
