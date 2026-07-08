//cpp
// NONMATCHING: different op / idiom (div=31). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern "C" {
int func_ov064_0211616c(void*);
int RandomIntInternal(int* seed);
void* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned, unsigned, void*, void*, int, int);
void* _ZN5Actor10FindWithIDEj(unsigned);
extern int RNG_STATE[];
struct V3 { int x, y, z; };
void func_ov064_02117220(char* c){
  if(func_ov064_0211616c(c) == 0) return;
  struct V3 vec;
  vec.x = *(int*)(c+0x5c);
  vec.y = *(int*)(c+0x60) + 0x136000;
  vec.z = *(int*)(c+0x64);
  unsigned int r = (unsigned int)RandomIntInternal(RNG_STATE);
  short v16[3];
  v16[0] = 0;
  v16[2] = 0;
  v16[1] = (short)(*(short*)(c+0x94) + 0x8000 + ((r >> 8) & 0x3ff));
  void* a = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0x120, 2, &vec, v16, *(signed char*)(c+0xcc), -1);
  if(a != 0){
    *(int*)((char*)a+0x98) = 0xa000;
    *(int*)((char*)a+0xa4) = 0;
    *(int*)((char*)a+0xa8) = 0x50000;
    *(int*)((char*)a+0xac) = 0;
    void* f = _ZN5Actor10FindWithIDEj(*(int*)(c+0x3fc));
    if(f != 0){
      unsigned char* p = (unsigned char*)((char*)f + 0x3fe);
      *p = *p + 1;
    }
  }
}
}
