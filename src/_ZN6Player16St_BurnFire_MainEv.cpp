//cpp
/* _ZN6Player16St_BurnFire_MainEv @ 0x020d53ac (ov002, size 0x3a4)
 * Player burn-in-fire state: early-exits via ChangeState on a couple of
 * flag checks, then (unless blocked by a hazard-level check) spawns two
 * fire/smoke particles plus a burn sound at the player's position offset
 * upward, then runs a small state machine (0/1) driving the burn anim and
 * a homing turn+speed approach before handing off to the common per-frame
 * tail function.
 */
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;

extern char data_ov002_0211013c;
extern s16 data_02082214[];
extern u8 data_020a0e40;
extern u16 data_0209f49e[];
extern s16 data_0209f4a0[];

extern "C" {
extern void _ZN6Player11ChangeStateERNS_5StateE(char *c, void *st);
extern void func_02012694(unsigned int id, const void *v);
extern int func_ov002_020d91e0(char *thiz, int damage, int doPre);
extern void func_ov002_020c5dec(char *c, int r1);
extern int _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    unsigned int sys, unsigned int id, int x, int y, int z, const s16 *vec, void *cb);
extern int _ZN5Sound8PlayLongEjjjRK7Vector3j(unsigned int handle, unsigned int id, unsigned int vol, const void *vec, unsigned int flags);
extern int _ZN6Player6IsAnimEj(char *c, unsigned int a);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(char *c, unsigned int anim, int a, int b, unsigned int d);
extern int _ZNK6Player14GetBodyModelIDEjb(char *c, unsigned int a, int b);
extern void ApproachAngle(short *cur, short target, int divisor, int band, int maxStep);
extern void _Z14ApproachLinearRiii(int *cur, int target, int step);
extern void Player_AdvanceAnims(char *c);
}

extern "C" int _ZN6Player16St_BurnFire_MainEv(char *c)
{
    int speed = 0;

    if (*(u8 *)(c + 0x6e5) >= 0xa0 || *(u8 *)(c + 0x6f9) != 0) {
        _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_0211013c);
        return 1;
    }

    if (*(u8 *)(c + 0x707) != 0) {
        func_02012694(3, c + 0x74);
        _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_0211013c);
        return 1;
    }

    {
        u16 lvl = *(u16 *)(c + 0x6a4);
        if (lvl == 0x50 || lvl == 0x32 || lvl == 0x14) {
            if (func_ov002_020d91e0(c, 0x100, 1) != 0) {
                func_ov002_020c5dec(c, 2);
                return 1;
            }
        }
    }

    *(u8 *)(((long long)(int)(c + 0x6e5)) & 0xFFFFFFFFFFFFFFFFLL) += 2;

    {
        struct Info {
            s16 vec[3];
            int pos[3];
        } info;
        int zz = *(int *)(c + 0x64) - 0x1e000;
        int yy = *(int *)(c + 0x60) + 0x32000;
        int xx = *(int *)(c + 0x5c);

        info.pos[0] = xx;
        info.pos[1] = yy;
        info.pos[2] = zz;
        info.vec[0] = data_02082214[(*(u16 *)(c + 0x8e) >> 4) * 2];
        info.vec[1] = 0;
        info.vec[2] = data_02082214[(*(u16 *)(c + 0x8e) >> 4) * 2 + 1];

        *(int *)(c + 0x628) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(int *)(c + 0x628), 0xbd, info.pos[0], info.pos[1], info.pos[2], info.vec, 0);
        *(int *)(c + 0x62c) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(int *)(c + 0x62c), 0xbe, info.pos[0], info.pos[1], info.pos[2], info.vec, 0);
        *(int *)(c + 0x620) = _ZN5Sound8PlayLongEjjjRK7Vector3j(
            *(int *)(c + 0x620), 0, 0x106, c + 0x74, 0);
    }

    switch (*(u8 *)(c + 0x6e3)) {
    case 0:
        if (_ZN6Player6IsAnimEj(c, 0x53) != 0) {
            *(u8 *)(((long long)((int)c + 0x6e5)) & 0xFFFFFFFFFFFFFFFFLL) += 1;
        }
        if (*(u8 *)(c + 0x6de) == 0) {
            *(u8 *)(c + 0x6e3) = 1;
            _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x3f, 0, 0x1000, 0);
            {
                char *anim = (char *)(((long long)(int)(*(char **)(c + (_ZNK6Player14GetBodyModelIDEjb(c, *(u32 *)(c + 8) & 0xff, 0) << 2) + 0xdc) + 0x50)) & 0xffffffffffffffffLL);
                *(int *)(anim + 0xc) = 0x4000;
            }
        }
        break;
    case 1:
        {
            int idx = data_020a0e40 * 0x18;
            if ((*(u16 *)((char *)data_0209f49e + idx) & 2) != 0) {
                *(u8 *)(c + 0x6e3) = 0;
                _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x53, 0, 0x1000, 0);
                *(u8 *)(c + 0x6de) = 1;
                *(u8 *)(c + 0x6df) = 0;
                *(int *)(c + 0xa8) = 0x1e000;
                break;
            }
        }
        if (*(u8 *)(c + 0x6de) != 0) {
            *(u8 *)(c + 0x6e3) = 0;
            _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x54, 0, 0x1000, 0);
            *(u8 *)(c + 0x6de) = 1;
            *(u8 *)(c + 0x6df) = 0;
            break;
        }
        _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x3f, 0, 0x1000, 0);
        {
            char *anim = (char *)(((long long)(int)(*(char **)(c + (_ZNK6Player14GetBodyModelIDEjb(c, *(u32 *)(c + 8) & 0xff, 0) << 2) + 0xdc) + 0x50)) & 0xffffffffffffffffLL);
            *(int *)(anim + 0xc) = 0x4000;
        }
        {
            int idx = data_020a0e40 * 0x18;
            speed = 0x28000;
            if (*(s16 *)((char *)data_0209f4a0 + idx) != 0) {
                ApproachAngle((short *)(c + 0x8e), *(s16 *)(c + 0x6d2), 8, 0x4000, 0x10);
            }
            *(s16 *)(c + 0x94) = *(s16 *)(c + 0x8e);
        }
        break;
    }

    _Z14ApproachLinearRiii((int *)(c + 0x98), speed, 0x1000);
    Player_AdvanceAnims(c);
    return 1;
}
