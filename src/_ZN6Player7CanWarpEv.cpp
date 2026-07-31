//cpp
extern "C" {
struct State;
extern State _ZN6Player7ST_WAITE;
extern State data_ov002_021102a4;
extern int _ZN6Player7IsStateERNS_5StateE(void* c, State* st);
}

struct Player {
    int CanWarp();
};

int Player::CanWarp()
{
    void* c = (void*)this;
if(_ZN6Player7IsStateERNS_5StateE(c, &_ZN6Player7ST_WAITE) || _ZN6Player7IsStateERNS_5StateE(c, &data_ov002_021102a4)) return 1;
  return 0;

}
