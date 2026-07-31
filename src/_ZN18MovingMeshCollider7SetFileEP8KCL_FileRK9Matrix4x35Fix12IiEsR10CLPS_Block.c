// @symbol _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "MovingMeshCollider.h"

struct KCL_File;
struct CLPS_Block;
extern void _ZN12MeshCollider7SetFileEP8KCL_FileR10CLPS_Block(void *thiz, void *f, void *b);
extern void InvMat4x3(void *dst, void *src);
extern void Matrix4x3_ApplyInPlaceToScale(void *m, int x, int y, int z);
extern void func_02039e18(void *thiz, int *vec, void *p);
extern Matrix4x3 data_020a0e68;
void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void *thiz, void *f, const Matrix4x3 *m, int fix, short s, void *b)
{
  char *c = (char *) thiz;
  int new_var;
  char stk[0x6c];
  _ZN12MeshCollider7SetFileEP8KCL_FileR10CLPS_Block(thiz, f, b);
  func_02039624(c);
  *((int *) (c + 0x50)) = fix;
  *((Matrix4x3 *) (c + 0x54)) = *m;
  *((Matrix4x3 *) (c + 0xe4)) = *((Matrix4x3 *) (c + 0x54));
  InvMat4x3(c + 0xe4, c + 0xe4);
  *((short *) (c + 0x114)) = s;
  *((short *) (c + 0x116)) = 0;
  *((int *) (c + 0x118)) = m->m[9];
  *((int *) (c + 0x11c)) = m->m[10];
  *((int *) (c + 0x120)) = m->m[11];
  *((int *) (c + 0x124)) = (*((int *) (c + 0x128)) = (*((int *) (c + 0x12c)) = 0));
  *((Matrix4x3 *) (stk + 0)) = *((Matrix4x3 *) (c + 0x54));
  ((Matrix4x3 *) (stk + 0))->m[9] = 0;
  ((Matrix4x3 *) (stk + 0))->m[10] = 0;
  ((Matrix4x3 *) (stk + 0))->m[11] = 0;
  *((Matrix4x3 *) (c + 0x84)) = *((Matrix4x3 *) (stk + 0));
  InvMat4x3(c + 0x84, c + 0x84);
  *((int *) (c + 0x164)) = (new_var = func_02053200(fix));
  data_020a0e68 = *m;
  Matrix4x3_ApplyInPlaceToScale(&data_020a0e68, *((int *) (c + 0x50)), *((int *) (c + 0x50)), *((int *) (c + 0x50)));
  *((Matrix4x3 *) (stk + 0x30)) = data_020a0e68;
  *((Matrix4x3 *) (c + 0x134)) = data_020a0e68;
  *((Matrix4x3 *) (c + 0x168)) = *((Matrix4x3 *) (stk + 0x30));
  InvMat4x3(c + 0x168, c + 0x168);
  *((Matrix4x3 *) (c + 0x198)) = *((Matrix4x3 *) (c + 0x168));
  *((Matrix4x3 *) (c + 0xb4)) = *((Matrix4x3 *) (stk + 0x30));
  *((int *) (stk + 0x60)) = 0;
  *((int *) (stk + 0x64)) = 0x1000;
  *((int *) (stk + 0x68)) = 0;
  func_02039e18(thiz, (int *) (stk + 0x60), c + 0x28);
  *((unsigned char *) (c + 0x130)) = 0;
}
