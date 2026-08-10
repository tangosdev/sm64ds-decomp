/* func_ov065_021183c8 @ 0x021183c8 (ov065, size 0x26c)
 * Swing/lift platform update: ramps the swing speed up (state 0) to 0x64000
 * or down (state 1) to rest; when the tracked player is idle on it, is below
 * the trigger height, close enough horizontally, and accepts the no-control
 * state, latches them (anim restart) and remembers the ride.
 */
typedef unsigned short u16;
typedef unsigned char u8;

extern char data_ov065_0211d748[];

extern int _ZN9Animation8FinishedEv(char *);
extern u16 DecIfAbove0_Short(char *);
extern int func_ov065_02119210(int);
extern int Vec3_HorzDist(int *, char *);
extern int _ZN6Player17SetNoControlStateEhih(char *, int, int, int);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(char *, int, int, int, unsigned int);

void func_ov065_021183c8(char *c)
{
    int v[3];
    u8 st = *(u8 *)(c + 0x11b0);

    if (st == 0) {
        (*(int *)(int)(c + 0x11a8)) += 0xa000;
        if (*(int *)(c + 0x11a8) >= 0x64000) {
            *(int *)(c + 0x11a8) = 0x64000;
            (*(u8 *)(int)(c + 0x11b0))++;
        }
    } else if (st == 1) {
        (*(int *)(int)(c + 0x11a8)) -= 0x5000;
        if (*(int *)(c + 0x11a8) <= 0) {
            *(int *)(c + 0x11a8) = 0;
            (*(u8 *)(int)(c + 0x11b0))++;
        }
    }

    if (_ZN9Animation8FinishedEv(c + 0x13c) == 0)
        return;

    (*(u16 *)(int)(c + 0x11b2))++;
    DecIfAbove0_Short(c + 0xe6);

    if (*(int *)(c + 0x118c) != 0) {
        if (*(u8 *)(c + 0x11b5) == 2 && *(u16 *)(c + 0xe6) == 0) {
            if (*(int *)(c + 0xd4) != 0 && func_ov065_02119210(*(int *)(c + 0xd4)) != 0)
                goto tail;
            {
                int *q = (int *)(int)(*(char **)(c + 0x118c) + 0x5c);
                v[0] = q[0];
                v[1] = q[1];
                v[2] = q[2];
            }
            if (v[1] < *(int *)(c + 0x1184))
                goto tail;
            if (Vec3_HorzDist(v, c + 0x1180) >= 0x88000)
                goto tail;
            if (_ZN6Player17SetNoControlStateEhih(*(char **)(c + 0x118c), 5, -1, 0) == 0)
                goto tail;
            (*(u8 *)(int)(c + 0x11b4))++;
            *(u8 *)(c + 0x11b0) = 0;
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xec,
                *(int *)(data_ov065_0211d748 + 4), 0x40000000, 0x1000, 0);
            *(int *)(c + 0x1190) = *(int *)(c + 0x118c);
        }
    tail:
        if (Vec3_HorzDist((int *)(c + 0x5c), *(char **)(c + 0x118c) + 0x5c) < 0x1f4000)
            *(u16 *)(c + 0x11b2) = 0;
    } else {
        if (*(u16 *)(c + 0x11b2) > 0x96) {
            (*(u8 *)(int)(c + 0x11b4))++;
            *(u8 *)(c + 0x11b0) = 0;
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xec,
                *(int *)(data_ov065_0211d748 + 4), 0x40000000, 0x1000, 0);
        }
        (*(u16 *)(int)((char *)(int)(c) + 0x11b2))++;
    }
}



