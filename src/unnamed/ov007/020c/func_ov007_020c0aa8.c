extern int* func_ov007_020c3df4(int,int);
extern void func_ov007_020c0a44(int*);
int* func_ov007_020c0aa8(int a, int b, int c, short d, unsigned short e){
  int* o = func_ov007_020c3df4(0,0x38);
  func_ov007_020c0a44(o);
  o[4]=a;
  *(short*)((char*)o+0x1c)=d;
  *(short*)((char*)o+0x1e)=e;
  o[0]=b;
  o[1]=c;
  return o;
}
