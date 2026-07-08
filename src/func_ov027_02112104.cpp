//cpp
extern "C" {
extern int data_ov036_02113b50[];
extern int data_ov056_02112158[];
extern int _ZN6Memory11gameHeapPtrE[];
void func_0207328c(void*, int, int, void*);
void _ZN5ActorD1Ev(void*);
void _ZN6Memory10DeallocateEPvP4Heap(void*, void*);
void* func_ov027_02112104(char* c){
  *(int*)c = (int)data_ov036_02113b50;
  func_0207328c(c+0xd4, 0x32, 0x60, (void*)data_ov056_02112158);
  _ZN5ActorD1Ev(c);
  _ZN6Memory10DeallocateEPvP4Heap(c, (void*)_ZN6Memory11gameHeapPtrE[0]);
  return c;
}
}
