typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef int Fix12;
struct Vector3 { Fix12 x, y, z; };
struct Ent { int f0; int f4; };
extern void func_0201267c(unsigned int a, void* p);
extern void _ZN5Actor10EarthquakeERK7Vector35Fix12IiE(void* self, const struct Vector3* v, Fix12 f);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, Fix12 x, Fix12 y, Fix12 z);
extern struct Ent *data_ov079_021275ec[];
extern unsigned int data_0209b454;
extern s16 data_02082214[];

void func_ov079_02125504(char *c)
{
    unsigned int frame;
    struct Vector3 v[2];
    s16 t;
    Fix12 px, pz;

    {
        unsigned int q;
        u8 i;
        struct Ent *e;
        unsigned int cur;
        unsigned int want;
        i = *(u8 *)(c + 0x414);
        q = *(unsigned int *)(c + 0x324);
        e = data_ov079_021275ec[(int)i * 5 + 3];
        cur = *(volatile unsigned int *)(c + 0x32c);
        want = e->f4;
        frame = (unsigned short)(q >> 12);
        if (cur != want)
            return;
    }

    if (frame > 7) {
        if (frame < 0x1e)
            goto reset;
        if (frame > 0x26)
            goto reset;
    }

    *(int *)(c + 0x98) = 0;
    if (*(u8 *)(c + 0x40a) != 0)
        return;
    *(u8 *)(c + 0x40a) = 1;
    func_0201267c(0xc5, c + 0x74);

    if ((data_0209b454 & 0x800000) == 0) {
        v[1].x = *(Fix12 *)(c + 0x5c);
        v[1].y = *(Fix12 *)(c + 0x60);
        v[1].z = *(Fix12 *)(c + 0x64);
        _ZN5Actor10EarthquakeERK7Vector35Fix12IiE(c, &v[1], 0x320000);
    }

    {
        char *hp = c + 0x300;
        s16 a0 = *(s16 *)(c + 0x8e);
        s16 a1 = *(s16 *)(hp + 0xe2);
        Fix12 py;
        px = *(Fix12 *)(c + 0x5c);
        v[0].x = px;
        py = *(Fix12 *)(c + 0x60);
        t = (s16)(a0 + a1 + 0x4000);
        v[0].y = py;
        pz = *(Fix12 *)(c + 0x64);
        v[0].z = pz;
        v[0].y = py + 0x28000;
    }

    if (frame <= 7) {
        int idx = (u16)t >> 4;
        s16 sn = data_02082214[idx * 2];
        s16 cs = data_02082214[idx * 2 + 1];
        v[0].x = px - sn * 0xaa;
        v[0].z = pz - cs * 0xaa;
    } else {
        int idx = (u16)t >> 4;
        s16 sn = data_02082214[idx * 2];
        s16 cs = data_02082214[idx * 2 + 1];
        v[0].x = px + sn * 0xaa;
        v[0].z = pz + cs * 0xaa;
    }
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xb2, v[0].x, v[0].y, v[0].z);
    return;
reset:
    *(u8 *)(c + 0x40a) = 0;
}
