// @symbol _ZN11ShadowModel9InitModelEP9Matrix4x35Fix12IiES3_S3_j
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "ShadowModel.h"
void _ZN11ShadowModel9InitModelEP9Matrix4x35Fix12IiES3_S3_j(struct ShadowModel *self, int a1, int a2, int a3, int a4, unsigned char a5) {
  *(int*)((char*)&self->unk_00c) = a1;
  *(int*)((char*)&self->unk_010) = a2;
  *(int*)((char*)&self->unk_014) = a3;
  *(int*)((char*)&self->unk_018) = a4;
  *(unsigned char*)((char*)&self->unk_01c) = a5;
  if(data_0209ceec[0]) return;
  *(void**)((char*)&self->unk_024) = *(void* volatile*)&data_0209cef4[0];
  void* head = data_0209cef4[0];
  if(head) *(void**)((char*)head+0x20) = ((void*)self);
  data_0209cef4[0] = ((void*)self);
}
