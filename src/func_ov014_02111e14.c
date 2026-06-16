extern int DecIfAbove0_Short(void*);
extern int func_ov014_02111ebc(void*, int);
extern int ApproachLinear(int*, int, int);
void func_ov014_02111e14(char* c){
  *(int*)(c+0xa8)=0;
  ApproachLinear((int*)(c+0x80), 0x1000, 0x500);
  *(int*)(c+0x88)=*(int*)(c+0x80);
  *(int*)(c+0x84)=*(int*)(c+0x88);
  if(DecIfAbove0_Short(c+0x5fc)!=0) return;
  func_ov014_02111ebc(c, 1);
}
