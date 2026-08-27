extern int data_0209fc4c;
extern int data_02089608;
extern int data_0209f2d8;
extern int data_0209caa0[];
extern int data_02087c00;
extern int data_0209fc48;

extern void _ZN5Sound16LoadInitialGroupEi(int);
extern void CollectStarInLevel(signed char levelID, int starID);
extern void DisableSoundPlayersForCredits(void);
extern void RunKuppaScript(int);

int ContinueKuppaScriptIfNecessary(void)
{
    int g = data_0209fc4c;
    if (g == 0) return 0;
    if (g == (int)&data_02089608) {
        *(unsigned char *)&data_0209f2d8 = 2;
        _ZN5Sound16LoadInitialGroupEi(0x26);
        data_0209caa0[1] &= ~0x204;
        CollectStarInLevel(7, 1);
        CollectStarInLevel(8, 1);
    } else if (g == (int)&data_02087c00) {
        DisableSoundPlayersForCredits();
    }
    data_0209fc48 = 0;
    RunKuppaScript(data_0209fc4c);
    data_0209fc4c = 0;
    return 1;
}
