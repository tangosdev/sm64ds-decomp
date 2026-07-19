typedef unsigned char u8;
typedef signed char s8;

extern s8 data_02092110;
extern s8 data_02092118;
extern s8 data_0209211c;
extern u8 data_0209f268;
extern u8 data_0209f220;
extern u8 data_0209f1f0;
extern u8 data_0209f26c;
extern s8 data_0209f2f8;
extern u8 data_0209f200;
extern u8 data_0209f270;
extern u8 data_0209f24c;
extern unsigned short data_0209f5e8;

extern void _ZN5Scene20SetAndStopColorFaderEv(void);
extern void SetNextStar(void);
extern int SublevelToLevel(int i);

void LoadLevel(s8 levelID, u8 entranceID, s8 starID, u8 arg3, s8 returnState)
{
    int cur;
    int chk;

    if (data_02092110 < 0) {
        _ZN5Scene20SetAndStopColorFaderEv();
        *(unsigned short *)((char *)&data_0209f5e8 + 0xc) = 0;
    }
    data_02092110 = levelID;
    data_0209f268 = entranceID;
    if (starID < 0)
        data_0209f1f0 = data_0209f220;
    else
        data_0209f1f0 = (u8)starID;
    SetNextStar();
    data_0209f26c = arg3;
    if (returnState < 0)
        goto no_return;
    data_0209211c = data_0209f2f8;
    data_0209f200 = (u8)returnState;
    chk = data_02092118;
    if (chk < 0)
        return;
    cur = SublevelToLevel(data_02092110);
    chk = SublevelToLevel(chk);
    if (cur != chk) {
        data_02092118 = -1;
        return;
    }
    data_02092110 = data_02092118;
    data_0209f268 = data_0209f24c;
    data_0209f1f0 = data_0209f270;
    return;
no_return:
    cur = SublevelToLevel(data_0209f2f8);
    if (data_0209211c < 0)
        goto done;
    if (cur == 6 || cur == 7 || cur == 0xb) {
        data_02092118 = data_02092110;
        data_0209f24c = data_0209f268;
        data_0209f270 = data_0209f1f0;
        return;
    }
done:
    data_02092118 = -1;
}
