// NONMATCHING: near-miss from DB (div=134)
extern void func_02012790(int a);
extern int _ZN8SaveData19IsCharacterUnlockedEj(unsigned int i);
extern int func_ov003_020adec0(char* c, unsigned int r6);

extern unsigned char data_020a0e40[];
extern unsigned char data_020a0de8[];
extern unsigned char data_020a0de9[];
extern unsigned char data_020a0dea[];
extern unsigned char data_020a0deb[];
extern int data_0208ee44;
extern unsigned char data_0209caa0[];
extern unsigned char data_02092128[];
extern unsigned char data_02092114[];
extern unsigned short data_020a0e58;
extern unsigned short data_020a0e5a[];

void func_ov003_020ae358(char* c)
{
    unsigned char idx = data_020a0e40[0];
    unsigned char unlocked = 0;
    int i;
    if (data_020a0de8[idx * 4] != 0) {
        unlocked = (data_020a0de9[idx * 4] != 0);
    }
    if (unlocked == 0) goto sect2;

    if (((unsigned char)(data_020a0dea[idx * 4] - 0x58) < 0x50) &&
        ((unsigned char)(data_020a0deb[idx * 4] - *(unsigned char*)(c + 0x12b) + 0x28) < 0x50)) {
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
            unsigned char id2 = data_020a0e40[0];
            unsigned char ax = data_020a0de8[id2 * 4 + 2];
            int dx = (unsigned short)(ax - *(unsigned char*)(c + 0x124 + i) + 0x18);
            if (dx < 0x30) {
                unsigned char ay = data_020a0de8[id2 * 4 + 3];
                int dy = (unsigned short)(ay - *(unsigned char*)(c + 0x128 + i) + 0x18);
                if (dy < 0x2b) {
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
    if (data_0209caa0[0x42] != 0) return;
    {
        unsigned short ctrl = data_020a0e58;
        if ((ctrl & 0x30) == 0) return;
        if (*(unsigned short*)(c + 0x106) != 0) {
            *(unsigned short*)(c + 0x106) -= 1;
            return;
        }
        if (*(unsigned char*)(c + 0x135) == 0) return;
        if (*(unsigned char*)(c + 0x133) != 1) return;
        if (*(unsigned char*)(c + 0x130) < 3) return;
        {
            unsigned char r3 = *(unsigned char*)(c + 0x134);
            unsigned char cur;
            if (ctrl & 0x20) {
                unsigned short c2 = data_020a0e58;
                if ((c2 & 0x20) == 0) {
                    if (*(unsigned short*)(c + 0x106) == 0) {
                        if ((data_020a0e5a[idx * 2] & 0x20) != 0) {
                            *(unsigned short*)(c + 0x106) = 0x10;
                        } else {
                            *(unsigned short*)(c + 0x106) = 8;
                        }
                        if (*(unsigned char*)(c + 0x134) != 0) {
                            r3 = (unsigned char)(r3 - 1);
                        }
                    }
                }
            } else if (ctrl & 0x10) {
                unsigned short c2 = data_020a0e58;
                if ((c2 & 0x10) == 0) {
                    if (*(unsigned short*)(c + 0x106) == 0) {
                        if ((data_020a0e5a[idx * 2] & 0x10) != 0) {
                            *(unsigned short*)(c + 0x106) = 0x10;
                        } else {
                            *(unsigned short*)(c + 0x106) = 8;
                        }
                        cur = *(unsigned char*)(c + 0x134);
                        if (cur != *(unsigned char*)(c + 0x130) - 2) {
                            r3 = (unsigned char)(r3 + 1);
                        }
                    }
                }
            }
            cur = *(unsigned char*)(c + 0x134);
            if (r3 == cur) return;
            *(unsigned char*)(c + 0x134) = r3;
            func_02012790(0x12e);
        }
    }
}
