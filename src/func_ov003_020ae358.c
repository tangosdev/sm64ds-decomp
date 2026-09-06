// NONMATCHING: 8/230 at exact size 0x398. The previous draft here was 0x394 -- four
// bytes SHORT, so it was an incomplete reconstruction, not a near-miss; this body is
// size-exact and every divergence is register/emission order:
//   * +0xfc/+0x100 the two hoisted pool addresses (&data_020a0e40, &data_020a0de8) are
//     loaded in the opposite ORDER; the registers themselves are the ROM's.
//   * +0x11c/+0x124 the two address temps `c + i` and &data_020a0de8[data_020a0e40]
//     swap r2 and r3, and that swap follows the load order above.
// Naming either address (a `p = (unsigned char *)(c + i)` cursor, with or without a
// named global index) costs 52 more words: both temps must stay anonymous. Per
// notes/mwccarm-codegen.md 6bu lever 2 the hoisted-constant emission order is a property
// of the region and no source edit permutes it.
extern void func_02012790(int a);
extern int _ZN8SaveData19IsCharacterUnlockedEj(unsigned int i);
extern int func_ov003_020adec0(char* c, unsigned int r6);

extern unsigned char data_020a0e40;
extern unsigned char data_020a0de8[][4];
extern unsigned char data_020a0de9[][4];
extern unsigned char data_020a0dea[][4];
extern unsigned char data_020a0deb[][4];
extern int data_0208ee44;
extern unsigned char data_0209caa0[];
extern unsigned char data_02092128[];
extern unsigned char data_02092114[];
extern unsigned short data_020a0e58;
extern unsigned short data_020a0e5a[][2];

void func_ov003_020ae358(char* c)
{
    int idx = data_020a0e40;
    int unlocked = 0;
    int i;
    if (data_020a0de8[idx][0] != 0) {
        unlocked = (data_020a0de9[idx][0] != 0);
    }
    if (unlocked == 0) goto sect2;
    
    if (((unsigned char)(data_020a0dea[idx][0] - 0x58) < 0x50) &&
        ((unsigned char)(data_020a0deb[idx][0] - *(unsigned char*)(c + 0x12b) + 0x28) < 0x50)) {
        *(unsigned char*)(c + 0x133) = 2;
        *(unsigned char*)(c + 0x132) = 3;
        *(unsigned char*)(c + 0x118) = (unsigned char)(data_0208ee44 * 6);
        *(unsigned char*)(c + 0x139) = 1;
        *(unsigned char*)(c + 0x119) = 0x10;
        func_02012790(data_0209caa0[0x41] + 0x3c);
        return;
    }

    if (*(unsigned char*)(c + 0x130) <= 1) return;
    if (data_0209caa0[0x41] != 3) return;
    for (i = 0; i < 3; i++) {
        if (_ZN8SaveData19IsCharacterUnlockedEj(i) != 0) {
            unsigned char *rec = data_020a0de8[data_020a0e40];
            if ((unsigned short)(rec[2] - *(unsigned char*)(c + i + 0x124) + 0x18) < 0x30) {
                if ((unsigned short)(rec[3] - *(unsigned char*)(c + i + 0x128) + 0x18) < 0x2b) {
                    *(unsigned char*)(c + 0x133) = 1;
                    *(unsigned char*)(c + 0x134) = (unsigned char)func_ov003_020adec0(c, i);
                    data_02092128[0] = (unsigned char)i;
                    data_02092114[0] = (unsigned char)i;
                    *(unsigned char*)(c + 0x132) = (unsigned char)i;
                    *(unsigned char*)(c + 0x118) = (unsigned char)(data_0208ee44 * 3);
                    *(unsigned char*)(c + 0x139) = 2;
                    *(unsigned char*)(c + 0x119) = 0x10;
                    func_02012790(data_0209caa0[0x41] + 0x3c);
                    return;
                }
            }
        }
    }
    return;

sect2:
    if (data_0209caa0[0x42] == 0) {
        unsigned short ctrl = data_020a0e58;
        if ((ctrl & 0x30) != 0) {
            unsigned short timer = *(unsigned short *) (c + 0x106);
            unsigned char nr;
            if (timer != 0) {
                *(unsigned short *) (c + 0x106) -= 1;
                return;
            }
            if (*(unsigned char *) (c + 0x135) == 0) {
                return;
            }
            if (*(unsigned char *) (c + 0x133) != 1) {
                return;
            }
            if (*(unsigned char *) (c + 0x130) < 3) {
                return;
            }
            nr = *(unsigned char *) (c + 0x134);
            if (ctrl & 0x20) {
                if (((&data_020a0e58)[1] & 0x20) != 0 || timer == 0) {
                    *(unsigned short *) (c + 0x106) = (data_020a0e5a[idx][0] & 0x20) ? 0x10 : 8;
                    if (*(unsigned char *) (c + 0x134) != 0) {
                        nr = nr - 1;
                    }
                }
            } else if (ctrl & 0x10) {
                if (((&data_020a0e58)[1] & 0x10) != 0 || timer == 0) {
                    *(unsigned short *) (c + 0x106) = (data_020a0e5a[idx][0] & 0x10) ? 0x10 : 8;
                    if (*(unsigned char *) (c + 0x134) != *(unsigned char *) (c + 0x130) - 2) {
                        nr = nr + 1;
                    }
                }
            }
            if (nr == *(unsigned char *) (c + 0x134)) {
                return;
            }
            *(unsigned char *) (c + 0x134) = nr;
            func_02012790(0x12e);
            return;
        }
    }
    *(unsigned short *) (c + 0x106) = 0;
}
