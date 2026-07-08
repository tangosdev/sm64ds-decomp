//cpp
// NONMATCHING: register allocation (div=35). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern "C" {
void _ZN9ModelBase12ApplyOpacityEj(void* m, unsigned int op);
void func_ov002_020f65b8(void* o);
}
struct M48 { int w[12]; };
extern M48 INV_VIEW_MATRIX_ASR_3;
extern int data_ov002_0210bb7c[];
typedef void (*VFN)(void*, int);
extern "C" int func_ov002_020f80ac(char* c){
  if (*(int*)(c + 8) == 0x19){
    char* p = *(char**)(c + 0xdc) + 0x1c;
    int* tbl = data_ov002_0210bb7c;
    int i;
    *(M48*)p = INV_VIEW_MATRIX_ASR_3;
    for (i = 0; i < 3; i++){
      *(int*)(p + 0x24) = tbl[0];
      *(int*)(p + 0x28) = tbl[1];
      *(int*)(p + 0x2c) = tbl[2];
      void* obj = *(void**)(c + 0xdc);
      (*(VFN*)(*(char**)obj + 0x14))(obj, 0);
      tbl += 3;
    }
    return 1;
  }
  if (*(unsigned char*)(c + 0x102) == 0) return 1;
  if (*(void**)(c + 0xdc) != 0){
    _ZN9ModelBase12ApplyOpacityEj(*(void**)(c + 0xdc), 0);
    void* obj = *(void**)(c + 0xdc);
    (*(VFN*)(*(char**)obj + 0x14))(obj, (int)(c + 0x80));
  } else if (*(void**)(c + 0xe0) != 0){
    func_ov002_020f65b8(c);
    _ZN9ModelBase12ApplyOpacityEj(*(void**)(c + 0xe0), *(unsigned char*)(c + 0x102));
    void* obj = *(void**)(c + 0xe0);
    (*(VFN*)(*(char**)obj + 0x14))(obj, (int)(c + 0x80));
  }
  return 1;
}
