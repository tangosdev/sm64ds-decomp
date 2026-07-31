// @symbol func_ov100_02145370
/* recovered: shared common types */
#include "common.h"
extern void Vec3_Sub(struct Vector3* out, struct Vector3* a, struct Vector3* b);
extern void* Vec3_RotateYAndTranslate(void* out, void* base, short ang, struct Vector3* v);
extern unsigned char data_0209f250[];
extern char* data_0209f394[];
extern int data_020a0ebc;
void* func_ov100_02145370(char* c){
  struct Vector3 v;
  char* r5 = data_0209f394[data_0209f250[0]];
  Vec3_Sub(&v, (struct Vector3*)(r5+0x5c), (struct Vector3*)(c+0x5c));
  Vec3_RotateYAndTranslate(c+0x80, &data_020a0ebc, (short)(-(*(short*)(c+0x8e))), &v);
  return r5;
}
