//cpp
#include "Butterfly.h"
#include "common.h"
extern "C" {
extern int RandomIntInternal(int* seed);
extern char* _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned int a, unsigned int b, Vector3* v, Vector3_16* w, int e, int f);
extern int data_0209e650;
}

void Butterfly::State0(){
  char* c = (char*)this;
  struct StateRotation { u16 w[3]; };
  StateRotation rot;
  int n;
  int i;
  n = (*(int*)(c+8) & 0xf) - 1;
  rot = *(StateRotation*)(c+0x92);
  i = 0;
  if (n > 0) {
    do {
    Vector3 pos;
    int r;
    r = RandomIntInternal(&data_0209e650);
    pos.x = ((int)((unsigned int)r % 20) - 0xa) * 0xa000 + *(int*)(c+0x5c);
    r = RandomIntInternal(&data_0209e650);
    pos.y = *(int*)(c+0x60) + ((int)((unsigned int)r % 40) << 13);
    r = RandomIntInternal(&data_0209e650);
    pos.z = ((int)((unsigned int)r % 20) - 0xa) * 0xa000 + *(int*)(c+0x64);
    r = RandomIntInternal(&data_0209e650);
    rot.w[1] = (s16)((s16)rot.w[1] + ((r << 1) >> 16));
    _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(0x150, 0, &pos, (Vector3_16*)&rot, *(signed char*)(c+0xcc), -1);
      i++;
    } while (i < n);
  }
  *(int*)(c+0x3e4) = 1;
}
