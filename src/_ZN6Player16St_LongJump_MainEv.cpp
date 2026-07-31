//cpp
extern "C" {
extern int func_ov002_020e28d4(void*,int,int);
extern int _ZN6Player11ChangeStateERNS_5StateE(void*,void*);
extern int func_ov002_020bedd4(void*);
extern int data_ov002_02110424[];
}

struct Player {
    int St_LongJump_Main();
};

int Player::St_LongJump_Main()
{
    char* c = (char*)this;
func_ov002_020e28d4(c,0x1800,0x800);
  if(*(unsigned char*)(c+0x6de)==0){
    _ZN6Player11ChangeStateERNS_5StateE(c,data_ov002_02110424);
  }
  func_ov002_020bedd4(c);
  return 1;

}
