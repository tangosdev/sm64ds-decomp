extern unsigned char data_020a0e40[];
extern unsigned char data_020a0de8[];
extern unsigned char data_020a0de9[];
extern unsigned char data_020a0deb[];
extern unsigned char data_020a0dea[];

extern void func_ov006_020def80(char* c, int i);
extern void FreeGfxSlotsById(int arg);
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int id);

#pragma opt_common_subs off
#pragma opt_strength_reduction off
void func_ov006_020df3bc(char* c)
{
    int flag = 0;
    unsigned int idx = data_020a0e40[0];
    int j;
    unsigned char thA;
    unsigned char thB;

    if (data_020a0de8[idx * 4] != 0 && data_020a0de9[idx * 4] != 0) flag = 1;
    if (flag == 0) return;

    {
        unsigned char* thP = &data_020a0de8[idx * 4];
        thA = thP[2];
        thB = thP[3];
    }

    for (j = 0; j < 3; j++) {
        int dx = thA - (*(int*)(c + j * 8 + 0x5000 + 0x3e8) >> 12);
        int dy = thB - (*(int*)(c + j * 8 + 0x5000 + 0x3ec) >> 12);
        if (dx > 0x10 || dx < -0x28 || dy > 0x18 || dy < -0x20) continue;

        *(unsigned char*)(c + j + 0x5000 + 0x465) = 1;
        func_ov006_020def80(c, j);

        if (*(unsigned char*)(c + 0x5000 + 0x468) == *(unsigned char*)(c + j + 0x5000 + 0x462)) {
            *(unsigned char*)(c + 0x5000 + 0x469) = 1;
            *(int*)(c + j * 4 + 0x5000 + 0x434) = 6;
        } else {
            *(unsigned char*)(c + 0x5000 + 0x469) = 0;
        }

        *(int*)(c + 0x5000 + 0x418) = 5;
        *(int*)(c + 0x5000 + 0x41c) = 0x3c;
        FreeGfxSlotsById(0x1d);

        {
            unsigned int idx2 = data_020a0e40[0];
            unsigned char a1 = *(volatile unsigned char*)&data_020a0deb[idx2 * 4];
            unsigned char a2 = *(volatile unsigned char*)&data_020a0dea[idx2 * 4];
            *(int*)(c + 0x5000 + 0xdc) = 1;
            *(int*)(c + 0x5000 + 0xd4) = a2;
            *(int*)(c + 0x5000 + 0xd8) = a1;
        }
        _ZN5Sound12PlayBank2_2DEj(0x1cd);
        return;
    }
}
