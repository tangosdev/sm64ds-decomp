//cpp
// _ZN5Stage12RenderNumberEhiibi at 0x020250d0
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
struct OamAttr;
struct Matrix2x2;

class OAM {
public:
    static void Render(bool sub, OamAttr *data, int x, int y, int a, int b, Matrix2x2 *mtx);
};

class Stage {
public:
    static void RenderNumber(unsigned char num, int x, int y, bool b, int p5);
};

extern "C" int __aeabi_idiv(int, int);
extern unsigned char data_020755b8[];
extern OamAttr *func_020aba70[];

void Stage::RenderNumber(unsigned char num, int x, int y, bool b, int p5)
{
    int leading = 0;
    Matrix2x2 *mtx = 0;
    int i;

    for (i = 0; i < 3; i++)
    {
        int digit = __aeabi_idiv(num, data_020755b8[i]);
        if (digit != 0 || leading != 0 || i == 2)
        {
            if (b == 0)
            {
                OAM::Render(true, func_020aba70[digit], x, y, p5, -1, mtx);
            }
            else
            {
                OAM::Render(false, func_020aba70[digit], x, y, p5, -1, 0);
            }
            leading = 1;
            x += 9;
        }
        num = num % data_020755b8[i];
    }
}
