//cpp
extern "C" {
int _ZN6Player20St_HoldLight_CleanupEv(char* c){
  char* p = *(char**)(c + 0x358);
  if (p) {
    *(unsigned int*)(((long long)(int)(p + 0xb0)) & 0xFFFFFFFFFFFFFFFFLL) |= 0x4000;
  }
  return 1;
}
}
