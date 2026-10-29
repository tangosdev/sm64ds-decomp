//cpp

namespace CP15 {
void FlushAndInvalidateDataCache(unsigned int addr, unsigned int len);
}
extern "C" {
extern void func_ov007_020be0dc(void *c);
extern void DMASyncFillTransfer(int ch, int dst, int val, int n);
extern int func_ov007_020c0b20(int *a, int b, int cc);
extern void func_ov007_020be3bc(void);

struct O {
  int f0;     /* 0x0 */
  int f4;     /* 0x4 */
  char pad[8];
  int f10;    /* 0x10 */
  int f14;    /* 0x14 */
};

void func_ov007_020bc9c8(struct O *c){
  func_ov007_020be0dc(c);
  DMASyncFillTransfer(3, c->f10, 0xc0, 0x400);
  DMASyncFillTransfer(3, c->f14, 0xc0, 0x400);
  func_ov007_020c0b20((int*)c->f0, c->f10, 0);
  func_ov007_020c0b20((int*)c->f4, c->f14, 0);
  func_ov007_020be3bc();
  CP15::FlushAndInvalidateDataCache(c->f10, 0x400);
  CP15::FlushAndInvalidateDataCache(c->f14, 0x400);
}
}
