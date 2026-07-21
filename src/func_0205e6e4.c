#pragma opt_loop_invariants off
typedef unsigned char u8;

struct Obj {
  char pad00[0x14];
  int f14;       // 0x14
  int f18;       // 0x18
  int f1c;       // 0x1c
  u8 buf[0x40];  // 0x20
  int f60;       // 0x60
  int f64;       // 0x64
};

extern void func_0205e3d4(struct Obj*);

int func_0205e6e4(struct Obj* obj, u8* p, int count) {
  if (count == 0) return 0;
  if (obj == 0 || p == 0) return 1;
  if (obj->f60 != 0) { obj->f64 = 3; return 3; }
  if (obj->f64 != 0) return obj->f64;
  goto test;

body:
  {
    int i = obj->f1c;
    *(int *)(((int)obj + 0x1c) & 0xFFFFFFFFFFFFFFFF) += 1;
    obj->buf[i] = *p;
  }
  *(int *)(((int)obj + 0x14) & 0xFFFFFFFFFFFFFFFF) += 8;
  if (obj->f14 == 0) {
    *(int *)(((int)obj + 0x18) & 0xFFFFFFFFFFFFFFFF) += 1;
    if (obj->f18 == 0) obj->f64 = 1;
  }
  if (obj->f1c == 0x40) func_0205e3d4(obj);
  p++;

test:
  if (count-- == 0) goto ret0;
  if (obj->f64 != 0) goto ret0;
  goto body;
ret0:
  return 0;
}
