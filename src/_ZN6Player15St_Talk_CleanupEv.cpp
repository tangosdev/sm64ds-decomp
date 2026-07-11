//cpp
extern "C" {
typedef unsigned int u32;
typedef unsigned char u8;

struct Player {
    char _pad0[0xb0];
    u32 flagB0;            /* 0xb0 */
    char _pad1[0x368 - 0xb4];
    void* field368;        /* 0x368 */
    char _pad2[0x725 - 0x36c];
    u8 flag725;             /* 0x725 */
};

struct Obj154 { char pad[0x154]; u32 f154; };

extern u32 data_0209b454;
extern Obj154* data_0209f318;

extern void _ZN5Sound22LoadAndSetMusic_Layer1Ei(int);

int _ZN6Player15St_Talk_CleanupEv(Player* this_)
{
    void* p = this_->field368;
    if (p) {
        *(u32*)(((long long)(int)((char*)p + 0xb0)) & 0xFFFFFFFFFFFFFFFFLL) &= ~0x800000;
        this_->field368 = 0;
    }
    *(u32*)(((long long)(int)((char*)this_ + 0xb0)) & 0xFFFFFFFFFFFFFFFFLL) &= ~0x800000;
    data_0209b454 &= ~0x800000;
    *(u32*)(((long long)(int)((char*)data_0209f318 + 0x154)) & 0xFFFFFFFFFFFFFFFFLL) &= ~8;
    if (this_->flag725) {
        _ZN5Sound22LoadAndSetMusic_Layer1Ei(0x39);
        this_->flag725 = 0;
    }
    return 1;
}
}
