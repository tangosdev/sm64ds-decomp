extern int data_ov007_02102dc4[];
extern int data_ov007_0210342c[];
extern int func_ov007_020c3df4(int,int);
extern void func_ov007_020b6a84(void);
extern void func_ov007_020b697c(void);
extern void func_ov007_020b69c4(void);
extern void func_ov007_020b693c(void);
extern void func_ov007_020b6d40(void);
void func_ov007_020b68e8(void){
  int r=func_ov007_020c3df4((int)data_ov007_02102dc4,0x188);
  data_ov007_0210342c[0]=r;
  func_ov007_020b6a84();
  func_ov007_020b697c();
  func_ov007_020b69c4();
  func_ov007_020b693c();
  func_ov007_020b6d40();
  int c=data_ov007_0210342c[0];
  *(int*)(c+0x184)=0;
}
