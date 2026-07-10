//cpp
/* _ZN6Player16St_BurnLava_MainEv @ 0x020d5038 (ov002, size 0x264)
 * Player burn-in-lava state: kills the player once outside battle levels,
 * spawns smoke/steam particles with the burn SFX, then runs the 3-hop
 * bounce (halving y-velocity, hop height 0x14000/0xa000) until the burnt
 * anim finishes, then leaves via state change or death.
 */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef short s16;

extern u8 data_ov002_0211117c;
extern s8 data_0209f2f8;
extern s16 data_02082214[];
extern char data_ov002_0211013c;

extern "C" {
extern void KillPlayer(void);
extern int _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(int, unsigned int, int, int, int, s16 *, void *);
extern int _ZN5Sound8PlayLongEjjjRK7Vector3j(int, unsigned int, unsigned int, char *, unsigned int);
extern void func_ov002_020e28d4(char *, int, int);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(char *, unsigned int, int, int, unsigned int);
extern int _ZN6Player12FinishedAnimEv(char *);
extern int _ZN6Player9GetHealthEv(char *);
extern void func_ov002_020c5dec(char *, int);
extern void _ZN6Player11ChangeStateERNS_5StateE(char *, char *);
extern void func_ov002_020bedd4(char *);

int _ZN6Player16St_BurnLava_MainEv(char *c)
{
    if (data_ov002_0211117c == 0 && *(u8 *)(c + 0x70c) == 0) {
        KillPlayer();
        *(u8 *)(c + 0x70c) = 1;
        *(u8 *)(c + 0x713) = 0;
    }

    {
        s8 lv = data_0209f2f8;
        if (lv != 0x13 && lv != 0x31 && *(int *)(c + 0xa8) >= 0) {
            struct Info {
                s16 vec[3];
                int pos[3];
            } info;
            int zz = *(int *)(c + 0x64);
            int yy = *(int *)(c + 0x60);
            int xx = *(int *)(c + 0x5c);
            info.pos[0] = xx;
            info.pos[2] = zz - 0x1e000;
            info.pos[1] = yy;
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
    }

    if (*(u8 *)(c + 0x6e3) == 0) {
        func_ov002_020e28d4(c, 0x1000, 0x1000);
        if (*(u8 *)(c + 0x6de) == 0) {
            (*(u8 *)(int)(((long long)(int)(c + 0x6e5)) & 0xFFFFFFFFFFFFFFFFLL))++;
            if (*(u8 *)(c + 0x6e5) >= 3) {
                _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x17, 0x40000000, 0x1000, 0);
                *(int *)(c + 0x98) = 0;
                (*(u8 *)(int)(((long long)(int)(c + 0x6e3)) & 0xFFFFFFFFFFFFFFFFLL))++;
            } else {
                (*(int *)(int)(((long long)(int)(c + 0x98)) & 0xFFFFFFFFFFFFFFFFLL)) >>= 1;
                *(u8 *)(c + 0x6de) = 1;
                *(u8 *)(c + 0x6df) = 0;
                if (*(u8 *)(c + 0x6e5) == 1)
                    *(int *)(c + 0xa8) = 0x14000;
                else
                    *(int *)(c + 0xa8) = 0xa000;
            }
        }
    } else {
        if (_ZN6Player12FinishedAnimEv(c) != 0) {
            *(s16 *)(c + 0x94) = *(s16 *)(c + 0x8e);
            if (_ZN6Player9GetHealthEv(c) == 0)
                func_ov002_020c5dec(c, 2);
            else
                _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_0211013c);
        }
    }

    func_ov002_020bedd4(c);
    return 1;
}
}
