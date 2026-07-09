typedef unsigned short u16;
typedef short s16;
typedef unsigned char u8;

struct Vec3 { int x, y, z; };

extern int _ZN5Actor13DistToCPlayerEv(char *self);
extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int id, struct Vec3 *v);
extern void _ZN5Actor10EarthquakeERK7Vector35Fix12IiE(void *self, struct Vec3 *v, int f);
extern void AddVec3(struct Vec3 *a, struct Vec3 *b, struct Vec3 *c);
extern void func_0200fa04();
extern void func_ov023_02111308(char *t);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void *self);
extern int func_ov023_0211124c(char *c);
extern short data_02082214[];

int func_ov023_021113b0(char *c)
{
    switch (*(u8 *)(c + 0x322)) {
    case 0:
        if (_ZN5Actor13DistToCPlayerEv(c) < 0x3e8000) {
            (*(u8 *)(((int)c + 0x322) & 0xFFFFFFFFFFFFFFFF))++;
            _ZN5Sound9PlayBank3EjRK7Vector3(0x43, (struct Vec3 *)(c + 0x74));
        }
        break;
    case 1:
        *(s16 *)(((int)c + 0x31e) & 0xFFFFFFFFFFFFFFFF) -= 0x40;
        if (*(s16 *)(c + 0x31e) <= -0x1000) {
            *(s16 *)(c + 0x31e) = -0x1000;
        }
        *(s16 *)(((int)c + 0x8c) & 0xFFFFFFFFFFFFFFFF) += *(s16 *)(c + 0x31e);
        if (*(s16 *)(c + 0x8c) <= -0x4000) {
            struct Vec3 off;
            struct Vec3 pos;
            int tmp[3];
            int s0, s1;

            *(s16 *)(c + 0x8c) = -0x4000;
            *(s16 *)(c + 0x31e) = 0;
            *(s16 *)(c + 0x320) = 0;

            pos.x = *(int *)(c + 0x5c);
            pos.y = *(int *)(c + 0x60);
            pos.z = *(int *)(c + 0x64);
            _ZN5Actor10EarthquakeERK7Vector35Fix12IiE(c, &pos, 0x4000000);
            _ZN5Sound9PlayBank3EjRK7Vector3(0x44, (struct Vec3 *)(c + 0x74));

            s0 = data_02082214[(*(u16 *)(c + 0x8e) >> 4) * 2];
            off.x = (int)(((long long)s0 * -0x190000 + 0x800) >> 12);
            s1 = data_02082214[(*(u16 *)(c + 0x8e) >> 4) * 2 + 1];
            off.z = (int)(((long long)s1 * -0x190000 + 0x800) >> 12);
            AddVec3(&off, (struct Vec3 *)(c + 0x5c), &off);

            tmp[0] = off.x;
            tmp[1] = off.y;
            tmp[2] = off.z;
            func_0200fa04(c, tmp, 1);

            (*(u8 *)(((int)c + 0x322) & 0xFFFFFFFFFFFFFFFF))++;
        }
        break;
    case 2:
        if (*(u16 *)(c + 0x320) >= 0x3c) {
            u8 *st = (u8 *)(((int)c + 0x322) & 0xFFFFFFFFFFFFFFFF);
            *(s16 *)(c + 0x31e) = 0x80;
            (*st)++;
            _ZN5Sound9PlayBank3EjRK7Vector3(0x45, (struct Vec3 *)(c + 0x74));
        } else {
            (*(u16 *)(((int)c + 0x320) & 0xFFFFFFFFFFFFFFFF))++;
        }
        break;
    case 3:
        *(s16 *)(((int)c + 0x8c) & 0xFFFFFFFFFFFFFFFF) += *(s16 *)(c + 0x31e);
        if (*(s16 *)(c + 0x8c) >= 0) {
            *(s16 *)(c + 0x31e) = 0;
            *(s16 *)(c + 0x8c) = 0;
            *(s16 *)(c + 0x320) = 0;
            (*(u8 *)(((int)c + 0x322) & 0xFFFFFFFFFFFFFFFF))++;
        }
        break;
    case 4:
        if (*(u16 *)(c + 0x320) >= 0x3c) {
            *(u8 *)(c + 0x322) = 0;
        } else {
            (*(u16 *)(((int)c + 0x320) & 0xFFFFFFFFFFFFFFFF))++;
        }
        break;
    }

    func_ov023_02111308(c);
    _ZN8Platform19UpdateClsnPosAndRotEv(c);
    func_ov023_0211124c(c);
    return 1;
}
