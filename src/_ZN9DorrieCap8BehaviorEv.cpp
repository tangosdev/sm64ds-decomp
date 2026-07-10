//cpp
// NONMATCHING: base materialization / addressing (div=39). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern "C" {
extern void* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned int, unsigned int, void*, void*, int, int);
extern void func_ov001_020ab110(void*);
extern void func_ov001_020ab228(void*, void*, int, int, int);
extern void func_ov065_02118c4c(char* c);
extern int func_ov065_021180d4(void*);
extern void _ZN12CylinderClsn5ClearEv(void*);
extern void _ZN12CylinderClsn6UpdateEv(void*);
int _ZN9DorrieCap8BehaviorEv(char* c){
  char* f = *(char**)(c + 0x174);
  int b;
  if (f == 0) return 1;
  b = (*(int*)(c + 0xb0) & 0x20000) != 0;
  if (b) {
    char* a = (char*)_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0x10d, 0x1210, c + 0x5c, c + 0x8c, *(signed char*)(c + 0xcc), -1);
    if (a != 0) {
      char* ab0 = a + 0xb0;
      char* cb0 = c + 0xb0;
      *(int*)(a + 0xd0) = *(int*)(c + 0xd0);
      *(int*)(*(char**)(c + 0xd0) + 0x360) = (int)a;
      *(int*)ab0 = *(int*)ab0 | 0x20000;
      *(int*)cb0 = *(int*)cb0 & ~0x20000;
      *(int*)(c + 0xd0) = 0;
      func_ov001_020ab110(c + 0xd4);
      func_ov001_020ab228(c + 0xd4, c, 2, 0, 0);
    }
    return 1;
  } else {
    int* src = (int*)(f + 0xd8);
    *(int*)(c + 0x5c) = src[0];
    *(int*)(c + 0x60) = src[1];
    *(int*)(c + 0x64) = src[2];
    *(short*)(c + 0x8c) = *(short*)(*(char**)(c + 0x174) + 0xe4);
    *(short*)(c + 0x8e) = *(short*)(*(char**)(c + 0x174) + 0x8e);
    if ((*(unsigned char*)(c + 0xef) >> 1) & 1) {
      func_ov065_02118c4c(c);
      if (func_ov065_021180d4(c) != 0) {
        _ZN12CylinderClsn5ClearEv(c + 0x140);
        return 1;
      }
      _ZN12CylinderClsn5ClearEv(c + 0x140);
      _ZN12CylinderClsn6UpdateEv(c + 0x140);
    }
    return 1;
  }
}
}
