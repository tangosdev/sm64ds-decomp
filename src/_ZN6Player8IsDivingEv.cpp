//cpp
extern "C" {
extern int data_ov002_021105bc[];
int _ZN6Player7IsStateERNS_5StateE(void*, void*);
}

struct Player {
    int IsDiving();
};

int Player::IsDiving()
{
    void* c = (void*)this;
return _ZN6Player7IsStateERNS_5StateE(c, data_ov002_021105bc) != 0;

}
