//cpp
extern "C" {
extern void func_ov007_020bfe4c(void *o, int x, int y, int z, void *out);

void func_ov007_020c510c(char *c){
  int neg;
  char *o = *(char**)(c+0xa8);
  neg = -*(int*)(o+0x2c);
  func_ov007_020bfe4c(o, (*(unsigned char*)(o+0xb8))<<0xc, (*(unsigned char*)(o+0xba))<<0xc, neg, c+0xb8);
  func_ov007_020bfe4c(o, (*(unsigned char*)(o+0xb9))<<0xc, (*(unsigned char*)(o+0xbb))<<0xc, neg, c+0xc4);
  {
    int t = *(int*)(c+0xbc);
    *(int*)(c+0xbc) = *(int*)(c+0xc8);
    *(int*)(c+0xc8) = t;
  }
}
}
