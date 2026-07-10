//cpp
struct Plat;
typedef void (Plat::*PMF)();
extern "C" PMF data_ov095_02137910[];

extern "C" void* _ZN5Actor13ClosestPlayerEv(void* c);
extern "C" void _ZN8Platform21UpdateModelPosAndRotYEv(void* c);
extern "C" int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void* c, int a, int b);
extern "C" void _ZN8Platform19UpdateClsnPosAndRotEv(void* c);
extern "C" int _ZN6Player7IsInAirEv(void* c);

typedef unsigned char u8;
typedef unsigned short u16;

extern "C" int _ZN13UpDownLiftBbh8BehaviorEv(char* c)
{
    int old;
    *(void**)(c + 0x324) = _ZN5Actor13ClosestPlayerEv(c);
    old = *(int*)(c + 0x32c);
    (((Plat*)c)->*data_ov095_02137910[old])();
    *(u16*)(((int)c + 0x344) & 0xFFFFFFFFFFFFFFFF) += 1;
    if (old != *(int*)(c + 0x32c)) *(u16*)((c + 0x300) + 0x44) = 0;
    _ZN8Platform21UpdateModelPosAndRotYEv(c);
    if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(c, 0, 0) != 0)
        _ZN8Platform19UpdateClsnPosAndRotEv(c);
    if (*(int*)(c + 0x32c) == 0 || (unsigned)(*(int*)(c + 0x32c) - 3) <= 1) {
        void* pl = *(void**)(c + 0x320);
        if (pl != 0) {
            if (_ZN6Player7IsInAirEv(pl) == 0) {
                if (*(u8*)(c + 0x348) == 0) {
                    *(int*)(c + 0x320) = 0;
                    *(u8*)(c + 0x347) = 1;
                }
            }
        }
    }
    *(u8*)(c + 0x348) = 0;
    if (*(void**)(c + 0x324) != 0)
        *(int*)(c + 0x330) = *(int*)(*(char**)(c + 0x324) + 0x60);
    return 1;
}
