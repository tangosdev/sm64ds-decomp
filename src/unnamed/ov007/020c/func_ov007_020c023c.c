extern char* func_ov007_020c3df4(int,int);
extern int func_ov007_020c8f64(void*);
extern int func_ov007_020c908c(void*);
extern void func_ov007_020c0128(void*);
char* func_ov007_020c023c(void){
  char* r4=func_ov007_020c3df4(0,0xd8);
  *(int*)(r4+0x38)=func_ov007_020c8f64(r4+8);
  *(int*)(r4+0x3c)=func_ov007_020c8f64(r4+0x14);
  *(int*)(r4+0x40)=func_ov007_020c908c(r4+0x2c);
  func_ov007_020c0128(r4);
  return r4;
}
