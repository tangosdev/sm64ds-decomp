//cpp
/* _ZN3HUD15RenderCoinCountEv at 0x020fc81c (ov002), size 0x1fc
 * Matched byte-for-byte with mwccarm 1.2/sp2p3.
 * flags: -O4,p -enum int -lang c++ -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc
 */
struct Matrix2x2;
struct OamAttr;

extern "C" {
extern unsigned char CURRENT_GAMEMODE;
extern unsigned char CURR_PLAYER_ID;
extern unsigned short NUM_COINS[];
extern signed char LEVEL_ID;
extern OamAttr* func_020aba70[];
extern OamAttr func_020ab9c8;
extern OamAttr func_020abad8;
extern int SublevelToLevel(int i);
}

struct HUD {
    char pad[0x74];
    signed char digits[3];
    void CalculateDigits(unsigned short n);
    void RenderCoinCount();
};

namespace OAM {
    void Render(bool vis, OamAttr* attr, int x, int y, int a, int b, Matrix2x2* m);
}

void HUD::RenderCoinCount()
{
    int t = (CURRENT_GAMEMODE == 1);
    if (t != false) {
        int sb = 0xf0;
        CalculateDigits(NUM_COINS[CURR_PLAYER_ID]);
        for (int i = 2; i >= 0; i--) {
            signed char d = digits[i];
            if (d >= 0) {
                OAM::Render(true, func_020aba70[d], sb, 2, -1, 1, 0);
                sb -= 9;
            }
        }
        OAM::Render(true, &func_020ab9c8, sb, 0xa, -1, 1, 0);
        OAM::Render(true, &func_020abad8, sb - 0x10, 2, -1, 1, 0);
    } else {
        if (SublevelToLevel(LEVEL_ID) == 0x1d)
            return;
        int i;
        int sb = 0xf0;
        CalculateDigits(NUM_COINS[CURR_PLAYER_ID]);
        for (i = 2; i >= 0; i--) {
            signed char d = digits[i];
            if (d >= 0) {
                OAM::Render(false, func_020aba70[d], sb, 2, -1, 1, 0);
                sb -= 9;
            }
        }
        OAM::Render(false, &func_020ab9c8, sb, 0xa, -1, 1, 0);
        OAM::Render(false, &func_020abad8, sb - 0x10, 2, -1, 1, 0);
    }
}