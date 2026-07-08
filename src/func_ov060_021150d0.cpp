//cpp
struct Actor {
    int DistToCPlayer();
};
extern "C" {
extern int RandomIntInternal(int* seed);
extern int RNG_STATE;

void func_ov060_021150d0(Actor* a) {
    char* c = (char*)a;
    if (*(int*)(c + 0x418) & 2) {
        if (a->DistToCPlayer() < 0x3e8000) {
            if (((unsigned)RandomIntInternal(&RNG_STATE) >> 0x10) % 10 < 4) {
                *(int*)(c + 0x40c) = 9;
            } else if (((unsigned)RandomIntInternal(&RNG_STATE) >> 0x10) % 10 < 8) {
                *(int*)(c + 0x40c) = 8;
            } else {
                *(int*)(c + 0x40c) = 0xf;
            }
            *(short*)(c + 0x400) = 0;
            *(int*)(c + 0x3f8) = 0x1000;
        } else if (((unsigned)RandomIntInternal(&RNG_STATE) >> 0x10) % 10 < 5) {
            *(int*)(c + 0x40c) = 0xd;
        } else {
            *(int*)(c + 0x40c) = 7;
        }
    } else {
        *(int*)(c + 0x40c) = 0xe;
    }
}
}
