extern void func_ov007_020c43bc(void *p);
extern void func_ov007_020c4388(char *r0, int r1);

struct Sub {
  int f0;          /* 0x0 */
  int f4;          /* 0x4 */
  int f8;          /* 0x8 */
  char pad[0x30];
  short f3c;        /* not used directly */
};

void func_ov007_020b4b5c(char *c, int arg){
  char *o = *(char**)(c+4);
  int t;
  *(int*)(o+0x40) = 0x1000;
  *(int*)(o+0x3c) = *(int*)(o+0x40);
  func_ov007_020c43bc(o);
  *(int*)(o+0x50) = 0x1f000;
  *(unsigned short*)(o+0x44) = 0x7fff;
  *(int*)(o+4) = ((short*)(*(char**)(*(char**)c + 0x24)))[1] << 0xc;
  *(int*)(o+8) = ((short*)(*(char**)(*(char**)c + 0x24)))[2] << 0xc;
  (void)t;
  func_ov007_020c4388(o, arg);
}
