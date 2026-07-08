/* func_ov065_0211a6d0 at 0x0211a6d0 (ov065), size 0x1a0
 * Matched byte-for-byte with mwccarm 1.2/sp2p3.
 * flags: -O4,p -enum int -lang c99 -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc
 */
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;

extern void func_020393c4(int* p, int v);
extern int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void* self, int a, int b);
extern int func_020393bc(int* p);
extern int _Z14ApproachLinearRiii(int* r, int t, int step);
extern u16 DecIfAbove0_Short(u16* p);
extern int RandomIntInternal(int* seed);
extern void _ZN9Animation7AdvanceEv(void* a);
extern void* _ZN5Sound8PlayLongEjjjRK7Vector3j(unsigned int a, unsigned int b, unsigned int cc, void* v, unsigned int d);
extern int func_ov065_0211a550(char* c);

extern u8 LEVEL_SPECIFIC_SETTING;
extern int func_ov065_0211aacc;
extern int RNG_STATE;
extern int data_ov065_0211c0b8[];

int func_ov065_0211a6d0(char* c)
{
    if (LEVEL_SPECIFIC_SETTING == 3) {
        func_020393c4((int*)(c + 0x124), 0);
        _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(c, 0, 0);
    } else {
        if (((*(int*)(c + 0xb0) & 8) ? 1 : 0) == 0) {
            if (func_020393bc((int*)(c + 0x124)) == 0) {
                func_020393c4((int*)(c + 0x124), (int)&func_ov065_0211aacc);
            }

            if (LEVEL_SPECIFIC_SETTING == 2) {
                if (_Z14ApproachLinearRiii((int*)(c + 0x38c), *(int*)(c + 0x390), 0xcc) != 0
                    && DecIfAbove0_Short((u16*)(c + 0x39c)) == 0) {
                    unsigned int r = (u16)((unsigned int)RandomIntInternal(&RNG_STATE) >> 0x10);
                    *(s16*)(c + 0x300 + 0x9c) = (s16)(((int)r % 7) * 0x14 + 0xa);
                    if (r >= 0x7fff) {
                        *(int*)(c + 0x390) = 0x1000;
                    } else {
                        *(int*)(c + 0x390) = -0x1000;
                    }
                }
            } else {
                *(int*)(c + 0x38c) = data_ov065_0211c0b8[LEVEL_SPECIFIC_SETTING];
            }

            *(int*)(c + 0x32c) = *(int*)(c + 0x38c);
            _ZN9Animation7AdvanceEv(c + 0x320);
            if (*(int*)(c + 0x38c) != 0) {
                *(int*)(c + 0x398) = (int)_ZN5Sound8PlayLongEjjjRK7Vector3j(*(int*)(c + 0x398), 3, 0x88, c + 0x74, 0);
            }
        }

        _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(c, 0, 0);
    }

    func_ov065_0211a550(c);
    return 1;
}
