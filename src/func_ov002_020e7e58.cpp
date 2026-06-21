//cpp
struct Actor;
struct ActorBase {
    void MarkForDestruction();
};

struct Actor {
    static Actor *FindWithID(unsigned int);
};

extern "C" void func_ov002_020e7e58(unsigned char *self)
{
    if (self[0x49c] == 0) return;
    unsigned int id = *(unsigned int *)(self + 0x430);
    if (id == 0) return;
    Actor *found = Actor::FindWithID(id);
    if (found != 0) {
        if (self[0x49c] == 1) {
            *(unsigned short *)((char *)found + 0xde) = 0;
        } else {
            ((ActorBase *)found)->MarkForDestruction();
        }
    }
    *(unsigned int *)(self + 0x430) = 0;
}
