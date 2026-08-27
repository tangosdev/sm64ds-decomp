//cpp
struct W2 { int w[2]; };
extern "C" {
extern void func_ov004_020ae274(int);
extern W2 data_ov004_020bca24;
extern W2 data_ov004_020bc8cc;
void func_ov004_020b7594(char* c){
  *(int*)(c+0x1c)=0xb4;
  func_ov004_020ae274(1);
  *(W2*)(c+0x8)=data_ov004_020bca24;
  *(W2*)(c+0x10)=data_ov004_020bc8cc;
}
}
