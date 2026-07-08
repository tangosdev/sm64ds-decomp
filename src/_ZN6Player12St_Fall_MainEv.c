typedef int Fix12i;
extern int func_ov002_020e28d4(void*, int, int);
extern int _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
extern int func_ov002_020e2664(void*);
extern int func_ov002_020bedd4(void*);
extern Fix12i func_ov002_020bf30c(void* c, Fix12i a);
extern char _ZN6Player7ST_LANDE[];
int _ZN6Player12St_Fall_MainEv(void* c) {
  func_ov002_020e28d4(c, 0x1800, 0x800);
  if (*(unsigned char*)((char*)c+0x6de) == 0) {
    _ZN6Player11ChangeStateERNS_5StateE(c, _ZN6Player7ST_LANDE);
  } else if (func_ov002_020e2664(c)) {
    return 1;
  }
  func_ov002_020bedd4(c);
  if (*(int*)((char*)c+0x98) >= func_ov002_020bf30c(c, 0x28000))
    *(int*)((char*)c+0x98) = func_ov002_020bf30c(c, 0x28000);
  return 1;
}
