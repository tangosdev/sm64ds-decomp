//cpp
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

extern "C" {
    int func_0203da3c(void);
    int func_0206045c(void);
    int func_02057020(void);
    void func_0205ff80(u16 a);
    void func_0205ff70(u16 a);
    void func_02057078(u16 lockID);
    int func_02060484(int a, int b, int c, int d, int e, int f);
}

extern u8 data_020a4b40[8];

extern "C" int _ZN8SaveData14SaveDataToCartEPcjj(char* data, u32 size, u32 fileID)
{
    int lockID;
    u16 sum = 0;
    int freeVal;
    int retry;
    int v14, v18, v10;
    int hdrOff, dataOff;
    int need;
    int newBase;
    int v20, v24, v1c;
    int hdrOff2, dataOff2;
    int retry2;

    if (func_0203da3c() == 2)
        return 1;

    freeVal = (int)((u32)func_0206045c() >> 1);
    need = (int)(fileID << 7);
    if ((u32)freeVal <= (u32)need)
        return 1;
    if (size > (u32)(freeVal - need - 10))
        return 1;

    sum = 0;
    {
        int i = 0;
        u16 acc = 0;
        const u8* t = data_020a4b40;
        do {
            u8 c = *t;
            i++;
            acc = (u16)(acc + c);
            t++;
        } while (i < 8);
        sum = acc;
    }

    {
        u32 i;
        for (i = 0; i < size; i++) {
            sum = (u16)(((sum << 1) & 0xfffe) | ((sum >> 15) & 1));
            sum = (u16)(sum ^ (((u8)data[i]) & 0xff));
        }
    }

    lockID = func_02057020();
    if (lockID == -3)
        return 1;

    func_0205ff80((u16)lockID);

    retry = 0;
    v14 = 0;
    v18 = 0;
    v10 = 0;
    hdrOff = need + 2;
    dataOff = need + 10;
    goto body1;

inc1:
    retry++;
    if (retry <= 0)
        goto body1;
    func_0205ff70((u16)lockID);
    func_02057078((u16)lockID);
    return 1;

body1:
    if (func_02060484(need, (int)&sum, 2, v10, v10, v10) != 1)
        goto inc1;
    if (func_02060484(hdrOff, (int)data_020a4b40, 8, v14, v14, v14) != 1)
        goto inc1;
    if (func_02060484(dataOff, (int)data, (int)size, v18, v18, v18) != 1)
        goto inc1;

    func_0205ff70((u16)lockID);
    func_0205ff80((u16)lockID);

    retry2 = 0;
    newBase = freeVal + need;
    v20 = 0;
    v24 = 0;
    v1c = 0;
    hdrOff2 = newBase + 2;
    dataOff2 = newBase + 10;
    goto body2;

inc2:
    retry2++;
    if (retry2 <= 0)
        goto body2;
    func_0205ff70((u16)lockID);
    func_02057078((u16)lockID);
    return 1;

body2:
    if (func_02060484(newBase, (int)&sum, 2, v1c, v1c, v1c) != 1)
        goto inc2;
    if (func_02060484(hdrOff2, (int)data_020a4b40, 8, v20, v20, v20) != 1)
        goto inc2;
    if (func_02060484(dataOff2, (int)data, (int)size, v24, v24, v24) != 1)
        goto inc2;

    func_0205ff70((u16)lockID);
    func_02057078((u16)lockID);
    return 0;
}
