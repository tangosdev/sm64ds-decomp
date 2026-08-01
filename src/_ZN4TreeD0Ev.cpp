//cpp
// @symbol _ZN4TreeD0Ev
/* recovered: named members + shared header */
#include "Tree.h"
extern "C" {
extern int _ZTV4Tree[];
extern int data_020a0eac[];
extern int _ZN5ModelD1Ev[];
void __destroy_arr(void*, int, int, void*);
void _ZN5ActorD2Ev(void*);
void _ZN6Memory10DeallocateEPvP4Heap(void*, void*);
void* _ZN4TreeD0Ev(char* c){
  *(int*)c = (int)_ZTV4Tree;
  __destroy_arr(c+0xd4, 5, 0x50, (void*)_ZN5ModelD1Ev);
  _ZN5ActorD2Ev(c);
  _ZN6Memory10DeallocateEPvP4Heap(c, (void*)data_020a0eac[0]);
  return c;
}
}
