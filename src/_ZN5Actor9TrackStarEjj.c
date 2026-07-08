struct Actor;
extern int SublevelToLevel(int i);
extern int IsStarCollectedInCurLevel(int i);
extern void SetStarMarker(int i, int v1, int v2);
extern unsigned char STAR_ID[];
extern signed char LEVEL_ID[];
extern int STAR_MARKERS[];

int _ZN5Actor9TrackStarEjj(struct Actor *self, unsigned int a, unsigned int b)
{
    int v = b;
    if (b == 2) {
        if (a != STAR_ID[0]) {
            if (SublevelToLevel(LEVEL_ID[0]) <= 0xe)
                return -1;
        }
        if (IsStarCollectedInCurLevel(a)) v = 3;
    }
    signed char i;
    for (i = 0; i < 0xc; i++) {
        if (STAR_MARKERS[i] == 0) {
            SetStarMarker(i, (int)self, v);
            return i;
        }
    }
    return -1;
}
