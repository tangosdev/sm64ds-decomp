//cpp
// @symbol func_ov085_0212b8dc
/* Rabbit (daMip_c, ov085) state Main -- the closest-Player flee/idle machine.
   Reached via the Rabbit state table at data_ov085_02130044 (state struct
   data_ov085_021306cc). VERIFIED byte-exact + link-verified at mwccarm 2004/b56.

   The scalar typedefs and Vector3 come from types.h rather than being respelled
   here: the local copies were a shadow declaration the langmode ratchet counts,
   and the header was built both ways -- all 0x338 bytes are identical. */
#include "types.h"

extern "C" {
    char* _ZN5Actor13ClosestPlayerEv();
    int func_ov085_0212a788(char* c);
    u32 func_02022cbc(u32 uid, u32 eid, Fix12i x, Fix12i y, Fix12i z, const void* dir);
    Fix12i Vec3_HorzDist(const Vector3* a, const Vector3* b);
    void func_ov085_0212bc78(char* c, void* p);
    int RandomIntInternal(int* seed);
    s16 Vec3_HorzAngle(const Vector3* a, const Vector3* b);
    int _ZN9Animation8FinishedEv(void* a);
    void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* self, void* bca, int a, Fix12i fix, u32 t);
    void ApproachAngle(short* p, s16 target, int a, int b, int step);
}

extern int data_0209e650[];
extern void* data_ov085_0213066c;
extern void* data_ov085_021305d0[];
extern void* data_ov085_021305b0[];
extern void* data_ov085_021305c0[];
extern void* data_ov085_021305c8[];

extern "C" int func_ov085_0212b8dc(char* c)
{
    char* player = _ZN5Actor13ClosestPlayerEv();
    if (player == 0) return 1;

    if (func_ov085_0212a788(c) == 1) {
        Vector3 sp;
        s32 pair[2];
        pair[0] = *((s32*)(c + 0x5c));
        pair[1] = *((s32*)(c + 0x464));
        s32 z = *((s32*)(c + 0x64));
        s32 y = pair[1] + 0x3000;
        s32 x = pair[0];
        *((volatile s32*)(&sp.x)) = x;
        *((volatile s32*)(&sp.y)) = y;
        *((volatile s32*)(&sp.z)) = z;
        *((s32*)(c + 0x46c)) = func_02022cbc(*((s32*)(c + 0x46c)), 0xe8,
            *((volatile s32*)(&sp.x)), *(&sp.y), z, 0);
    }

    Vector3 pp;
    Vector3* ppp = (Vector3*)(player + 0x5c);
    pp.x = ppp->x;
    pp.y = ppp->y;
    pp.z = ppp->z;
    int lim = 0x3e8000;
    if (*(int*)(c + 0x43c) == 7) lim = 0x2ee000;

    if (Vec3_HorzDist((Vector3*)(c + 0x5c), &pp) < lim) {
        int t = *(int*)(c + 0x43c);
        int cond = 0;
        if (t == 7 || t == 1 ||
            (t == 4 && *(int*)(c + 0x440) == 1) ||
            (t == 4 && *(int*)(c + 0x440) == 3)) {
            if (*(int*)(c + 0x60) + 0x64000 <= pp.y) cond = 1;
        }
        if (!cond) {
            *(int*)(c + 0x98) = 0;
            func_ov085_0212bc78(c, &data_ov085_0213066c);
            return 1;
        }
    }

    u32 r = (u32)RandomIntInternal(data_0209e650) >> 8;
    if (*(int*)(c + 0x43c) == 1) {
        if (Vec3_HorzDist((Vector3*)(c + 0x5c), &pp) < 0x4b0000) {
            *(s16*)(c + 0x424) = Vec3_HorzAngle((Vector3*)(c + 0x5c), &pp) + 0x8000;
            *(s16*)(c + 0x100) = 0x1e;
        }
    }

    if (_ZN9Animation8FinishedEv(c + 0x350) != 0) {
        switch (*(int*)(c + 0x41c)) {
        case 1:
            *(int*)(c + 0x98) = 0x4000;
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x300, data_ov085_021305d0[1], 0x40000000, 0x1000, 0);
            (*(int*)(c + 0x41c))++;
            break;
        case 2:
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x300, data_ov085_021305b0[1], 0x40000000, 0x1000, 0);
            *(int*)(c + 0x98) = 0;
            (*(int*)(c + 0x41c))++;
            break;
        case 3:
            *(s16*)(c + 0x100) = (r & 0x1f) + 0x1e;
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x300, data_ov085_021305c0[1], 0, 0x1000, 0);
            *(int*)(c + 0x41c) = 0;
            break;
        }
    }

    if (*(int*)(c + 0x41c) == 0 && *(u16*)(c + 0x100) == 0) {
        *(s16*)(c + 0x424) = Vec3_HorzAngle((Vector3*)(c + 0x5c), (Vector3*)(c + 0x42c));
        s16* ang = (s16*)(c + 0x424);
        *ang = *ang + (0x1800 - ((r & 3) << 12));
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x300, data_ov085_021305c8[1], 0x40000000, 0x1000, 0);
        *(int*)(c + 0x41c) = 1;
    }

    ApproachAngle((short*)(c + 0x94), *(s16*)(c + 0x424), 1, 0x500, 0x500);
    return 1;
}
