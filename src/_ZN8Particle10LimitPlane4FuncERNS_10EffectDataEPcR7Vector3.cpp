//cpp
typedef short s16;
typedef unsigned short u16;
typedef long long s64;

struct Vector3 { int x, y, z; };

struct EffectData {
    int f0;
    s16 f4;
    u16 mode : 2;
    u16 rest : 14;
};

struct Data {
    char pad0[0xc];
    int f0c;
    char pad10[8];
    int f18;
    char pad1c[8];
    int f24;
    char pad28[4];
    u16 f2c, f2e;
};

extern "C" void _ZN8Particle10LimitPlane4FuncERNS_10EffectDataEPcR7Vector3(struct EffectData* ed, char* p, struct Vector3* v);

extern "C" void _ZN8Particle10LimitPlane4FuncERNS_10EffectDataEPcR7Vector3(struct EffectData* ed, char* p, struct Vector3* v)
{
    struct Data* d = (struct Data*)p;
    switch (ed->mode) {
    case 0:
        {
            int c, val;
            val = ed->f0;
            c = d->f0c;
            if (c < val) {
                if (c + d->f18 > val) { d->f2e = d->f2c; return; }
            }
            if (c > val) {
                if (c + d->f18 < val) d->f2e = d->f2c;
            }
        }
        break;
    case 1:
        {
            int c, val;
            val = ed->f0;
            c = d->f0c;
            if (c < val) {
                if (c + d->f18 > val) {
                    d->f18 = val - c;
                    d->f24 = -(int)(((s64)d->f24 * (s16)ed->f4 + 0x800) >> 12);
                    return;
                }
            }
            if (c > val) {
                if (c + d->f18 < val) {
                    d->f18 = val - c;
                    d->f24 = -(int)(((s64)d->f24 * (s16)ed->f4 + 0x800) >> 12);
                }
            }
        }
        break;
    }
}
