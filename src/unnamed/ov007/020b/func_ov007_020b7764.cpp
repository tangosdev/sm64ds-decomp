//cpp
namespace Player { void St_EndingFly_Main(); }
extern "C" {
extern int data_ov007_02103448;
void func_ov007_020b3edc(int);
void func_ov007_020b7764(void){
  func_ov007_020b3edc(0x17);
  if(data_ov007_02103448==0) return;
  Player::St_EndingFly_Main();
  data_ov007_02103448=0;
}
}
