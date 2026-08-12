// @symbol _ZN12daBakubaku_cD0Ev
/* Vtable slot 17, the deleting destructor -- previously func_ov032_021111f0.
 * Stays C: see include/Enemy.h on why the inline operator delete on the immediate
 * base is what lets a D0 reproduce at all. */
extern void _ZN11ShadowModelD1Ev(void *);
extern void _ZN9ModelAnimD1Ev(void *);
extern void _ZN12WithMeshClsnD1Ev(void *);
extern void _ZN25MovingCylinderClsnWithPosD1Ev(void *);
extern void _ZN5EnemyD2Ev(void *);
extern void _ZN6Memory10DeallocateEPvP4Heap(void *, void *);
extern int _ZTV12daBakubaku_c[];
extern void *data_020a0eac;
int *_ZN12daBakubaku_cD0Ev(int *t)
{
    t[0] = (int)_ZTV12daBakubaku_c;
    _ZN11ShadowModelD1Ev((char *)t + 0x3b4);
    _ZN9ModelAnimD1Ev((char *)t + 0x34c);
    _ZN12WithMeshClsnD1Ev((char *)t + 0x190);
    _ZN25MovingCylinderClsnWithPosD1Ev((char *)t + 0x150);
    _ZN25MovingCylinderClsnWithPosD1Ev((char *)t + 0x110);
    _ZN5EnemyD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
