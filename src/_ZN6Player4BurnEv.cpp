//cpp
extern "C" {
extern int func_ov002_020d82f0(void*);
extern int data_ov002_021100dc[];
extern void _ZN6Player11ChangeStateERNS_5StateE(void*,void*);
}

struct Player {
    int Burn();
};

int Player::Burn()
{
    char* c = (char*)this;
if(func_ov002_020d82f0(c)==0) return 0;
  if(*(unsigned char*)(c+0x6f9) || *(unsigned char*)(c+0x706)) return 0;
  _ZN6Player11ChangeStateERNS_5StateE(c,data_ov002_021100dc);
  return 1;

}
