//cpp
extern "C" {
extern int func_ov006_020e6da4(void*);
extern void func_020127ec(int a0,int a1,int a2,int a3,int a4,int a5);
extern unsigned char data_ov006_0212e574[];
void func_ov006_020e6df0(int a0,char* a1,void* a2){
  a1 += data_ov006_0212e574[a0];
  int r=func_ov006_020e6da4(a2);
  func_020127ec(1,(int)a1,4,0,0,r);
}
}
