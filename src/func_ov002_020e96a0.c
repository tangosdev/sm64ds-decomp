extern unsigned char data_0209f2d8;
extern signed char data_0209f310[];
extern void GiveVsStars(int idx, int delta);
extern void func_ov002_020e8244(int *out, char *c);
extern void _ZN5Actor11SpawnNumberERK7Vector3jbtPS_(char *c, int *pos, int num, int b, int t, char *p);
extern void _ZN5Actor11UntrackStarERa(char *c, signed char *p);
extern void func_ov002_020e7e58(char *c);
extern void _ZN9ActorBase18MarkForDestructionEv(char *c);
extern void _ZN5Actor24KillAndTrackInDeathTableEv(char *c);
extern void func_ov002_020e8098(char *c);

void func_ov002_020e96a0(char *c)
{
    int v[3];
    char *p;
    int *src;
    unsigned short t;

    *(unsigned short *)(((int)c + 0x490) & 0xFFFFFFFFFFFFFFFF) += 1;
    *(short *)(((int)c + 0x8e) & 0xFFFFFFFFFFFFFFFF) += 0x800;
    t = *(unsigned short *)(c + 0x490);
    if (t >= 0x1e) {
        if (t == 0x1e) {
            GiveVsStars(*(unsigned char *)(*(char **)(c + 0x438) + 0x6d8), 1);
            func_ov002_020e8244(v, c);
            p = *(char **)(c + 0x438);
            _ZN5Actor11SpawnNumberERK7Vector3jbtPS_(c, v, data_0209f310[*(unsigned char *)(p + 0x6d8)], 1, 0, p);
            _ZN5Actor11UntrackStarERa(c, (signed char *)(c + 0x498));
        }
        *(unsigned short *)(((int)c + 0x4a2) & 0xFFFFFFFFFFFFFFFF) &= ~2;
        if (*(unsigned short *)(c + 0x490) < 0x64) return;
        func_ov002_020e7e58(c);
        if ((int)(data_0209f2d8 == 1) != 0) {
            _ZN9ActorBase18MarkForDestructionEv(c);
        } else {
            _ZN5Actor24KillAndTrackInDeathTableEv(c);
        }
    } else {
        p = *(char **)(c + 0x438);
        src = (int *)(((int)p + 0x5c) & 0xFFFFFFFFFFFFFFFF);
        *(int *)(c + 0x5c) = src[0];
        *(int *)(c + 0x60) = src[1];
        *(int *)(c + 0x64) = src[2];
        *(int *)(((int)c + 0x60) & 0xFFFFFFFFFFFFFFFF) += 0x104000;
        func_ov002_020e8098(c);
    }
}
