//cpp
extern "C" {
extern int data_ov002_021104b4[];
extern int data_ov002_0211025c[];
extern int _ZN6Player7IsStateERNS_5StateE(char*c, void*s);
}

struct Player {
    int IsStateEnteringLevel();
};

int Player::IsStateEnteringLevel()
{
    char* c = (char*)this;
if(_ZN6Player7IsStateERNS_5StateE(c, data_ov002_021104b4)) return 1;
  return _ZN6Player7IsStateERNS_5StateE(c, data_ov002_0211025c);

}
