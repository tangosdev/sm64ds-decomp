//cpp
// @symbol _ZN16daObjFallBlock_c8BehaviorEv
#include "daObjFallBlock_c.h"
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov098).
/* daObjFallBlock_c::Behavior - the whole fall-block state machine; see the
   class header for the field-by-field account. The draft cast `this` to a
   32-entry vtable-shim struct and called slot 31 (KillOrWhatever); that
   slot is Kill(), already a named virtual on this class (its own key
   function, per daObjFallBlock_c.h), so this calls it directly. UpdatePos
   is dActor_c's own named member (dActor_c.h), inherited; the
   dBgW calls go through the real named mMeshCollider member
   (dBgActor_c.h) instead of a raw offset-0x124 cast. The Fix12<int>-typed
   dBgActor_c calls stay spelled as their extern "C" mangled names -- see
   dBgActor_c.h's note on why declaring the true types there breaks the
   byte match. */
extern "C" {
int DecIfAbove0_Byte(u8 *p);
int DecIfAbove0_Short(u16 *p);
void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned id, void *pos);
void *_ZN8dActor_c13ClosestPlayerEv(void *self);
int Vec3_HorzDist(void *a, void *b);
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void *self);
void func_ov098_0213a00c(void *c);
void func_ov098_0213a0a8(void *c);
void func_ov098_0213a0e8(void *c);
void func_ov098_0213a148(void *c);
void func_ov098_0213a23c(void *c);
extern s16 data_02082214[];
extern signed char data_0209f2f8;
}

#define U64 0xFFFFFFFFFFFFFFFFLL
#define BASE(c, off) ((char *)(((int)(c) + (off)) & U64))

s32 daObjFallBlock_c::Behavior()
{
    char *c = (char *)this;
    int all;
    char *p;
    char *q;
    int is53;
    s16 yaw;
    int dist;
    int thresh;
    int flag;
    s16 sinv;
    u8 *st;
    char *b300;
    void *pl;

    is53 = (int)(*(u16 *)(c + 0xc) == 0x53);
    if (is53 != 0) {
        if (*(u8 *)(c + 0x342) != 0) {
            if (*(int *)(c + 0x330) == 0)
                func_ov098_0213a0e8(c);
            else
                func_ov098_0213a0a8(c);
            return 1;
        }
    }

    func_ov098_0213a00c(c);
    if (*(u8 *)(c + 0x33f) != 0) {
        if (DecIfAbove0_Byte((u8 *)BASE(c, 0x33f)) == 0) {
            if (mMeshCollider.IsEnabled() != 0)
                mMeshCollider.Disable();
        }
        return 1;
    }

    is53 = (int)(*(u16 *)(c + 0xc) == 0x53);
    if (is53 == 0) {
        if (*(int *)(c + 0x344) == 0) {
            p = c;
            all = 1;
            while (1) {
                if (*(u8 *)(p + 0x340) == 0) {
                    all = 0;
                    break;
                }
                p = *(char **)(p + 0x348);
                if (p == 0)
                    break;
            }
            if (all != 0) {
                q = c;
                while (1) {
                    if (q == 0)
                        break;
                    func_ov098_0213a148(q);
                    q = *(char **)(q + 0x348);
                }
            }
        }
    }

    switch (*(u8 *)(c + 0x33c)) {
    case 0:
        if (*(u8 *)(c + 0x33e) != 0) {
            *(u8 *)(c + 0x340) = 0;
            b300 = BASE(c, 0x300);
            *(s16 *)(b300 + 0x3a) = 4;
            st = (u8 *)BASE(c, 0x33c);
            *st = (u8)(*st + 1);
        }
        break;

    case 1:
        if (DecIfAbove0_Short((u16 *)BASE(c, 0x33a)) == 0) {
            st = (u8 *)BASE(c, 0x33c);
            *st = (u8)(*st + 1);
            _ZN5Sound9PlayBank3EjRK7Vector3(0x2d, c + 0x74);
            b300 = BASE(c, 0x300);
            *(s16 *)(b300 + 0x3a) = 0x5a;
        } else {
            b300 = BASE(c, 0x300);
            sinv = data_02082214[(*(u16 *)(b300 + 0x38) >> 4) << 1];
            *(int *)(c + 0x60) =
                *(int *)(c + 0x324)
                + (int)(((long long)sinv * 0x19000 + 0x800) >> 12);
            {
                s16 *p338 = (s16 *)BASE(c, 0x338);
                *p338 = (s16)(*p338 + 0x3000);
            }
        }
        break;

    case 2:
        if (*(s16 *)(c + 0x8c) < 0x400) {
            s16 *p334 = (s16 *)BASE(c, 0x334);
            *p334 = (s16)(*p334 + 0x80);
        } else {
            if (mMeshCollider.IsEnabled() != 0)
                mMeshCollider.Disable();
        }
        yaw = *(s16 *)(c + 0x90);
        if (yaw > -0x400) {
            if (yaw < 0x400) {
                s16 *p336 = (s16 *)BASE(c, 0x336);
                *p336 = (s16)(*p336 + 0x40);
            }
        }
        {
            s16 t = *(s16 *)(((int)c + 0x8c) & U64);
            t = (s16)(t + *(s16 *)(c + 0x334));
            *(s16 *)(((int)c + 0x8c) & U64) = t;
            t = (s16)(*(s16 *)(((int)c + 0x90) & U64) + *(s16 *)(c + 0x336));
            *(s16 *)(((int)c + 0x90) & U64) = t;
            UpdatePos(0);
        }
        if (DecIfAbove0_Short((u16 *)BASE(c, 0x33a)) == 0) {
            *(int *)(c + 0x5c) = *(int *)(c + 0x320);
            *(int *)(c + 0x60) = *(int *)(c + 0x324);
            *(int *)(c + 0x64) = *(int *)(c + 0x328);
            st = (u8 *)BASE(c, 0x33c);
            *st = (u8)(*st + 1);
        } else {
            if (*(int *)(c + 0x60) < *(int *)(c + 0x32c)) {
                Kill();
                *(int *)(c + 0x5c) = *(int *)(c + 0x320);
                *(int *)(c + 0x60) = *(int *)(c + 0x324);
                *(int *)(c + 0x64) = *(int *)(c + 0x328);
                st = (u8 *)BASE(c, 0x33c);
                *st = (u8)(*st + 1);
            }
        }
        break;

    case 3:
        *(int *)(c + 0x5c) = *(int *)(c + 0x320);
        *(int *)(c + 0x60) = *(int *)(c + 0x324);
        *(int *)(c + 0x64) = *(int *)(c + 0x328);
        is53 = (int)(*(u16 *)(c + 0xc) == 0x53);
        if (is53 == 0) {
            pl = _ZN8dActor_c13ClosestPlayerEv(c);
            dist = Vec3_HorzDist(c + 0x320, (char *)pl + 0x5c);
            thresh = 0x28a000;
            if (data_0209f2f8 == 0x2e)
                thresh = 0xa000;
            flag = (*(int *)(c + 0xb0) & 8) ? 1 : 0;
            if (flag != 0 && dist > thresh)
                *(u8 *)(c + 0x340) = 1;
            else
                *(u8 *)(c + 0x340) = 0;
        }
        break;

    default:
        break;
    }

    func_ov098_0213a23c(c);
    if (*(u8 *)(c + 0x33c) <= 1) {
        if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(c, 0, 0) != 0)
            _ZN10dBgActor_c19UpdateClsnPosAndRotEv(c);
    }
    *(u8 *)(c + 0x33e) = 0;
    return 1;
}
