// @symbol _ZN7WigglerD1Ev
/* recovered: named members + shared header */
#include "Wiggler.h"
extern int _ZN12WithMeshClsnD1Ev(void *p);
extern int __destroy_arr(void *p, int a, int b, void *f);
extern int func_ov002_020aed18(void *p);
extern int _ZN25MovingCylinderClsnWithPosD1Ev(void *p);
extern int func_02011508(void *p);
extern int func_020072c0(void *p);
extern int _ZN15TextureSequenceD1Ev(void *p);
extern int _ZN15MaterialChangerD1Ev(void *p);
extern int _ZN9ModelAnimD1Ev(void *p);
extern int _ZTV7Wiggler[];
int _ZN7WigglerD1Ev(struct Wiggler *self) {
  *(int**)(((char *)self)) = _ZTV7Wiggler;
  _ZN12WithMeshClsnD1Ev((char *)&self->mWithMeshClsn);
  __destroy_arr(((char *)self)+0x5b8, 5, 0x40, _ZN25MovingCylinderClsnWithPosD1Ev);
  __destroy_arr(((char *)self)+0x478, 5, 0x40, _ZN25MovingCylinderClsnWithPosD1Ev);
  __destroy_arr(((char *)self)+0x444, 5, 6, func_02011508);
  __destroy_arr(((char *)self)+0x408, 5, 0xc, func_020072c0);
  __destroy_arr(((char *)self)+0x3cc, 5, 0xc, func_020072c0);
  __destroy_arr(((char *)self)+0x368, 5, 0x14, _ZN15TextureSequenceD1Ev);
  __destroy_arr(((char *)self)+0x304, 5, 0x14, _ZN15MaterialChangerD1Ev);
  __destroy_arr(((char *)self)+0x110, 5, 0x64, _ZN9ModelAnimD1Ev);
  func_ov002_020aed18(((char *)self));
  return (int)((char *)self);
}
