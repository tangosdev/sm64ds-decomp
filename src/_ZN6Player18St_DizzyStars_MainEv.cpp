//cpp
extern "C" {
extern int func_ov002_020d98b4(void*);
extern int data_ov002_0211013c[];
extern int _ZN6Player11ChangeStateERNS_5StateE(void*,void*);
}

struct Player {
    int St_DizzyStars_Main();
};

int Player::St_DizzyStars_Main()
{
    char* c = (char*)this;
func_ov002_020d98b4(c);
  if(*(short*)(c+0x6a4) > 0) return 1;
  _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_0211013c);
  return 1;

}
