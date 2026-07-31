//cpp
// @symbol _ZN15TextureSequence17UpdateFileOffsetsER8BTP_File
/* recovered: named members + shared header */
#include "TextureSequence.h"
extern "C" void _ZN15TextureSequence17UpdateFileOffsetsER8BTP_File(struct TextureSequence *self) {
  int i = 0;
  if (self->unk_004) self->unk_004 += (int)((char *)self);
  char *p = *(char**)((char *)&self->unk_004);
  while (i < (int)self->unk_002){
    if (*(int*)(p+4)) *(int*)(p+4) += (int)((char *)self);
    i++;
    p += 8;
  }
  if (self->unk_00c) self->unk_00c += (int)((char *)self);
  char *q = *(char**)((char *)&self->unk_00c);
  int j = 0;
  while (j < (int)*(unsigned short*)((char *)&self->unk_008)){
    if (*(int*)(q+4)) *(int*)(q+4) += (int)((char *)self);
    j++;
    q += 8;
  }
  if (self->unk_010) self->unk_010 += (int)((char *)self);
  if (self->unk_014) self->unk_014 += (int)((char *)self);
  if (self->unk_018) self->unk_018 += (int)((char *)self);
  if (self->unk_020) self->unk_020 += (int)((char *)self);
  char *r = *(char**)((char *)&self->unk_020);
  int k = 0;
  while (k < (int)self->unk_01c){
    if (*(int*)(r+4)) *(int*)(r+4) += (int)((char *)self);
    k++;
    r += 0xc;
  }
}
