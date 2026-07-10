extern void _ZN25MovingCylinderClsnWithPosD1Ev(void*);
extern void _ZN11ShadowModelD1Ev(void*);
extern void _ZN12WithMeshClsnD1Ev(void*);
extern void _ZN18MovingMeshColliderD1Ev(void*);
extern void _ZN5ModelD1Ev(void*);
extern void _ZN5ActorD1Ev(void*);
extern void _ZN6Memory10DeallocateEPvP4Heap(void*, void*);
extern void* _ZTV5Crate;
extern void* _ZTV17ExclamationSwitch;
extern void* data_020a0eac[];

void* _ZN5CrateD0Ev(char* c)
{
    *(void**)c = &_ZTV5Crate;
    _ZN25MovingCylinderClsnWithPosD1Ev(c+0x5a4);
    _ZN25MovingCylinderClsnWithPosD1Ev(c+0x564);
    _ZN11ShadowModelD1Ev(c+0x508);
    _ZN12WithMeshClsnD1Ev(c+0x320);
    *(void**)c = &_ZTV17ExclamationSwitch;
    _ZN18MovingMeshColliderD1Ev(c+0x124);
    _ZN5ModelD1Ev(c+0xd4);
    _ZN5ActorD1Ev(c);
    _ZN6Memory10DeallocateEPvP4Heap(c, data_020a0eac[0]);
    return c;
}
