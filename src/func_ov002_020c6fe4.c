typedef unsigned int u32;
typedef int Fix12i;
typedef unsigned char u8;

extern int _ZNK6Player14GetBodyModelIDEjb(void* thiz, u32 a, u32 b);
extern int _ZNK9Animation12WillHitFrameEi(void* anim, int frame);
extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(u32 a, u32 b, void* v);
extern int _ZN6Player12FinishedAnimEv(void* thiz);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void* thiz, u32 a, int b, Fix12i c, u32 d);
extern u8 PAUSE_LEVEL_CLEAR_SAVE_MENU_ACTIVE;

void func_ov002_020c6fe4(char* c)
{
    int id;

    id = _ZNK6Player14GetBodyModelIDEjb(c, *(u32*)(c + 8) & 0xff, 0);
    if (_ZNK9Animation12WillHitFrameEi(*(void**)(c + id * 4 + 0xdc) + 0x50, 0x58))
        _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(u8*)(c + 0x6d9), 0x35, c + 0x74);

    id = _ZNK6Player14GetBodyModelIDEjb(c, *(u32*)(c + 8) & 0xff, 0);
    if (_ZNK9Animation12WillHitFrameEi(*(void**)(c + id * 4 + 0xdc) + 0x50, 0x75))
        PAUSE_LEVEL_CLEAR_SAVE_MENU_ACTIVE = 1;

    if (_ZN6Player12FinishedAnimEv(c)) {
        _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x47, 0, 0x1000, 0);
        *(u8*)(c + 0x6e5) = 2;
    }
}
