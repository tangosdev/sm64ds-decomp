//cpp
extern "C" {
extern int _ZN5Event6GetBitEj(unsigned int bit);
extern void func_02012790(int id);
extern int _ZN6Player7IsInAirEv(void *p);
extern void GiveLives(int n);

extern unsigned char data_0209f250;
extern unsigned char data_0209f2c4;
extern unsigned char data_0209f20c;
extern unsigned char data_0209f294;
extern short data_02092144[];
extern char *data_0209f394[];
extern int data_0208ee44;
extern unsigned char data_ov002_0211117c;
extern unsigned char data_ov002_02111178;

void _ZN3HUD17UpdateHealthMeterEv(char *c) {
    int chr = data_0209f250;
    char *player = data_0209f394[chr];
    unsigned int health = (data_02092144[chr] >> 8) & 0xff;
    if ((unsigned char)(data_0209f2c4 | data_0209f20c | data_0209f294) != 0)
        return;
    if (*(unsigned short *)(c + 0x6a) != 0)
        *(unsigned short *)((((int)c) + 0x6a) & 0xFFFFFFFFFFFFFFFFLL) -= data_0208ee44;
    if (*(unsigned short *)(c + 0x6c) != 0)
        *(unsigned short *)((((int)c) + 0x6c) & 0xFFFFFFFFFFFFFFFFLL) -= data_0208ee44;
    if (_ZN5Event6GetBitEj(0x1d)) {
        unsigned char s = *(unsigned char *)(c + 0x73);
        if (s != 0) {
            if (s < 5)
                *(unsigned char *)(c + 0x73) = 5;
        }
    }
    switch (*(unsigned char *)(c + 0x73)) {
    case 0:
        if (*(unsigned char *)(player + 0x706) != 0) {
            *(unsigned short *)(c + 0x68) = 0x39;
            *(unsigned char *)(c + 0x73) = 1;
            return;
        }
        if (data_ov002_0211117c <= health)
            return;
        *(unsigned short *)(c + 0x68) = 0x39;
        data_ov002_0211117c = *(volatile unsigned char *)&data_ov002_0211117c - 1;
        *(unsigned short *)(c + 0x6c) = 0x1e;
        *(unsigned short *)(c + 0x6a) = 0x5a;
        *(unsigned char *)(c + 0x73) = 1;
        return;
    case 1:
        if (*(unsigned short *)(c + 0x6a) == 0) {
            if (*(short *)(c + 0x68) != 0x19) {
                *(short *)((((int)c) + 0x68) & 0xFFFFFFFFFFFFFFFFLL) -= 4;
                if (*(short *)(c + 0x68) <= 0x19)
                    *(short *)(c + 0x68) = 0x19;
            }
        }
        if (*(unsigned short *)(c + 0x6c) != 0)
            return;
        {
            unsigned char cur = data_ov002_0211117c;
            if (cur < health) {
                if ((int)(health - cur) > 1) {
                    data_ov002_0211117c = cur + 1;
                    func_02012790(0xd);
                    *(unsigned short *)(c + 0x6c) = 0x10;
                    return;
                }
                data_ov002_0211117c = health;
                func_02012790(0xd);
                if (*(unsigned char *)(player + 0x706) != 0)
                    return;
                if (health != 8)
                    return;
                if (data_ov002_02111178 == 1) {
                    data_ov002_02111178 = 3;
                    *(unsigned char *)(c + 0x73) = 2;
                } else {
                    *(unsigned short *)(c + 0x6a) = 0x3c;
                    *(unsigned char *)(c + 0x73) = 4;
                }
                return;
            }
            if (cur > health) {
                data_ov002_0211117c = cur - 1;
                *(unsigned short *)(c + 0x6c) = 0x1e;
                return;
            }
            if (cur != 8)
                return;
            if (*(unsigned char *)(player + 0x706) != 0)
                return;
            if (_ZN6Player7IsInAirEv(player))
                return;
            if (data_ov002_02111178 == 1) {
                data_ov002_02111178 = 3;
                *(unsigned char *)(c + 0x73) = 2;
            } else {
                *(unsigned short *)(c + 0x6a) = 0x3c;
                *(unsigned char *)(c + 0x73) = 4;
            }
        }
        return;
    case 2:
        if (data_ov002_02111178 == 4) {
            *(unsigned short *)(c + 0x6a) = 0x3c;
            *(unsigned char *)(c + 0x73) = 3;
        }
        return;
    case 3:
        if (*(unsigned short *)(c + 0x6a) != 0)
            return;
        GiveLives(-1);
        func_02012790(0x40);
        data_ov002_02111178 = 5;
        *(unsigned short *)(c + 0x6a) = 0x3c;
        *(unsigned char *)(c + 0x73) = 4;
        return;
    case 4:
    case 5:
        if (*(unsigned short *)(c + 0x6a) != 0)
            return;
        *(short *)((((int)c) + 0x68) & 0xFFFFFFFFFFFFFFFFLL) -= 4;
        if (*(short *)(c + 0x68) >= -0x18)
            return;
        *(short *)(c + 0x68) = -0x18;
        if (*(unsigned char *)(c + 0x73) == 4)
            *(unsigned char *)(c + 0x73) = 0;
        else
            *(unsigned char *)(c + 0x73) = 6;
        return;
    case 6:
        if (_ZN5Event6GetBitEj(0x1d) == 0)
            *(unsigned char *)(c + 0x73) = 1;
        return;
    }
}
}
