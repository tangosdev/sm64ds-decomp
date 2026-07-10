/* _ZN4Clam8BehaviorEv @ 0x0211aa58 (ov064, size 0x26c)
 * Chain-chomp-like enemy bark/lunge update: state 0 barks on anim frame
 * 0x19 (smoke puff, 10-frame shake), lunges (state 1) after 150 frames if
 * the player is near; state 1 returns to idle after 150 frames, toggling
 * the collision bit on anim frames 8/15. Hurts a touched player each frame.
 */
typedef unsigned short u16;
typedef unsigned char u8;

extern char data_ov064_0211c9cc[];
extern char data_ov064_0211c9bc[];

extern int _ZNK9Animation12WillHitFrameEi(char *, int);
extern void func_0201267c(int, char *);
extern int _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int, int, int, int);
extern int _ZN5Actor13DistToCPlayerEv(char *);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(char *, int, int, int, unsigned int);
extern char *_ZN5Actor10FindWithIDEj(int);
extern void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(char *, int *, unsigned int, int, unsigned int, unsigned int, unsigned int);
extern void _ZN9Animation7AdvanceEv(char *);
extern void _ZN12CylinderClsn5ClearEv(char *);
extern void _ZN12CylinderClsn6UpdateEv(char *);

int _ZN4Clam8BehaviorEv(char *c)
{
    struct {
        int v1[3];
        int v2[3];
        int v3[3];
    } u;
    switch (*(u8 *)(c + 0x16c)) {
    case 0:
        if (_ZNK9Animation12WillHitFrameEi(c + 0x124, 0x19) != 0) {
            func_0201267c(0x27, c + 0x74);
            u.v1[0] = *(int *)(c + 0x5c);
            u.v1[1] = *(int *)(c + 0x60);
            u.v1[2] = *(int *)(c + 0x64);
            u.v1[1] = u.v1[1] + 0x32000;
            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x10c, u.v1[0], u.v1[1], u.v1[2]);
            *(u16 *)(c + 0x170) = 0xa;
            *(u16 *)(c + 0x16e) = 0;
            *(int *)(int)(((long long)(int)(c + 0x150)) & 0xFFFFFFFFFFFFFFFFLL) &= ~1;
        } else {
            if (*(u16 *)(c + 0x16e) > 0x96 &&
                _ZN5Actor13DistToCPlayerEv(c) < 0x1f4000) {
                func_0201267c(0x26, c + 0x74);
                u.v2[0] = *(int *)(c + 0x5c);
                u.v2[1] = *(int *)(c + 0x60);
                u.v2[2] = *(int *)(c + 0x64);
                u.v2[1] = u.v2[1] + 0x32000;
                _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x10b, u.v2[0], u.v2[1], u.v2[2]);
                *(u16 *)(c + 0x16e) = 0;
                *(u8 *)(c + 0x16c) = 1;
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4,
                    *(int *)(data_ov064_0211c9cc + 4), 0x40000000, 0x1000, 0);
            } else {
                if (*(u16 *)(c + 0x170) != 0)
                    (*(u16 *)(int)(((long long)(int)(c + 0x170)) & 0xFFFFFFFFFFFFFFFFLL))--;
            }
        }
        break;
    case 1:
        if (*(u16 *)(c + 0x16e) > 0x96) {
            *(u8 *)(c + 0x16c) = 0;
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4,
                *(int *)(data_ov064_0211c9bc + 4), 0x40000000, 0x1000, 0);
            *(u16 *)(c + 0x16e) = 0;
        } else {
            if (_ZNK9Animation12WillHitFrameEi(c + 0x124, 8) == 0) {
                if (_ZNK9Animation12WillHitFrameEi(c + 0x124, 0xf) != 0)
                    *(int *)(int)(((long long)(int)(c + 0x150)) & 0xFFFFFFFFFFFFFFFFLL) |= 1;
            }
        }
        break;
    }

    (*(u16 *)(int)(((long long)(int)(c + 0x16e)) & 0xFFFFFFFFFFFFFFFFLL))++;

    if (*(int *)(c + 0x15c) != 0) {
        char *p = _ZN5Actor10FindWithIDEj(*(int *)(c + 0x15c));
        if (p != 0) {
            int isPlayer = (int)(((long long)(*(u16 *)(p + 0xc) == 0xbf)) & 0xFFFFFFFFFFFFFFFFLL);
            if (isPlayer != 0) {
                u.v3[0] = *(int *)(c + 0x5c);
                u.v3[1] = *(int *)(c + 0x60);
                u.v3[2] = *(int *)(c + 0x64);
                _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(p, u.v3, 2, 0xc000, 1, 0, 1);
            }
        }
    }

    _ZN9Animation7AdvanceEv(c + 0x124);
    _ZN12CylinderClsn5ClearEv(c + 0x138);
    _ZN12CylinderClsn6UpdateEv(c + 0x138);
    return 1;
}
