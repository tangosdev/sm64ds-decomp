extern short* func_ov007_020b8f8c(void);
extern int func_ov007_020c1d78();
extern int* data_ov007_0210342c;
void func_ov007_020af0e4(void){
  short* p = func_ov007_020b8f8c();
  p[1] = 5;
  int* g = data_ov007_0210342c;
  if(*(short*)(*(int*)(g+1)+0xa) != 1)
    *(short*)(*(int*)(g+3)+2) = 5;
  func_ov007_020c1d78(0);
}
