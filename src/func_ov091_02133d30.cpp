//cpp
// NONMATCHING: register allocation (div=47). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef unsigned int u32;
typedef int Fix12i;
typedef short s16;
typedef unsigned short u16;
typedef signed char s8;

extern "C" {
void func_02012694(u32 id, void *pos);
struct Callback;
u32 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32 slot, u32 effect, Fix12i x, Fix12i y, Fix12i z, const void *rot, Callback *cb);
void *_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(u32 id, u32 unk, const void *pos, const void *rot, int a, int b);
void *_ZN5Actor13ClosestPlayerEv(void *self);
int RandomIntInternal(int *seed);
s16 Vec3_VertAngle(const void *v1, const void *v0);
int _ZN9Animation8FinishedEv(void *self);
int func_ov091_02134044(void *c, void *p);
}

extern s16 SINE_TABLE[];
extern int RNG_STATE;
extern void *data_ov091_021356d0;

struct Vector3_16f { s16 x, y, z; };
struct Vec { Fix12i x, y, z; };

struct Self {
    char pad0[0x5c];
    Fix12i x, y, z;
    char pad1[0x8e - 0x68];
    u16 angle;
    char pad2[0x94 - 0x90];
    s16 f94;
    char pad3[0xcc - 0x96];
    s8 f_cc;
    char pad4[0x350 - 0xcd];
    char pad5[0x358 - 0x350];
    unsigned f358_lo : 12;
    unsigned type : 16;
    unsigned f358_hi : 4;
    char pad6[0x368 - 0x35c];
    u32 f368;
    u32 f36c;
};

extern "C" int func_ov091_02133d30(Self *self)
{
    Vector3_16f vec;
    volatile Vec pos;
    Vec tgt;
    void *spawned;
    void *player;
    int rnd;
    s16 ang;

    pos.x = self->x;
    pos.y = self->y;
    pos.z = self->z;

    if (self->type == 0x1f) {
        func_02012694(0x164, (char *)self + 0x74);
    }

    if (self->type > 0x1e) {
        vec.x = SINE_TABLE[(self->angle >> 4) * 2];
        vec.y = 0;
        vec.z = SINE_TABLE[(self->angle >> 4) * 2 + 1];

        pos.y = pos.y + 0x28000;

        self->f368 = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            self->f368, 0x72, pos.x, pos.y, pos.z, &vec, 0);
        self->f36c = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            self->f36c, 0x73, pos.x, pos.y, pos.z, &vec, 0);
    }

    if (!(self->type & 1)) {
        spawned = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0xe7, 1, &self->x, 0, self->f_cc, -1);
        if (spawned != 0) {
            player = _ZN5Actor13ClosestPlayerEv(self);
            if (player != 0) {
                void *ppos = (char *)player + 0x5c;
                tgt.x = *(int *)((char *)ppos + 0);
                tgt.y = *(int *)((char *)ppos + 4);
                tgt.z = *(int *)((char *)ppos + 8);
                tgt.y = *(int *)((char *)player + 0x644);

                rnd = RandomIntInternal(&RNG_STATE);
                ang = self->f94 + (s16)(0x2000 - (((rnd >> 8) & 3) << 0xc));

                *(s16 *)((char *)spawned + 0x92) = Vec3_VertAngle(&self->x, &tgt);
                *(s16 *)((char *)spawned + 0x94) = ang;
                *(s16 *)((char *)spawned + 0x96) = 0;
            }
        }
    }

    if (_ZN9Animation8FinishedEv((char *)self + 0x350) != 0) {
        *(s16 *)((char *)self + 0x100) = 0x64;
        func_ov091_02134044(self, &data_ov091_021356d0);
    }

    return 1;
}
