//cpp
extern "C" {
extern int data_ov002_0211037c[];
extern void _ZN6Player11ChangeStateERNS_5StateE(void*,void*);
}

struct Player {
    void BlowAway(short v);
};

void Player::BlowAway(short v)
{
    char* c = (char*)this;
if(*(unsigned char*)(c+0x6f9)) return;
  if(*(unsigned char*)(c+0x6fd)) return;
  *(short*)(c+0x94)=v;
  *(short*)(c+0x8e)=(short)(v+0x8000);
  _ZN6Player11ChangeStateERNS_5StateE(c,data_ov002_0211037c);

}
