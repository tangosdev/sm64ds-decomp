//cpp
/* _ZN3HUD15RenderCoinCountEv at 0x020fc81c (ov002), size 0x1fc
 * Matched byte-for-byte with mwccarm 1.2/sp2p3.
 * flags: -O4,p -enum int -lang c++ -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc
 */
struct Matrix2x2;
struct OamAttr;

extern "C" {
extern unsigned char data_0209f2d8;
extern unsigned char data_0209f250;
extern unsigned short data_0209f358[];
extern signed char data_0209f2f8;
extern OamAttr* _ZN3OAM7NUMBERSE[];
extern OamAttr _ZN3OAM5TIMESE;
extern OamAttr _ZN3OAM4COINE;
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
    int t = (data_0209f2d8 == 1);
    if (t != false) {
        int sb = 0xf0;
        CalculateDigits(data_0209f358[data_0209f250]);
        for (int i = 2; i >= 0; i--) {
            signed char d = digits[i];
            if (d >= 0) {
                OAM::Render(true, _ZN3OAM7NUMBERSE[d], sb, 2, -1, 1, 0);
                sb -= 9;
            }
        }
        OAM::Render(true, &_ZN3OAM5TIMESE, sb, 0xa, -1, 1, 0);
        OAM::Render(true, &_ZN3OAM4COINE, sb - 0x10, 2, -1, 1, 0);
    } else {
        if (SublevelToLevel(data_0209f2f8) == 0x1d)
            return;
        int i;
        int sb = 0xf0;
        CalculateDigits(data_0209f358[data_0209f250]);
        for (i = 2; i >= 0; i--) {
            signed char d = digits[i];
            if (d >= 0) {
                OAM::Render(false, _ZN3OAM7NUMBERSE[d], sb, 2, -1, 1, 0);
                sb -= 9;
            }
        }
        OAM::Render(false, &_ZN3OAM5TIMESE, sb, 0xa, -1, 1, 0);
        OAM::Render(false, &_ZN3OAM4COINE, sb - 0x10, 2, -1, 1, 0);
    }
}