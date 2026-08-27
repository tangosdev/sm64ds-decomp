extern int func_ov007_020c3d40(int, int);
void func_ov007_020b72d8(char *c, int n){
  *(int*)(c+0x30) = n;
  *(int*)(c+0x34) = func_ov007_020c3d40(0, n<<2);
}
