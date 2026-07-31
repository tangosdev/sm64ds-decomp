// @symbol _ZN9Animation17UpdateFileOffsets_ZN9AnimationER8BCA_File
/* recovered: named members + shared header */
#include "Animation.h"
void _ZN9Animation17UpdateFileOffsets_ZN9AnimationER8BCA_File(struct Animation *self, void* f) {
  *(char**)((char*)&self->unk_008) = ((char*)self) + self->unk_008;
  *(char**)((char*)&self->unk_00c) = ((char*)self) + self->unk_00c;
  *(char**)((char*)&self->unk_010) = ((char*)self) + self->unk_010;
  *(char**)((char*)&self->unk_014) = ((char*)self) + self->unk_014;
}
