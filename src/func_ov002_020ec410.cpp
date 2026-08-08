//cpp
struct Actor {
    static Actor* FindWithID(unsigned int id);
};
struct Player {
    bool CanWarp();
};
struct CylinderClsn {
    void Clear();
    void Update();
};
extern "C" int IsPlayerWarping(int a0);
extern "C" void WarpPlayer(int i, int val);

extern "C" int func_ov002_020ec410(char* c)
{
    unsigned int id = *(unsigned int*)(c + 0xf8);
    if (id != 0) {
        if (*(short*)(c + 0x8e) == 0) {
            Actor* a = Actor::FindWithID(id);
            if (a != 0) {
                int isPlayer = (*(unsigned short*)((char*)a + 0xc) == 0xbf) ? 1 : 0;
                if (isPlayer != 0) {
                    if (IsPlayerWarping(*(unsigned char*)((char*)a + 0x6d8)) != 0) {
                        *(short*)(c + 0x8e) = 1;
                    } else if (((Player*)a)->CanWarp()) {
                        unsigned int param = (*(unsigned int*)(c + 8) >> 0xc) + 1;
                        WarpPlayer(*(unsigned char*)((char*)a + 0x6d8), param & 0xff);
                    }
                }
            }
        }
    } else {
        *(short*)(c + 0x8e) = 0;
    }

    ((CylinderClsn*)(c + 0xd4))->Clear();
    ((CylinderClsn*)(c + 0xd4))->Update();
    return 1;
}
