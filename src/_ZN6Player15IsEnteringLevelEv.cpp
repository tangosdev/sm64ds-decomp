//cpp
extern "C" {
extern int _ZN6Player20IsStateEnteringLevelEv(char*c);
}

struct Player {
    int IsEnteringLevel();
};

int Player::IsEnteringLevel()
{
    char* c = (char*)this;
if(_ZN6Player20IsStateEnteringLevelEv(c)==0) return 0;
  unsigned char s=*(unsigned char*)(c+0x6e3);
  if(s==8||s==9||s==0x10) return 1;
  return 0;

}
