extern int func_ov007_020c3d40(int, int);
void func_ov007_020b7320(char *c, int a, int b){
  *(int*)(c+0x20) = a;
  *(int*)(c+0x28) = b;
  *(int*)(c+0x24) = func_ov007_020c3d40(0, a<<2);
  *(int*)(c+0x2c) = func_ov007_020c3d40(0, b<<2);
}
