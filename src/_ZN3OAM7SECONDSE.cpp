//cpp
struct UnkVtbl {
    virtual void Virtual0();
    virtual void Virtual4();
};

extern "C" {
extern int data_ov006_0213e63c[][2];
extern void* data_ov006_0213e96c[];
extern void** data_ov006_0213e5ec[];
extern unsigned char data_ov006_0213e4d8[];
extern void Hud_RenderSprite(void* a0, int a1, int a2, int a3, int a4);
extern void func_ov004_020b2444(int a1, int a2, int num, int a4, int a5, int sel, int idx);
extern void func_ov004_020af770(void* a0, int a1, int a2, int a3, int a4, int a5, unsigned short a6);
extern int GetGameLanguage(void);
extern void func_ov004_020af868(void* a0, int a1, int a2, int a3, int a4, void* a5);
extern void func_ov006_0210c234(unsigned char* o);

#pragma opt_strength_reduction off
int _ZN3OAM7SECONDSE(unsigned char* t)
{
    int i, pos, x, j;
    unsigned char* cur = t;
    unsigned char b;
    for (i = 0; i < 3; i++) {
        pos = *(t + i + 0x46ff);
        x = data_ov006_0213e63c[i][1] - (*(int*)(t + (i << 2) + 0x46a4) >> 12);
        for (j = 0; j < 2; j++) {
            Hud_RenderSprite(data_ov006_0213e96c[*(cur + pos + 0x46c0)],
                                data_ov006_0213e63c[i][0], x, -1, 3);
            pos--;
            x += 0x40;
            if (pos < 0)
                pos += 0x15;
        }
        cur += 0x15;
    }
    ((UnkVtbl*)(t + 0x4660))->Virtual4();
    func_ov004_020b2444(0x70, 0xb0, *(int*)(t + 0xa8), 0, 1, 1, 0x14);
    if (*(int*)(t + 0x46b4) >= 5 && (*(t + 0x470a) != 0 || (*(t + 0x470b) != 0 && *(t + 0x470b) < 3))) {
        b = *(t + 0x470b);
        if (b != 0 && b < 3) {
            i = 0;
            if ((int)b > 0) {
                pos = 0x18;
                do {
                    func_ov004_020af770(data_ov006_0213e96c[0xb], pos, 0x30, -1, 2, 0x1000, 0);
                    pos += 0x10;
                    i++;
                } while (i < *(t + 0x470b));
            }
            func_ov004_020af868(data_ov006_0213e5ec[GetGameLanguage()][1], 0x50, 0x30, -1, 2, 0);
            func_ov004_020b2444(0x60, 0x30, *(t + 0x470b) * 2, 0, 2, 2, 0x14);
        }
        if (*(t + 0x470a) != 0) {
            i = 0;
            pos = 0x18;
            for (; i < 3; i++) {
                func_ov004_020af770(data_ov006_0213e96c[*(t + 0x4709) + 6], pos, 0x40, -1, 2, 0x1000, 0);
                pos += 0x10;
            }
            func_ov004_020af868(data_ov006_0213e5ec[GetGameLanguage()][1], 0x50, 0x40, -1, 2, 0);
            func_ov004_020b2444(0x60, 0x40, data_ov006_0213e4d8[*(t + 0x4709)], 0, 2, 2, 0x14);
        }
    }
    func_ov006_0210c234(t + 0x4684);
    func_ov006_0210c234(t + 0x4690);
    return 1;
}
}
