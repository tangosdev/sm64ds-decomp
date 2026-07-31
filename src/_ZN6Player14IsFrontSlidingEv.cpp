//cpp
extern "C" {
int _ZN6Player6IsAnimEj(void*, unsigned int);
}

struct Player {
    int IsFrontSliding();
};

int Player::IsFrontSliding()
{
    void* c = (void*)this;
return _ZN6Player6IsAnimEj(c, 0x43) || _ZN6Player6IsAnimEj(c, 0x40);

}
