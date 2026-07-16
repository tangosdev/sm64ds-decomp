extern void _ZN9ActorBaseD1Ev(void *self);
extern void _ZN6Memory10DeallocateEPvP4Heap(void *ptr, void *heap);
extern int data_ov003_020b1650[];
extern int _ZTV5Stage[];
extern int data_0208e4b8[];
extern void *data_020a0eac;

int *func_ov003_020ad69c(int *t)
{
    t[0] = (int)data_ov003_020b1650;
    t[0] = (int)_ZTV5Stage;
    t[0] = (int)data_0208e4b8;
    _ZN9ActorBaseD1Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
