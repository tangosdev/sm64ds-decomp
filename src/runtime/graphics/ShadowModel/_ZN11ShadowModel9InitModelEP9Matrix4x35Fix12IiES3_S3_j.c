// @symbol _ZN11ShadowModel9InitModelEP9Matrix4x35Fix12IiES3_S3_j
/* The mangled name is ShadowModel::InitModel(Matrix4x3 *, Fix12<int>,
 * Fix12<int>, Fix12<int>, u32) and include/ShadowModel.h declares exactly
 * that. The DEFINITION stays this C file because of wall 6az in
 * notes/mwccarm-codegen.md: CW homes class-typed by-value parameters to the
 * stack, so a real method body with the true Fix12<int> signature comes out
 * 0x14 bytes bigger than the ROM. Scalar args here keep the bytes.
 */
#include "types.h"
#include "ShadowModel.h"

extern unsigned char data_0209ceec;
extern struct ShadowModel *data_0209cef4;

void _ZN11ShadowModel9InitModelEP9Matrix4x35Fix12IiES3_S3_j(struct ShadowModel *self, int a1, int a2, int a3, int a4, unsigned char a5) {
  *(int*)((char*)&self->mat) = a1;
  *(int*)((char*)&self->scale.x) = a2;
  *(int*)((char*)&self->scale.y) = a3;
  *(int*)((char*)&self->scale.z) = a4;
  *(unsigned char*)((char*)&self->opacity) = a5;
  if(data_0209ceec) return;
  *(void**)((char*)&self->next) = *(void* volatile*)&data_0209cef4;
  {
    void* head = (void*)data_0209cef4;
    if(head) *(void**)((char*)head+0x20) = ((void*)self);
  }
  data_0209cef4 = self;
}
