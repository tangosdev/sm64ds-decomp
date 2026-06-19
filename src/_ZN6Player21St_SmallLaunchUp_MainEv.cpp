//cpp
extern "C" {
struct State;
extern State data_ov002_02110424;
extern void _ZN6Player11ChangeStateERNS_5StateE(void*,State*);
extern void func_ov002_020bedd4(void*);
int _ZN6Player21St_SmallLaunchUp_MainEv(char* c){
  if(*(unsigned char*)(c+0x6de)==0){
    _ZN6Player11ChangeStateERNS_5StateE(c,&data_ov002_02110424);
  }
  func_ov002_020bedd4(c);
  return 1;
}
}
