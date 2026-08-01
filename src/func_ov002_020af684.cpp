//cpp
struct Actor;
extern "C" Actor* _ZN5Actor15FindWithActorIDEjPS_(unsigned int actorID, Actor* prev);
extern "C" void GiveCoins(int idx, int amount);
extern "C" void _ZN6Player4HealEi(void* p, int amt);
extern "C" void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int id, void* pos);
extern "C" void _ZN5Actor24KillAndTrackInDeathTableEv(void* self);

extern "C" void func_ov002_020af684(char* self, int target, char* player){
    char* p = player;
    Actor* found = 0;
    for (;;) {
        found = _ZN5Actor15FindWithActorIDEjPS_(*(unsigned short*)(self + 0xc), found);
        if (found == 0)
            break;
        if (target == *(int*)((char*)found + 0x384)) {
            (*(int *)(((int)found + 0x390)))--;
            break;
        }
    }
    if ((unsigned int)(*(int*)(self + 0x384) - 0xb) <= 1) {
        GiveCoins(*(unsigned char*)(p + 0x6d8), 1);
        _ZN6Player4HealEi(p, 0x100);
        if (*(unsigned char*)(p + 0x706))
            _ZN5Sound9PlayBank3EjRK7Vector3(0x12, self + 0x74);
        else
            _ZN5Sound9PlayBank3EjRK7Vector3(0x11, self + 0x74);
    }
    _ZN5Actor24KillAndTrackInDeathTableEv(self);
}
