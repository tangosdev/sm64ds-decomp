//cpp
#include "Tree.h"
extern "C" {
extern void* _ZN7fBase_cnwEj(unsigned int);
extern void _ZN8dActor_cC2Ev(void*);
extern void _ZN5ModelD1Ev(void*);
extern void _ZN5ModelC1Ev(void*);
extern void func_020733a8(void* arr, int count, int size, void(*ctor)(void*), void(*dtor)(void*));
extern void* _ZTV4Tree[];
int* Tree_Spawn(void){
  int* p = (int*)_ZN7fBase_cnwEj(sizeof(struct Tree));
  if(p){
    _ZN8dActor_cC2Ev(p);
    *(void***)p = (void**)_ZTV4Tree;
    func_020733a8((char*)p+0xd4, 5, 0x50, _ZN5ModelC1Ev, _ZN5ModelD1Ev);
  }
  return p;
}
}
