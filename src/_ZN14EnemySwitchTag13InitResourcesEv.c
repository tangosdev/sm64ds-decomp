typedef unsigned int u32;
typedef signed char s8;
typedef unsigned char u8;

extern int IsStarCollectedInLevel(s8 levelID, int starID);
extern s8 data_0209f2f8;
extern u8 data_0209f220;
extern u8 data_0209f2d8;
extern int data_0209caa0[];
extern int data_0209fc48;
extern int data_ov002_02110aec;

int _ZN14EnemySwitchTag13InitResourcesEv(char* c)
{
    int flag;

    if (data_0209f2f8 == 8 && (data_0209f220 == 1 || IsStarCollectedInLevel(8, 1) == 0))
        return 0;

    *(int*)(c + 8) &= 0xf;

    if ((int)(data_0209f2d8 == 0) != 0
        && (data_0209caa0[2] & 0x80) == 0
        && (int)(data_0209fc48 != 0) == 0)
        flag = 1;
    else
        flag = 0;

    data_ov002_02110aec = flag;
    return 1;
}
