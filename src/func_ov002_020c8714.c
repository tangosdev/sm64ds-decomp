#include "types.h"
/* func_ov002_020c8714 @ 0x020c8714 (ov002, size 0x268)
 * Player cap-power ending update: while active, ducks the music, damps the
 * forward speed (or launches on variant 1), and streams the sparkle trail;
 * when the anim finishes either spawns the cap-return actor with the level
 * fanfare (mode 0x10) or spawns the star and hands the player to it.
 */
extern void _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(int, int);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(char *, unsigned int, int, int, unsigned int);
extern int _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(int, unsigned int, int, int, int, int, int);
extern int _ZN6Player12FinishedAnimEv(char *);
extern int _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(int, int, char *, int, int, int);
extern void _ZN5Sound9PlayBank0EjRK7Vector3(int, char *);
extern void func_ov002_020c7ff8(char *);
extern void func_ov002_020e7090(int, char *);

int func_ov002_020c8714(char *c)
{
    volatile int v[3];

    if (*(u8 *)(c + 0x70c) == 0) {
        _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0x14, 0x15666);
        if (*(u16 *)(c + 0x6a4) == 0) {
            *(int *)(c + 0x98) = (int)((*(int *)(c + 0x98) * 0xf60LL + 0x800) >> 12);
            if (*(u8 *)(c + 0x6de) == 0) {
                *(int *)(c + 0x98) = 0;
                *(int *)(c + 0xa8) = 0;
                _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x51, 0x40000000, 0x1000, 0);
                *(u8 *)(c + 0x70c) = 1;
            }
        } else if (*(u16 *)(c + 0x6a4) == 1) {
            _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x56, 0, 0x1000, 0);
            *(int *)(c + 0x98) = 0x1a000;
            *(int *)(c + 0xa8) = 0x38000;
            *(u8 *)(c + 0x6de) = 1;
            *(u8 *)(c + 0x6df) = 0;
        }
        v[0] = *(int *)(c + 0x5c);
        v[1] = *(int *)(c + 0x60);
        v[2] = *(int *)(c + 0x64);
        v[1] = ((int *)c)[0x18] + 0x3c000;
        *(int *)(c + 0x628) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(volatile int *)(c + 0x628), 0x27, v[0], v[1], *(int *)(c + 0x64), 0, 0);
    } else if (_ZN6Player12FinishedAnimEv(c) != 0) {
        if (*(u8 *)(c + 0x70a) == 0x10) {
            *(u8 *)(c + 0x6e3) = 0x19;
            _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x99, 0x40000000, 0x1000, 0);
            *(int *)(c + 0x368) = 0;
            if (*(unsigned int *)(c + 8) <= 1) {
                *(int *)(c + 0x368) = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(
                    0x10d, (*(unsigned int *)(c + 8) << 8) | 0xf, c + 0x5c, 0,
                    *(s8 *)(c + 0xcc), -1);
            }
            switch (*(unsigned int *)(c + 8)) {
            case 0:
                _ZN5Sound9PlayBank0EjRK7Vector3(0xd9, c + 0x74);
                break;
            case 1:
                _ZN5Sound9PlayBank0EjRK7Vector3(0xda, c + 0x74);
                break;
            case 2:
                _ZN5Sound9PlayBank0EjRK7Vector3(0xdb, c + 0x74);
                break;
            }
        } else {
            int st;
            *(int *)(c + 0xa8) = 0;
            *(int *)(c + 0x688) = 0x187;
            func_ov002_020c7ff8(c);
            st = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(0xb2, 0x6f, c + 0x5c,
                                                              (int)(c + 0x8c),
                                                              *(s8 *)(c + 0xcc), -1);
            if (st != 0)
                func_ov002_020e7090(st, c);
        }
        *(u8 *)(c + 0x70c) = 0;
    }
    return 0;
}

