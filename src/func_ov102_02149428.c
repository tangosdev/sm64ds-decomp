// @symbol func_ov102_02149428
/* recovered: shared common types */
#include "common.h"
extern int func_ov102_02149684(struct Vector3* out, void* b);
extern int _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned int,unsigned int,struct Vector3*,void*,int,int);
extern int _ZN8dActor_c24KillAndTrackInDeathTableEv(void*);
void func_ov102_02149428(char* this){
  struct Vector3 pos;
  func_ov102_02149684(&pos, this);
  _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(0x114, 0, &pos, 0, *(signed char*)(this+0xcc), -1);
  _ZN8dActor_c24KillAndTrackInDeathTableEv(this);
}
