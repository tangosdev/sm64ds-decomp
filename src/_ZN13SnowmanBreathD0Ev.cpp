//cpp
extern "C" {
extern int RotatingPlatformRr_SpawnInfo[];
extern int data_ov056_02112158[];
extern int data_020a0eac[];
void func_0207328c(void*, int, int, void*);
void _ZN5ActorD1Ev(void*);
void _ZN6Memory10DeallocateEPvP4Heap(void*, void*);
void* _ZN13SnowmanBreathD0Ev(char* c){
  *(int*)c = (int)RotatingPlatformRr_SpawnInfo;
  func_0207328c(c+0xd4, 0x32, 0x60, (void*)data_ov056_02112158);
  _ZN5ActorD1Ev(c);
  _ZN6Memory10DeallocateEPvP4Heap(c, (void*)data_020a0eac[0]);
  return c;
}
}
