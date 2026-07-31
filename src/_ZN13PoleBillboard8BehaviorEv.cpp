//cpp
extern "C" {
struct Vector3 { int x, y, z; };

extern short data_02082214[];
extern void _ZN5Actor10EarthquakeERK7Vector35Fix12IiE(void *self, struct Vector3 *v, int f);
extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int id, struct Vector3 *v);
extern void func_ov015_0211166c(char *t);
extern int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void *self);
extern int func_ov015_021114f0(char *c);

int _ZN13PoleBillboard8BehaviorEv(char *c)
{
    switch (*(unsigned char *)(c + 0x397)) {
    case 0:
        break;
    case 1: {
        unsigned short ang = *(unsigned short *)(c + 0x390);
        int idx = ang >> 4;
        int s = *(short *)((char *)data_02082214 + (idx << 2));
        int amp = *(signed char *)(c + 0x396);
        int prod = amp * s;
        int d = (int)(((long long)prod * *(short *)(c + 0x394) + 0x800) >> 0xc);
        *(short *)(c + 0x8c) = d;
        if (*(short *)(c + 0x394) <= 0) {
            *(short *)(c + 0x8c) = 0;
            *(short *)(c + 0x390) = 0;
            *(unsigned char *)(c + 0x397) = 0;
        } else {
            *(short *)(((int)c + 0x394)) -= 8;
        }
        *(short *)(((int)c + 0x390)) += 0x400;
        break;
    }
    case 2: {
        short *p392 = (short *)(((long long)(int)(c + 0x392)));
        short *p8c = (short *)(((long long)(int)(c + 0x8c)));
        int amt = *(signed char *)(c + 0x396) << 0x17;
        *p392 = *p392 + (amt >> 16);
        *p8c = *p8c + *(short *)(c + 0x300 + 0x92);
        if (*(signed char *)(c + 0x396) == -1) {
            if (*(short *)(c + 0x8c) < -0x4000) {
                *(short *)(c + 0x8c) = -0x4000;
                *(short *)(c + 0x392) = 0;
                (*(unsigned char *)(((int)c + 0x397)))++;
                struct Vector3 pos;
                pos.x = *(int *)(c + 0x5c);
                pos.y = *(int *)(c + 0x60);
                pos.z = *(int *)(c + 0x64);
                _ZN5Actor10EarthquakeERK7Vector35Fix12IiE(c, &pos, 0x2000000);
                _ZN5Sound9PlayBank3EjRK7Vector3(0x44, (struct Vector3 *)(c + 0x74));
            }
        } else {
            *(int *)(((int)c + 0x38c)) += 0x2000;
            *(int *)(((int)c + 0x60)) += *(int *)(c + 0x38c);
            {
                int lim = *(int *)(c + 0x388) + 0x46000;
                if (*(int *)(c + 0x60) > lim) {
                    *(int *)(c + 0x60) = lim;
                }
            }
            if (*(short *)(c + 0x8c) > 0x4000) {
                *(short *)(c + 0x8c) = 0x4000;
                *(short *)(c + 0x392) = 0;
                (*(unsigned char *)(((int)c + 0x397)))++;
                struct Vector3 pos;
                pos.x = *(int *)(c + 0x5c);
                pos.y = *(int *)(c + 0x60);
                pos.z = *(int *)(c + 0x64);
                _ZN5Actor10EarthquakeERK7Vector35Fix12IiE(c, &pos, 0x2000000);
                _ZN5Sound9PlayBank3EjRK7Vector3(0x44, (struct Vector3 *)(c + 0x74));
            }
        }
        break;
    }
    case 3:
        break;
    default:
        break;
    }

    func_ov015_0211166c(c);
    if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(c, 0, 0))
        _ZN8Platform19UpdateClsnPosAndRotEv(c);
    func_ov015_021114f0(c);
    return 1;
}
}
