//cpp
// @symbol func_ov027_02112618
/* recovered: shared common types */
#include "common.h"
extern "C" {
void* _ZN5Actor13ClosestPlayerEv(void*);
void Vec3_Asr(void* d, void* s, int sh);

void MulVec3Mat4x3(void* in, void* m, void* out);
void Vec3_LslInPlace(void* v, int sh);
extern struct Matrix4x3 data_020a0e68;
int func_ov027_02112618(char* c){
  void* p = _ZN5Actor13ClosestPlayerEv(c);
  *(void**)(c+0x13c4) = p;
  int v[3];
  Vec3_Asr(v, (char*)p+0x5c, 3);
  data_020a0e68 = *(struct Matrix4x3*)(c+0x1394);
  int out[3];
  MulVec3Mat4x3(v, &data_020a0e68, out);
  Vec3_LslInPlace(out, 3);
  if(out[0] < -0xe1000) goto fail;
  if(out[0] > 0xe1000) goto fail;
  if(out[1] < -0x5000) goto fail;
  if(out[1] > 0x300000) goto fail;
  if(out[2] < -0x430000) goto fail;
  if(out[2] > 0x430000) goto fail;
  return 1;
fail:
  return 0;
}
}
