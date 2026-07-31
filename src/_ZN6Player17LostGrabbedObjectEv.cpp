//cpp
extern "C" {
int _ZN6Player6IsAnimEj(void*, unsigned int);
}

struct Player {
    int LostGrabbedObject();
};

int Player::LostGrabbedObject()
{
    void* c = (void*)this;
return _ZN6Player6IsAnimEj(c, 0x18) || _ZN6Player6IsAnimEj(c, 0x8b);

}
