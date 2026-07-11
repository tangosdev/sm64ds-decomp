typedef unsigned short u16;
typedef signed short s16;

struct Vec3 { int x, y, z; };

extern void func_020393a4(int *p, int v);
extern int func_ov002_020b5ab4(char *c);
extern int _Z14ApproachLinearRiii(int *p, int value, int speed);
extern short data_02082214[];
extern int Vec3_HorzDist(void *a, void *b);
extern short _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
extern void _Z14ApproachLinearRsss(short *p, short value, short speed);
extern void func_ov002_020b5b98(char *c);
extern int _ZN8Platform21IsClsnInRangeOnScreenE5Fix12IiES1_(void *self, int a, int b);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void *self);
extern unsigned short DecIfAbove0_Short(unsigned short *p);

int func_ov002_020b5c4c(char *c)
{
    struct Vec3 tpos;
    int dist;
    short atanRes;
    short angleDiff;
    int j;
    short s, co, tv;
    short val1, val2;

    func_020393a4((int *)(c + 0x124), 0x300000);

    if (func_ov002_020b5ab4(c) != 0) {
        if (_Z14ApproachLinearRiii((int *)(c + 0x330),
                *(void **)(c + 0x33c) != 0 ? -0x28000 : 0, 0x5000) != 0) {
            short *ctr = (short *)((long long)(int)(c + 0x338) & 0xFFFFFFFFFFFFFFFFLL);
            short cval = *ctr;
            char *st = c + 0x300;
            *ctr = (short)(cval + 0xa00);
            j = (*(unsigned short *)(st + 0x38) >> 4) * 2;
            tv = data_02082214[j];
            *(int *)(c + 0x334) = 10 * tv;
        }

        if (*(void **)(c + 0x33c) == 0) {
            val2 = 0;
            val1 = val2;
        } else {
            char *target = *(char **)(c + 0x33c);
            struct Vec3 *tp = (struct Vec3 *)((long long)(int)(target + 0x5c) & 0xFFFFFFFFFFFFFFFFLL);
            tpos.x = tp->x;
            tpos.y = tp->y;
            tpos.z = tp->z;
            dist = Vec3_HorzDist((void *)(c + 0x5c), &tpos) >> 0xc;
            atanRes = _ZN4cstd5atan2E5Fix12IiES1_(
                *(int *)(c + 0x5c) - tpos.x, *(int *)(c + 0x64) - tpos.z);
            angleDiff = atanRes - *(short *)(c + 0x8e);
            j = ((u16)angleDiff >> 4) * 2;
            s = data_02082214[j];
            co = data_02082214[j + 1];
            val1 = (short)((-dist * co) / 2200);
            val2 = (short)((dist * s) / 1500);
        }

        _Z14ApproachLinearRsss((short *)(c + 0x8c), val1, 0xc0);
        _Z14ApproachLinearRsss((short *)(c + 0x90), val2, 0xc0);

        *(int *)(c + 0x60) = *(int *)(c + 0x324) + *(int *)(c + 0x330) + *(int *)(c + 0x334);
    } else {
        *(int *)(c + 0x60) = *(int *)(c + 0x324);
        *(short *)(c + 0x90) = 0;
        *(short *)(c + 0x8c) = *(short *)(c + 0x90);
    }

    func_ov002_020b5b98(c);
    if (_ZN8Platform21IsClsnInRangeOnScreenE5Fix12IiES1_(c, 0, 0) != 0)
        _ZN8Platform19UpdateClsnPosAndRotEv(c);

    if (DecIfAbove0_Short((unsigned short *)(c + 0x342)) == 0)
        *(void **)(c + 0x33c) = 0;

    return 1;
}
