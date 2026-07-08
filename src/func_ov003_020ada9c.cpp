//cpp
extern "C" {
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

extern void UnloadArchives(void);
extern void Enable3dEngines(void);
extern void func_020233f4(void);
extern void func_ov003_020ad6ec(int);

extern u8 font_2D_ncg_bin[];
extern u8 font_2D_ncl_bin[];
extern u8 debug_level_select_arrow_ncg_bin[];
extern u8 TOP_SCREEN_RELATED[];
extern u8 GAME_SPEED_RELATED[];
extern u8 data_0209ee90[];
}

namespace GX {
    void DisableAllBanks();
    void SetBankForBG(unsigned short);
    void SetBankForOBJ(unsigned short);
    void SetGraphicsMode(int, int, int);
    void LoadBG0Char(void const *, unsigned int, unsigned int);
    void LoadBGPltt(void const *, unsigned int, unsigned int);
    void LoadOBJ(void const *, unsigned int, unsigned int);
    void LoadOBJPltt(void const *, unsigned int, unsigned int);
}
namespace G2 { unsigned short *GetBG0ScrPtr(); }
namespace Sound { void LoadInitialGroup(int); void LoadAndSetMusic_Layer1(int); }

extern "C" int func_ov003_020ada9c(int arg)
{
    UnloadArchives();
    Enable3dEngines();
    GX::DisableAllBanks();
    GX::SetBankForBG(8);
    GX::SetBankForOBJ(1);
    *(volatile u32*)0x4000000 &= 0xffcfffef;
    GX::SetGraphicsMode(1, 0, 0);
    *(volatile u32*)0x4001000 |= 0x10000;
    func_020233f4();
    *(volatile u16*)0x4000008 &= ~3;
    *(volatile u16*)0x4000008 = (*(volatile u16*)0x4000008 & 0x43) | 0x1f00;
    *(volatile u16*)0x4000008 &= ~0x40;
    GX::LoadBG0Char(font_2D_ncg_bin, 0, 0x2000);
    GX::LoadBGPltt(font_2D_ncl_bin, 0, 0x200);
    {
        u16 *scr = G2::GetBG0ScrPtr();
        int i = 0;
        do { *scr++ = 0xff; i++; } while (i < 0x1800);
    }
    GX::LoadOBJ(debug_level_select_arrow_ncg_bin, 0, 0x2000);
    GX::LoadOBJPltt(font_2D_ncl_bin, 0, 0x200);
    *(u8*)TOP_SCREEN_RELATED = 0x11;
    *(volatile u32*)0x4000000 = (*(volatile u32*)0x4000000 & ~0x1f00) | 0x1100;
    *(int*)GAME_SPEED_RELATED = 2;
    func_ov003_020ad6ec(arg);
    *(volatile u32*)0x4000010 = 0;
    *(u8*)(data_0209ee90 + 0x340) = 0;
    *(u8*)(data_0209ee90 + 0x341) = 0;
    Sound::LoadInitialGroup(2);
    Sound::LoadAndSetMusic_Layer1(0x38);
    return 1;
}
