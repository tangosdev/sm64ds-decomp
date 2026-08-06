#include "types.h"
#include "private/mtx43.h"
typedef short s16;
typedef signed char s8;
typedef unsigned char u8;
typedef unsigned int u32;
typedef int s32;


extern void _ZN8SaveData13PlayerLoseCapEv(void);
/* ROM symbol is Vector3_16 (not Vector3s) — wrong mangling left this BLIND and broke mwldarm. */
extern void *_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
    u32 actorID, u32 param1, const Vector3 *pos,
    const void *rot, int areaID, int deathTableID);
extern void func_02012790(int arg);
extern s16 Vec3_HorzAngle(const Vector3 *a, const Vector3 *b);
extern void Matrix4x3_FromTranslation(Mtx43 *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationY(Mtx43 *m, s16 angY);
extern void Matrix4x3_ApplyInPlaceToTranslation(Mtx43 *m, int x, int y, int z);
extern int _ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(
    void *self, void *actor, unsigned int msgId, const Vector3 *pos,
    unsigned int d, unsigned int e);
extern void func_0201267c(int a, void *b);
extern int _ZN6Player12GetTalkStateEv(void *self);
extern void _ZN6Player9DropActorEv(void *self);
extern void _ZN6Player18SetNewHatCharacterEjjb(void *self, unsigned int a, unsigned int b, int c);
extern void *_ZN5Actor10FindWithIDEj(u32 id);
extern void func_ov030_021141a8(char *c, int v);
extern void _ZN9Animation7AdvanceEv(void *self);
extern void _ZN12CylinderClsn5ClearEv(void *self);

extern void *data_0209f318;
extern Mtx43 data_020a0e68;

int func_ov030_021136b0(char *c)
{
    int msg;
    s16 a = *(s16 *)(*(char **)(c + 0x3a8) + 0x8e);
    *(s16 *)(c + 0x8e) = a;
    *(s16 *)(c + 0x94) = *(s16 *)(c + 0x8e);

    switch (*(u8 *)(c + 0x3c7)) {
    case 0:
        if (*(u8 *)(c + 0x3c8) != 0) {
            {
                char *p = *(char **)(c + 0x3a8);
                int t = (*(u8 *)(p + 0x6d9) == *(int *)(p + 8));
                t = (t != 0);
                *(u8 *)(c + 0x3c9) = t;
            }
            if (*(u8 *)(c + 0x3c9) == 0) {
                char *p = *(char **)(c + 0x3a8);
                *(u8 *)(p + 0x71a) = 1;
            } else {
                _ZN8SaveData13PlayerLoseCapEv();
            }
            {
                char *p = *(char **)(c + 0x3a8);
                void *spawned;
                *(u32 *)(c + 0x3b0) = *(u32 *)(p + 8);
                msg = *(s8 *)(c + 0xcc);
                spawned = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
                    0x10d,
                    (*(u32 *)(c + 0x3b0) << 8) | 2,
                    (Vector3 *)(c + 0x5c),
                    0,
                    msg,
                    -1);
                *(u32 *)(c + 0x3ac) = ((u32 *)spawned)[1];
            }
        } else {
            if (*(u8 *)(c + 0x3c9) != 0)
                func_02012790(0xa);
        }
        {
            u8 *st = (u8 *)(c + 0x3c7);
            (*st)++;
        }
        /* fall through */
    case 1: {
        s16 ang;
        Vector3 camPos;
        Vector3 msgPos;
        {
            u8 fl = *(u8 *)(c + 0x3c8);
            msg = fl ? 0xbe : 0xbf;
            void *camBase = data_0209f318;
            Vector3 *src = (Vector3 *)((char *)camBase + 0x8c);
            camPos.x = src->x;
            camPos.y = src->y;
            camPos.z = src->z;
            ang = Vec3_HorzAngle(&camPos, (Vector3 *)(*(char **)(c + 0x3a8) + 0x5c));
        }
        {
            int *op = (int *)(*(char **)(c + 0x3a8) + 0x5c);
            Matrix4x3_FromTranslation(&data_020a0e68, op[0], op[1], op[2]);
        }
        Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, ang);
        Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, 0, -0x64000);
        {
            int msgArg = ((int)msg << 16) >> 16;
            int my = data_020a0e68.a[10];
            int mx = data_020a0e68.a[9];
            int mz = data_020a0e68.a[11];
            msgPos.x = mx;
            msgPos.y = my;
            msgPos.z = mz;
            msgPos.y = *(int *)(c + 0x60) + 0x64000;
            if (_ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(
                    *(void **)(c + 0x3a8), c, msgArg, &msgPos, 0, 2) != 0) {
                func_0201267c(0xd1, c + 0x74);
                {
                    u8 *st = (u8 *)(c + 0x3c7);
                    (*st)++;
                }
            }
        }
        break;
    }
    case 2:
        if (_ZN6Player12GetTalkStateEv(*(void **)(c + 0x3a8)) == -1) {
            if (*(u8 *)(c + 0x3c8) != 0) {
                {
                    u32 *fl = (u32 *)(c + 0xb0);
                    *fl &= ~0x200u;
                }
                _ZN6Player9DropActorEv(*(void **)(c + 0x3a8));
                {
                    u32 *fl = (u32 *)(c + 0xb0);
                    *fl |= 0x200u;
                }
            } else {
                _ZN6Player9DropActorEv(*(void **)(c + 0x3a8));
            }
            {
                u8 *st = (u8 *)(c + 0x3c7);
                (*st)++;
            }
        }
        break;
    case 3: {
        int f1 = (int)((*(u32 *)(c + 0xb0) & 0x100) != 0);
        if (f1 == 0) {
            if (*(u8 *)(c + 0x3c8) != 0) {
                if (*(u8 *)(c + 0x3c9) == 0) {
                    char *p = *(char **)(c + 0x3a8);
                    _ZN6Player18SetNewHatCharacterEjjb(p, *(u8 *)(p + 0x6d9), 0, 0);
                }
                *(int *)(c + 0x3b8) = 1;
                func_ov030_021141a8(c, 2);
            } else {
                void *act = _ZN5Actor10FindWithIDEj(*(u32 *)(c + 0x3ac));
                int z = 0;
                *(int *)((char *)act + 0xc8) = z;
                {
                    char *p = *(char **)(c + 0x3a8);
                    int *src = (int *)(p + 0x5c);
                    *(int *)((char *)act + 0x5c) = src[0];
                    *(int *)((char *)act + 0x60) = src[1];
                    *(int *)((char *)act + 0x64) = src[2];
                }
                *(u32 *)(c + 0x3ac) = (u32)z;
                func_ov030_021141a8(c, 0xa);
            }
            *(void **)(c + 0x3a8) = 0;
        }
        break;
    }
    case 4: {
        char *p = *(char **)(c + 0x3a8);
        if (*(u8 *)(p + 0x6f9) == 0 &&
            *(u8 *)(p + 0x6fb) == 0 &&
            *(u8 *)(p + 0x6ff) == 0) {
            *(u8 *)(c + 0x3c7) = 0;
            {
                u8 *f = (u8 *)(c + 0x3c8);
                *f ^= 1;
            }
            break;
        }
        /* fall through */
    }
    case 5: {
        u32 flags = *(u32 *)(c + 0xb0);
        int f1 = (int)((flags & 0x100) != 0);
        if (f1 != 0) {
            int f2 = (int)((flags & 0x2000) != 0);
            if (f2 == 0)
                break;
        }
        func_ov030_021141a8(c, 0xa);
        *(void **)(c + 0x3a8) = 0;
        break;
    }
    default:
        break;
    }

    _ZN9Animation7AdvanceEv(c + 0x124);
    _ZN12CylinderClsn5ClearEv(c + 0x160);
    return 1;
}
