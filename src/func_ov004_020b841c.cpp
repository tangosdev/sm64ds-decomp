//cpp
struct Obj {
  virtual void v0(); virtual void v1(); virtual void v2();
  virtual int v3(int a, int b);            /* +0xc */
  virtual int v4(int a, int b);            /* +0x10 */
  virtual int v5();                        /* +0x14 */
  virtual int v6();                        /* +0x18 */
  virtual int v7();                        /* +0x1c */
};
struct Obj2 {
  char pad[0x4c];
  int m(int a);                            /* virtual at +0x4c */
};
struct Obj2v {
  virtual void a0(); virtual void a1(); virtual void a2(); virtual void a3();
  virtual void a4(); virtual void a5(); virtual void a6(); virtual void a7();
  virtual void a8(); virtual void a9(); virtual void a10(); virtual void a11();
  virtual void a12(); virtual void a13(); virtual void a14(); virtual void a15();
  virtual void a16(); virtual void a17(); virtual void a18();
  virtual int m18(int a);                  /* +0x4c */
};
extern "C" {
extern Obj* data_0209f5bc;
extern Obj2v* data_ov004_020beb68;
extern void func_ov004_020b0aa0(int arg);
extern void func_ov004_020b29a0(void* c, int arg);

void func_ov004_020b841c(char* c) {
  if (data_0209f5bc->v6() != 0) {
    Obj2v* r5 = data_ov004_020beb68;
    if (r5 == 0) return;
    if (r5->m18(*(int*)(c + 0x18)) == 0) return;
    data_0209f5bc->v3(0x1e, 0);
    func_ov004_020b0aa0(0x1d);
    func_ov004_020b29a0(r5, *(int*)(c + 0x18));
    *(int*)(c + 0x1c) = 0;
    *(int*)(c + 0x18) = -1;
    *(int*)(c + 0x20) = 0;
    *(int*)(c + 0x24) = 0;
  } else {
    if (*(int*)(c + 0x1c) != 0) return;
    if (data_0209f5bc->v7() != 0) return;
    data_0209f5bc->v4(0x1e, 0);
  }
}
}
