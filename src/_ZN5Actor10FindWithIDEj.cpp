//cpp
// @symbol _ZN5Actor10FindWithIDEj
/* recovered: named members + shared header */
#include "Actor.h"
extern "C" {
struct Actor;
extern struct Actor* data_0209b468;
extern struct Actor* func_02043f98(struct Actor**, unsigned int);
struct Actor* _ZN5Actor10FindWithIDEj(unsigned int id){
  struct Actor* p = func_02043f98(&data_0209b468, id);
  if(p) return *(struct Actor**)((char*)p+8);
  return 0;
}
}
