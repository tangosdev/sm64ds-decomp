//cpp
extern "C" {
struct State;
extern State data_ov002_0211046c;
extern int _ZN6Player7IsStateERNS_5StateE(void* c, State* st);
}

struct Player {
    int HasFinishedTalking();
};

int Player::HasFinishedTalking()
{
    char* c = (char*)this;
if(_ZN6Player7IsStateERNS_5StateE(c,&data_ov002_0211046c) && *(unsigned char*)(c+0x6e3)==3){
    *(unsigned char*)(c+0x6e3)=4;
    return 1;
  }
  return 0;

}
