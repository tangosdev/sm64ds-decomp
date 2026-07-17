typedef struct Vector3 { int x, y, z; } Vector3;

#define LAUNDER(p) ((long long)(int)(p) & 0xFFFFFFFFFFFFFFFFLL)

struct Obj {
    char pad0[0x60];
    int f60;
    char pad64[0x74 - 0x64];
    Vector3 pos;
    char pad80[0x128 - 0x80];
    unsigned int f128;
    int f12c;
    int f130;
    char pad134[0x144 - 0x134];
    int f144;
    unsigned int f148;
    unsigned short f14c;
    unsigned short f14e;
    unsigned char f150;
    unsigned char f151;
    char pad152[0x155 - 0x152];
    unsigned char f155;
    unsigned char f156;
};

extern void *_ZN5Actor10FindWithIDEj(unsigned int id);
extern unsigned int _ZN5Sound8PlayLongEjjjRK7Vector3j(unsigned int a0, unsigned int a1, unsigned int a2, const Vector3 *a3, unsigned int a4);
extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int a0, const Vector3 *a1);
extern int func_ov063_0211c770(void *actor, int idx);
extern int _ZN5Sound15PlaySecretSoundEP5ActorPt(void *actor, unsigned short *snd);
extern int data_ov063_0211e270[];

void func_ov063_0211cc18(struct Obj *a)
{
    int limit;
    struct Obj *other;
    unsigned char *pst;
    int *p60;
    int *p144;

    limit = data_ov063_0211e270[a->f151];
    other = (struct Obj *)_ZN5Actor10FindWithIDEj(a->f128);
    if (other != 0) {
        if (other->f155 == 0)
            return;
        a->f156 = 1;
    } else {
        if (a->f155 == 0)
            return;
        a->f156 = 1;
    }

    switch (a->f150) {
    case 0:
        a->f60 = a->f130;
        a->f144 = 0;
        pst = (unsigned char *)(int)LAUNDER((char *)a + 0x150);
        *pst = (unsigned char)(*pst + 1);
        /* fallthrough */
    case 1:
        p60 = (int *)(int)LAUNDER((char *)a + 0x60);
        p144 = (int *)(int)LAUNDER((char *)a + 0x144);
        *p60 = *p60 + 0x8000;
        *p144 = *p144 + 8;
        a->f148 = _ZN5Sound8PlayLongEjjjRK7Vector3j(a->f148, 3, 0x82, &a->pos, 0);
        if (a->f144 <= limit)
            return;
        a->f144 = limit;
        a->f60 = a->f130 + (limit << 12);
        pst = (unsigned char *)(int)LAUNDER((char *)a + 0x150);
        *pst = (unsigned char)(*pst + 1);
        return;
    case 2:
        if (a->f14c == 0)
            _ZN5Sound9PlayBank3EjRK7Vector3(0x3d, &a->pos);
        if (func_ov063_0211c770(a, a->f14c) != 0) {
            pst = (unsigned char *)(int)LAUNDER((char *)a + 0x150);
            *pst = (unsigned char)(*pst + 1);
        }
        return;
    case 3:
        if (a->f151 == 2) {
            if (_ZN5Sound15PlaySecretSoundEP5ActorPt(a, &a->f14e) != 0) {
                pst = (unsigned char *)(int)LAUNDER((char *)a + 0x150);
                *pst = (unsigned char)(*pst + 1);
            }
        } else {
            pst = (unsigned char *)(int)LAUNDER((char *)a + 0x150);
            *pst = (unsigned char)(*pst + 1);
        }
        return;
    case 4:
    default:
        return;
    }
}
