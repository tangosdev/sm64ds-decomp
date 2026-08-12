extern int _ZTV9CameraTag[];
extern void _ZN5ActorD2Ev(void *);
extern void _ZN6Memory10DeallocateEPvP4Heap(void *, void *);
extern void *data_020a0eac;
int *_ZN9CameraTagD0Ev(int *t)
{
    t[0] = (int)_ZTV9CameraTag;
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
