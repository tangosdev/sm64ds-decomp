//cpp
extern "C" {
int func_ov007_020c11ac(void*);
void* func_ov007_020c3df4(int,int);
void func_ov007_020c4090(void*);
void* func_ov007_020c40d4(void* r5){
  short* r4=(short*)func_ov007_020c3df4(0,0xc);
  *(int*)(r4)=func_ov007_020c11ac(r5);
  r4[3]=0;
  r4[2]=r4[3];
  *((char*)r4+0xa)=0;
  func_ov007_020c4090(r4);
  return r4;
}
}
