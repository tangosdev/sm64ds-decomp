//cpp
extern "C" {
void* _ZN8dActor_c13ClosestPlayerEv(void*);
int _ZN8dActor_c13DistToCPlayerEv(void*);
int func_ov100_021467e8(char* c){
  if (_ZN8dActor_c13ClosestPlayerEv(c)) {
    int d = _ZN8dActor_c13DistToCPlayerEv(c);
    if (d < 0x1770000) return d;
  }
  *(int*)(c+0x14c) = 2;
  return 2;
}
}
