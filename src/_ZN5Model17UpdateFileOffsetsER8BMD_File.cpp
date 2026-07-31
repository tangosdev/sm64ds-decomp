//cpp
// @symbol _ZN5Model17UpdateFileOffsetsER8BMD_File
/* recovered: named members + shared header */
#include "Model.h"
extern "C" void _ZN5Model17UpdateFileOffsetsER8BMD_File(struct Model *self) {
  unsigned int i;
  int k;
  if (self->unk_008) self->unk_008 += (int)((char *)self);
  for (i = 0; i < self->unk_004; i++) {
    char *b = *(char **)((char *)&self->unk_008) + i * 0x40;
    if (*(int *)(b + 4)) *(int *)(b + 4) += (int)((char *)self);
    if (*(int *)(b + 0x34)) *(int *)(b + 0x34) += (int)((char *)self);
    if (*(int *)(b + 0x38)) *(int *)(b + 0x38) += (int)((char *)self);
  }
  if (self->unk_010) self->unk_010 += (int)((char *)self);
  for (i = 0; i < self->unk_00c; i++) {
    char *m = *(char **)((char *)&self->unk_010) + i * 8;
    if (*(int *)(m + 4)) *(int *)(m + 4) += (int)((char *)self);
    for (k = 0; k < *(int *)m; k++) {
      char *t = *(char **)(m + 4) + k * 0x10;
      if (*(int *)(t + 4)) *(int *)(t + 4) += (int)((char *)self);
      if (*(int *)(t + 0xc)) *(int *)(t + 0xc) += (int)((char *)self);
    }
  }
  if (self->unk_018) self->unk_018 += (int)((char *)self);
  for (i = 0; i < self->unk_014; i++) {
    char *e = *(char **)((char *)&self->unk_018) + i * 0x14;
    if (*(int *)e) *(int *)e += (int)((char *)self);
    if (*(int *)(e + 4)) *(int *)(e + 4) += (int)((char *)self);
  }
  if (self->unk_020) self->unk_020 += (int)((char *)self);
  for (i = 0; i < self->unk_01c; i++) {
    char *e = *(char **)((char *)&self->unk_020) + i * 0x10;
    if (*(int *)e) *(int *)e += (int)((char *)self);
    if (*(int *)(e + 4)) *(int *)(e + 4) += (int)((char *)self);
  }
  if (self->unk_028) self->unk_028 += (int)((char *)self);
  for (i = 0; i < self->unk_024; i++) {
    char *e = *(char **)((char *)&self->unk_028) + i * 0x30;
    if (*(int *)e) *(int *)e += (int)((char *)self);
  }
  if (self->unk_02c) self->unk_02c += (int)((char *)self);
  if (self->unk_030 == 0) return;
  if (self->unk_034) self->unk_034 += (int)((char *)self);
  {
    char *e = *(char **)((char *)&self->unk_034);
    if (*(int *)e) *(int *)e += (int)((char *)self);
    if (*(int *)(e + 4)) *(int *)(e + 4) += (int)((char *)self);
    if (*(int *)(e + 8)) *(int *)(e + 8) += (int)((char *)self);
    if (*(int *)(e + 0xc)) *(int *)(e + 0xc) += (int)((char *)self);
  }
}
