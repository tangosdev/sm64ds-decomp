extern int func_ov007_020c3d40(int, int);
void func_ov007_020b72fc(char *c, int n){
  *(int*)(c+0x38) = n;
  *(int*)(c+0x3c) = func_ov007_020c3d40(0, n<<2);
}
