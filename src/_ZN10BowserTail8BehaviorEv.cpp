//cpp
struct Actor { static Actor* FindWithID(unsigned int); };
extern "C" void func_ov060_02115b84(void* c);
extern short data_02082214[];

extern "C" int _ZN10BowserTail8BehaviorEv(char* c)
{
    Actor* a = Actor::FindWithID(*(unsigned int*)(c + 0x108));
    if (!a) return 1;

    int ang = *(short*)((char*)a + 0x94);
    a = (Actor*)(((int)a + 0x5c) & 0xFFFFFFFFFFFFFFFFull);
    int x = *(int*)a;
    volatile int v[3];
    v[0] = x;
    v[1] = ((int*)a)[1];
    v[2] = ((int*)a)[2];
    int j = 2 * (((unsigned short)(short)(ang + 0x8000)) >> 4);
    *(int*)(c + 0x5c) = (short)data_02082214[j] * 0x8c + x;
    *(int*)(c + 0x60) = v[1];
    *(int*)(c + 0x64) = (short)data_02082214[j + 1] * 0x8c + v[2];

    func_ov060_02115b84(c);
    return 1;
}
