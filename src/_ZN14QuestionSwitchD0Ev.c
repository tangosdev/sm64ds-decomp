extern void _ZN9ModelAnimD1Ev(void *);
extern void _ZN18MovingMeshColliderD1Ev(void *);
extern void _ZN5ModelD1Ev(void *);
extern void _ZN5ActorD1Ev(void *);
extern void _ZN6Memory10DeallocateEPvP4Heap(void *, void *);
extern int _ZTV14QuestionSwitch[];
extern int _ZTV17ExclamationSwitch[];
extern void *data_020a0eac;
int *_ZN14QuestionSwitchD0Ev(int *t)
{
    t[0] = (int)_ZTV14QuestionSwitch;
    _ZN9ModelAnimD1Ev((char *)t + 0x6b4);
    _ZN18MovingMeshColliderD1Ev((char *)t + 0x4ec);
    _ZN18MovingMeshColliderD1Ev((char *)t + 0x324);
    t[0] = (int)_ZTV17ExclamationSwitch;
    _ZN18MovingMeshColliderD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN5ActorD1Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
