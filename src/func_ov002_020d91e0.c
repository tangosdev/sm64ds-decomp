typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef int s32;
typedef unsigned int u32;

extern void func_ov002_020da9d4(void* p);
extern int _ZN8SaveData16HasPlayerLostCapEv(void);
extern void _ZN6Player4HealEi(void* thiz, int amt);
extern int _ZN6Player9GetHealthEv(void* thiz);

extern u8 CURRENT_GAMEMODE;
extern char data_0209ee90[];

int func_ov002_020d91e0(char* thiz, int damage, int doPre)
{
    if (doPre != 0) func_ov002_020da9d4(thiz);

    if (_ZN8SaveData16HasPlayerLostCapEv()) {
        if (*(u8*)(thiz + 0x6ff) == 0) damage = damage + (damage >> 1);
    }

    {
        int b = (int)(CURRENT_GAMEMODE == 1);
        if (b != 0) return 0;
    }

    if (*(int*)(data_0209ee90 + 0x248) == 0) {
        _ZN6Player4HealEi(thiz, -damage);
    }
    if (_ZN6Player9GetHealthEv(thiz) != 0) return 0;

    _ZN6Player4HealEi(thiz, -0x100);
    return 1;
}
