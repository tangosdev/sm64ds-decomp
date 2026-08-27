extern int func_ov002_020c14b8(void*);
extern int func_ov002_020c231c(void*);
extern void func_02035798(void*, int);
extern int func_ov002_020c25a8(void*, int);
extern int func_ov002_020c1234(void*);
extern int func_ov002_020c0fb4(void*);
extern int func_ov002_020c5d60(void*);
extern int func_ov002_020c61ac(void*);
extern int func_ov002_020eea84(void*, void*);
extern int func_ov002_020c2b08(void*);
extern int func_ov002_020c29d4(void*);
void func_ov002_020c2db8(unsigned char* c) {
  func_ov002_020c14b8(c);
  c[0x6e9] = 0;
  if (c[0x716] != 0) return;
  c[0x70e] = 0;
  if (*(int*)(c+0x658) == 5) c[0x70e] = 1;
  if (func_ov002_020c231c(c) == 0) {
    func_02035798(c+0x380, 0x1000);
    func_ov002_020c25a8(c, 0);
  } else {
    func_02035798(c+0x380, 0);
    func_ov002_020c25a8(c, 1);
  }
  func_ov002_020c1234(c);
  func_ov002_020c0fb4(c);
  func_ov002_020c5d60(c);
  func_ov002_020c61ac(c);
  func_ov002_020eea84(c+0x380, c);
  func_ov002_020c2b08(c);
  func_ov002_020c29d4(c);
}
