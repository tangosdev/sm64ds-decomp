// @symbol func_ov071_02120d30
#include "common.h"

extern int _Z14ApproachLinearRiii(int *ref, int target, int step);
extern unsigned char DecIfAbove0_Byte(unsigned char *p);
extern void func_0201267c(unsigned int id, void *p);
extern unsigned int _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    unsigned int a, unsigned int b, int x, int y, int z, const void *ang, void *cb);
extern unsigned int _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
    unsigned int a, unsigned int b, int x, int y, int z, const void *ang);
extern void *_ZN8Particle6System12FromUniqueIDEj(unsigned int id);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
extern void _ZN9Animation7AdvanceEv(void *self);
extern int _ZN9Animation8FinishedEv(void *self);
extern void *_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
    unsigned int actorID, unsigned int param, Vector3 *pos, void *rot, s8 areaID, s16 deathTableID);
extern void _ZN8dActor_c8PoofDustEv(void *self);
extern void _ZN8dActor_c19UntrackAndSpawnStarERajRK7Vector3h(
    void *self, signed char *starFlag, unsigned int starID, Vector3 *pos, unsigned int how);
extern void _ZN8dActor_c24KillAndTrackInDeathTableEv(void *self);
extern void _ZN7fBase_c18MarkForDestructionEv(void *self);

extern s16 data_02082214[];
extern s8 data_0209f2f8;

int func_ov071_02120d30(char *c)
{
    unsigned int id1;

    _Z14ApproachLinearRiii((int *)(c + 0x98), *(s16 *)(c + 0x20e), 300);
    *(s16 *)(c + 0x8e) = (s16)(*(s16 *)(c + 0x8e) + *(int *)(c + 0x98));
    *(int *)(c + 0x1f8) = *(int *)(c + 0x1f8) + *(int *)(c + 0x98);
    if (*(int *)(c + 0x1f8) / 131070 != 0) {
        func_0201267c(0x119, c + 0x74);
    }
    {
        int *spin = (int *)(c + 0x1f8);
        *spin %= 131070;
    }

    id1 = *(unsigned int *)(c + 0x204);
    if (id1 != 0 && *(unsigned int *)(c + 0x208) != 0) {
        void *p1;
        void *p2;
        *(unsigned int *)(c + 0x204) =
            _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                id1, 0x13a, *(int *)(c + 0x5c), *(int *)(c + 0x60), *(int *)(c + 0x64), 0, 0);
        *(unsigned int *)(c + 0x208) =
            _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
                *(unsigned int *)(c + 0x208), 0x13b, *(int *)(c + 0x5c), *(int *)(c + 0x60), *(int *)(c + 0x64), 0);
        p1 = _ZN8Particle6System12FromUniqueIDEj(*(unsigned int *)(c + 0x204));
        p2 = _ZN8Particle6System12FromUniqueIDEj(*(unsigned int *)(c + 0x208));
        if (p1 != 0) {
            *(int *)((char *)p1 + 0x50) = 0x7fff;
        }
        if (p2 != 0) {
            *(int *)((char *)p2 + 0x50) = 0x7fff;
        }
    }

    switch ((unsigned char)c[0x214]) {
    case 0: {
        s16 sinv = data_02082214[((int)*(unsigned short *)(c + 0x210) >> 4) * 2];
        *(s16 *)(c + 0x8c) = (s16)((int)(((s64)*(int *)(c + 0x1fc) * sinv + 0x800) >> 12));
        *(s16 *)(c + 0x210) = (s16)(*(s16 *)(c + 0x210) + 0xe000);
        _Z14ApproachLinearRiii((int *)(c + 0x1fc), 0, 0x1b);
        if (DecIfAbove0_Byte((unsigned char *)c + 0x215) == 0) {
            unsigned char *st = (unsigned char *)c + 0x214;
            *st = (unsigned char)(*st + 1);
        }
        break;
    }
    case 1:
        _ZN9Animation7AdvanceEv(c + 0x124);
        if (_ZN9Animation8FinishedEv(c + 0x124) != 0) {
            unsigned char *st = (unsigned char *)c + 0x214;
            *st = (unsigned char)(*st + 1);
        }
        break;
    case 2: {
        int scale;
        unsigned short kind;
        int isBig;
        if (_Z14ApproachLinearRiii((int *)(c + 0x1f0), 0xa4, 0xa4) != 0) {
            unsigned char *st = (unsigned char *)c + 0x214;
            *st = (unsigned char)(*st + 1);
        }
        scale = *(int *)(c + 0x1f0);
        *(int *)(c + 0x80) = scale;
        *(int *)(c + 0x84) = scale;
        *(int *)(c + 0x88) = scale;
        kind = *(unsigned short *)(c + 0xc);
        isBig = (int)(kind == 0x106);
        if (isBig != 0) {
            *(int *)(c + 0x178) = *(int *)(c + 0x1f0) * 0x55;
        } else {
            int isSmall = (int)(kind == 0x107);
            if (isSmall != 0) {
                *(int *)(c + 0x178) = *(int *)(c + 0x1f0) * 0x55;
            }
        }
        break;
    }
    case 3: {
        unsigned short kind = *(unsigned short *)(c + 0xc);
        int isBig = (int)(kind == 0x106);
        if (isBig != 0) {
            int yadj, zcopy, y, z, x;
            Vector3 pos;
            y = *(int *)(c + 0x60);
            z = *(int *)(c + 0x64);
            yadj = 0x78000;
            yadj = y + yadj;
            zcopy = z;
            x = *(int *)(c + 0x5c);
            pos.x = x;
            pos.z = zcopy;
            pos.y = yadj;
            _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
                0x122, 2, &pos, 0, *(signed char *)(c + 0xcc), -1);
            _ZN8dActor_c8PoofDustEv(c);
        } else {
            int isSmall = (int)(kind == 0x107);
            if (isSmall != 0) {
                unsigned char star = (unsigned char)(*(unsigned int *)(c + 8) & 0xf);
                _ZN8dActor_c19UntrackAndSpawnStarERajRK7Vector3h(
                    c, (signed char *)(c + 0x217), star, (Vector3 *)(c + 0x5c), 4);
                _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
                    0x124, *(int *)(c + 0x5c), *(int *)(c + 0x60), *(int *)(c + 0x64));
                _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
                    0x125, *(int *)(c + 0x5c), *(int *)(c + 0x60), *(int *)(c + 0x64));
                _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
                    0x126, *(int *)(c + 0x5c), *(int *)(c + 0x60), *(int *)(c + 0x64));
            }
        }
        func_0201267c(0xc4, c + 0x74);
        if (data_0209f2f8 == 0x2e) {
            _ZN8dActor_c24KillAndTrackInDeathTableEv(c);
        } else {
            _ZN7fBase_c18MarkForDestructionEv(c);
        }
        break;
    }
    }
    return 1;
}
