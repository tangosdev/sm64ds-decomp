// @symbol _ZN7WigglerD0Ev
/* recovered: named members + shared header */
#include "Wiggler.h"
extern void _ZN12WithMeshClsnD1Ev(void *);
extern void __destroy_arr(void *p, int count, int size, void *dtor);
extern int _ZN5EnemyD2Ev(int *x);
extern void _ZN6Memory10DeallocateEPvP4Heap(void *p, void *h);
extern char _ZTV7Wiggler[];
extern void _ZN25MovingCylinderClsnWithPosD1Ev(void);
extern void func_02011508(void);
extern void func_020072c0(void);
extern void _ZN15TextureSequenceD1Ev(void);
extern void _ZN15MaterialChangerD1Ev(void);
extern void _ZN9ModelAnimD1Ev(void);
extern void *data_020a0eac;
int _ZN7WigglerD0Ev(struct Wiggler *self) {
  *(void**)((char *)self) = _ZTV7Wiggler;
  _ZN12WithMeshClsnD1Ev((char *)&self->mWithMeshClsn);
  __destroy_arr(((char *)self)+0x5b8, 5, 0x40, (void*)_ZN25MovingCylinderClsnWithPosD1Ev);
  __destroy_arr(((char *)self)+0x478, 5, 0x40, (void*)_ZN25MovingCylinderClsnWithPosD1Ev);
  __destroy_arr(((char *)self)+0x444, 5, 6, (void*)func_02011508);
  __destroy_arr(((char *)self)+0x408, 5, 0xc, (void*)func_020072c0);
  __destroy_arr(((char *)self)+0x3cc, 5, 0xc, (void*)func_020072c0);
  __destroy_arr(((char *)self)+0x368, 5, 0x14, (void*)_ZN15TextureSequenceD1Ev);
  __destroy_arr(((char *)self)+0x304, 5, 0x14, (void*)_ZN15MaterialChangerD1Ev);
  __destroy_arr(((char *)self)+0x110, 5, 0x64, (void*)_ZN9ModelAnimD1Ev);
  _ZN5EnemyD2Ev((int*)((char *)self));
  _ZN6Memory10DeallocateEPvP4Heap(((char *)self), data_020a0eac);
  return (int)((char *)self);
}
