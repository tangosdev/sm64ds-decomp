//cpp
extern "C" {
extern int _ZN6Player7IsStateERNS_5StateE(void* c, void* st);
extern int data_ov002_02110094[];
}

struct Player {
    int GetHurtState();
};

int Player::GetHurtState()
{
    char* c = (char*)this;
if(_ZN6Player7IsStateERNS_5StateE(c, data_ov002_02110094))
    return *(unsigned char*)(c+0x6e3) & 7;
  return -1;

}
