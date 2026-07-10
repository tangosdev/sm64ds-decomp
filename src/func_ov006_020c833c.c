/* func_ov006_020c833c @ 0x020c833c (ov006, size 0x264)
 * Bob-omb/enemy bounce update: on despawn timer expiry cleans up; plays the
 * bounce SFX on anim frames 12/24 near the camera plane; reflects and damps
 * the x-velocity at +/-0x6c000, restarting the walk anim; handles touch
 * states 1/2; otherwise steers the facing angle toward the motion sign.
 */
typedef unsigned short u16;
typedef short s16;
typedef long long s64;

extern int data_ov006_0214041c;
extern char *data_ov006_0213b098[];
extern int data_ov006_0213b008;

extern void func_ov006_020c802c(char *);
extern void func_ov006_020c7c68(char *);
extern int _ZNK9Animation12WillHitFrameEi(char *, int);
extern void func_ov006_020e6e3c(int, int);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(char *, void *, int, int, unsigned int);
extern void func_ov006_020e6df0(int, int, int);
extern void func_ov006_020c8270(char *);
extern void func_ov006_020c7ba4(char *);
extern void _Z11UpdateAngleRssis(char *, int, int, int);

void func_ov006_020c833c(char *c)
{
    if (*(int *)(c + 0x24) < 0) {
        func_ov006_020c802c(c);
        func_ov006_020c7c68(c);
        return;
    }

    if (*(int *)(c + 0xac) == data_ov006_0214041c) {
        if (_ZNK9Animation12WillHitFrameEi(c + 0x9c, 0xc) != 0 ||
            _ZNK9Animation12WillHitFrameEi(c + 0x9c, 0x18) != 0)
            func_ov006_020e6e3c(0x1b5, *(int *)(c + 0x14));
    }

    if (*(int *)(c + 0x14) < -0x6c000 && *(int *)(c + 0x20) < 0) {
        *(int *)(c + 0x20) = -(int)((*(int *)(c + 0x20) * 0xd00LL + 0x800) >> 12);
        if (*(int *)(c + 0x24) > 0) {
            *(int *)(c + 0x44) = 2;
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x4c,
                *(void **)data_ov006_0213b098[*(int *)(c + 0x44)], 0x40000000, 0x800, 0);
            *(int *)(c + 0xa4) = 0;
            func_ov006_020e6df0(0, *(int *)(c + 0x44), *(int *)(c + 0x14));
        }
    } else if (*(int *)(c + 0x14) > 0x6c000 && *(int *)(c + 0x20) > 0) {
        *(int *)(c + 0x20) = -(int)((*(int *)(c + 0x20) * 0xd00LL + 0x800) >> 12);
        if (*(int *)(c + 0x24) > 0) {
            *(int *)(c + 0x44) = 2;
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x4c,
                *(void **)data_ov006_0213b098[*(int *)(c + 0x44)], 0x40000000, 0x800, 0);
            *(int *)(c + 0xa4) = 0;
            func_ov006_020e6df0(0, *(int *)(c + 0x44), *(int *)(c + 0x14));
        }
    } else {
        u16 t = *(u16 *)(c + 0x10);
        if (t == 1) {
            *(u16 *)(c + 0x10) = 0;
            *(int *)(c + 0x24) = data_ov006_0213b008;
            func_ov006_020c8270(c);
            return;
        }
        if (t == 2) {
            *(u16 *)(c + 0x10) = 0;
            func_ov006_020c7ba4(c);
            return;
        }
    }

    if (*(int *)(c + 0x20) > 0)
        _Z11UpdateAngleRssis(c + 0x2e, 0x2800, 2, 0x1000);
    else
        _Z11UpdateAngleRssis(c + 0x2e, -0x2800, 2, 0x1000);
}
