//cpp
struct Vector3 { int x, y, z; };
struct Vector3_16 { short x, y, z; };
struct WithMeshClsn { int IsOnGround() const; };
struct ActorBase { void MarkForDestruction(); };
struct Actor : ActorBase {
    static int Spawn(unsigned int, unsigned int, const Vector3&, const Vector3_16*, int, int);
    void SetRanges(int, int, int, int);
};
struct SaveData {
    static int HasPlayerLostCap();
    static void PlayerLoseCap();
};
extern "C" unsigned short DecIfAbove0_Short(unsigned short* p);
extern "C" void func_ov002_020b6fcc(char* c);
extern int STAR_CAP_MIN_POS_Y;

extern "C" int func_ov002_020b7e1c(char* self);
int func_ov002_020b7e1c(char* self) {
    if (*(int*)(self + 0xc8) == 0 && !SaveData::HasPlayerLostCap()) {
        if (*(unsigned short*)(self + 0x100) == 0 && ((WithMeshClsn*)(self + 0x144))->IsOnGround()) {
            if (Actor::Spawn(0x10d, *(int*)(self + 8) | 0x12, *(Vector3*)(self + 0x5c), 0, *(signed char*)(self + 0xcc), -1)) {
                *(unsigned short*)(self + 0x100) = 3;
            }
        }
        if (DecIfAbove0_Short((unsigned short*)(self + 0x100)) == 1) {
            ((ActorBase*)self)->MarkForDestruction();
        }
        return 1;
    }
    if (*(int*)(self + 0xc8) == 0) {
        ((Actor*)self)->SetRanges(0x32000, 0x32000, 0x1000000, 0x1000000);
    }
    func_ov002_020b6fcc(self);
    if (STAR_CAP_MIN_POS_Y > *(int*)(self + 0x60)) {
        SaveData::PlayerLoseCap();
        ((ActorBase*)self)->MarkForDestruction();
    }
    return 1;
}
