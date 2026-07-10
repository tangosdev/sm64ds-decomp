//cpp
extern "C" {
extern int _ZTV4Tree[];
extern int data_020a0eac[];
extern int _ZN5ModelD1Ev[];
void func_0207328c(void*, int, int, void*);
void _ZN5ActorD1Ev(void*);
void _ZN6Memory10DeallocateEPvP4Heap(void*, void*);
void* _ZN4TreeD0Ev(char* c){
  *(int*)c = (int)_ZTV4Tree;
  func_0207328c(c+0xd4, 5, 0x50, (void*)_ZN5ModelD1Ev);
  _ZN5ActorD1Ev(c);
  _ZN6Memory10DeallocateEPvP4Heap(c, (void*)data_020a0eac[0]);
  return c;
}
}
