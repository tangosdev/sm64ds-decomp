extern int* func_ov007_020c3df4(int,int);
extern void func_ov007_020c05e4(int*);
int* func_ov007_020c0604(int a, int b){
  int* o = func_ov007_020c3df4(0,0x10);
  func_ov007_020c05e4(o);
  o[0]=a;
  o[1]=b;
  return o;
}
