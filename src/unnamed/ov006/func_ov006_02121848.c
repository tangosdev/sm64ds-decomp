extern void func_ov006_020cd39c(void*);
extern int _Z14ApproachLinearRiii(int*,int,int);
extern int func_ov006_020cd158(void);
extern void func_ov004_020b0a54(void*);
struct P2 { int a,b; };
extern struct P2 data_ov006_0213faa0;
void func_ov006_02121848(unsigned char* c){
  func_ov006_020cd39c(c);
  if(_Z14ApproachLinearRiii((int*)(c+0x5d90),0,1)==0) return;
  if(func_ov006_020cd158()==0) return;
  func_ov004_020b0a54((void*)0x12);
  *(unsigned char*)(c+0xc3)=0;
  *(int*)(c+0x5d90)=0xb4;
  *(struct P2*)(c+0x5004)=data_ov006_0213faa0;
}
