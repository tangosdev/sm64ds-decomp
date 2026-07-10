//cpp
// If the state index at +0x10c is set: clears bit 0x10000000 at +0xb0, zeroes
// the halfword at +0x102, invokes the pointer-to-member-function from
// data_ov002_0210db80[index-1] (forwarding both int args), then writes
// -0x2000 to +0x9c and clears the +0xb0 bit again.
struct C;
typedef void (C::*PMF)(int, int);
extern PMF data_ov002_0210db80[];
struct C {
  char pad0[0x9c];
  int f9c;
  char pad1[0x10];
  int fb0;
  char pad2[0x4e];
  short f102;
  char pad3[8];
  int f10c;
};
extern "C" void func_ov002_020aea30(C* c, int a, int b) {
  if (c->f10c == 0) return;
  (*(unsigned int*)((long long)(int)((char*)c + 0xb0) & 0xFFFFFFFFFFFFFFFFLL)) &= ~0x10000000;
  c->f102 = 0;
  (c->*data_ov002_0210db80[c->f10c - 1])(a, b);
  c->f9c = -0x2000;
  (*(unsigned int*)((unsigned long long)(unsigned int)((char*)c + 0xb0))) &= ~0x10000000;
}
