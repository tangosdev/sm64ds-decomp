//cpp
extern "C" {
extern int func_ov100_02145014(void);
extern int _ZN5Sound7PlaySubEjjj5Fix12IiEb(unsigned int, unsigned int, unsigned int, int, int);
}
extern "C" int func_ov100_02144fcc(void) {
  int r = func_ov100_02145014();
  if (r == 0) return 0;
  return _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x24, 0x7f, 0, 0x7222, 0);
}
