//cpp
struct Player;
extern "C" void func_ov002_020c9e40(Player*);
extern "C" void func_02035860(void*, void*);
extern "C" void func_0200cf40(char*);
extern unsigned char CURR_PLAYER_ID;
extern char* CAMERA;

struct Player {
    int dummy;
    unsigned int SetAnim(unsigned int, int, int, unsigned int);
};

extern "C" int _ZN6Player15St_Respawn_InitEv(Player* thiz);
int _ZN6Player15St_Respawn_InitEv(Player* thiz) {
    char* self = (char*)thiz;
    int* ptr_60 = (int*)(self + 0x60);
    int* ptr_380 = (int*)(self + 0x380);
    int* ptr_5c = (int*)(self + 0x5c);
    func_ov002_020c9e40(thiz);
    *(int*)(self + 0x5c) = *(int*)(self + 0x53c);
    *(int*)(self + 0x60) = *(int*)(self + 0x540);
    *(int*)(self + 0x64) = *(int*)(self + 0x544);
    *(int*)(((int)ptr_60) & 0xFFFFFFFFFFFFFFFF) += 0x3e8000;
    func_02035860(((void*)(((int)ptr_380) & 0xFFFFFFFFFFFFFFFF)), ((void*)(((int)ptr_5c) & 0xFFFFFFFFFFFFFFFF)));
    *(int*)(self + 0x68) = *(int*)(self + 0x5c);
    *(int*)(self + 0x6c) = *(int*)(self + 0x60);
    *(int*)(self + 0x70) = *(int*)(self + 0x64);
    *(short*)(self + 0x94) = *(short*)(self + 0x698);
    *(short*)(self + 0x8e) = *(short*)(self + 0x94);
    *(unsigned char*)(self + 0x713) = 1;
    *(unsigned char*)(self + 0x716) = 0;
    if (*(unsigned char*)(self + 0x6d8) == CURR_PLAYER_ID) {
        func_0200cf40(*(char**)((int)&CAMERA & 0xFFFFFFFFFFFFFFFF));
    }
    thiz->SetAnim(0x54, 0x40000000, 0x1000, 0);
    *(unsigned char*)(self + 0x6e3) = 0;
    *(int*)(self + 0x684) = *(int*)(self + 0x60);
    *(int*)(self + 0x98) = 0;
    *(int*)(self + 0xa8) = *(int*)(self + 0x98);
    *(int*)(self + 0x9c) = 0;
    *(short*)(self + 0x6a4) = 0xa;
    return 1;
}
