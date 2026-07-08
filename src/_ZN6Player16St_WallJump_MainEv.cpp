//cpp
extern "C" {
typedef int Fix12i;
extern int func_ov002_020eeca8(void*, void*);
extern int func_ov002_020e28d4(void*, int, int);
extern int _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
extern int _ZN6Player7IsStateERNS_5StateE(void*, void*);
extern int func_ov002_020e2664(void*);
extern int func_ov002_020bedd4(void*);
extern char _ZN6Player7ST_LANDE[];
extern unsigned char data_020a0e40[];
extern unsigned short data_0209f49e[];
extern char _ZN6Player12ST_BACK_FLIPE[];
extern int data_ov002_0211073c[];

int _ZN6Player16St_WallJump_MainEv(void* c) {
  func_ov002_020eeca8((char*)c+0x380, c);
  func_ov002_020e28d4(c, 0x1800, 0x800);
  if (*(unsigned char*)((char*)c+0x6de) == 0) {
    _ZN6Player11ChangeStateERNS_5StateE(c, _ZN6Player7ST_LANDE);
  } else {
    if (*(unsigned short*)((char*)data_0209f49e + data_020a0e40[0]*0x18) & 0x400) {
      if (_ZN6Player7IsStateERNS_5StateE(c, _ZN6Player12ST_BACK_FLIPE)) {
        *(short*)((char*)c+0x94) = *(short*)((char*)c+0x8e);
      }
    }
    if (func_ov002_020e2664(c)) return 1;
    {
      int idx = *(int*)((char*)c+8);
      int* row = &data_ov002_0211073c[idx*2];
      int v = row[1];
      void* p = (char*)c + (v>>1);
      int (*f)(void*);
      if (v & 1) {
        f = *(int(**)(void*))((char*)(*(int**)p) + row[0]);
      } else {
        f = (int(*)(void*))row[0];
      }
      f(p);
    }
  }
  func_ov002_020bedd4(c);
  return 1;
}
}
