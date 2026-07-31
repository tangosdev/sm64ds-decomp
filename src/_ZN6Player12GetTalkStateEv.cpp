//cpp
extern "C" {
struct State;
extern State data_ov002_0211046c;
extern int _ZN6Player7IsStateERNS_5StateE(void* c, State* st);
}

struct Player {
    int GetTalkState();
};

int Player::GetTalkState()
{
    char* c = (char*)this;
if(!_ZN6Player7IsStateERNS_5StateE(c,&data_ov002_0211046c)) return -1;
  unsigned char v=*(unsigned char*)(c+0x6e3);
  if(v==0) return 0;
  if(v==3) return 2;
  if(v==5||v==7) return 3;
  return 1;

}
