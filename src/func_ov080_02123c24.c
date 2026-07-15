extern void _ZN9Animation7AdvanceEv(void *anim);
extern int _ZN9Animation8FinishedEv(void *anim);
extern unsigned int _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    unsigned int a, unsigned int b, int x, int y, int z, void *vel, void *cb);
extern int RandomIntInternal(int *seed);
extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(char *anim, void *file, int a, int b, unsigned int u);
extern void *_ZN5Actor13ClosestPlayerEv(void *self);

struct Vector3 { int x, y, z; };

extern short Vec3_HorzAngle(const struct Vector3 *a, const struct Vector3 *b);
extern int AngleDiff(int a, int b);

extern int data_ov080_021276c4[];
extern int data_0209e650[];

struct G { int w[2]; };
extern struct G data_ov080_021283e0;
extern struct G data_ov080_021283d0;
extern struct G data_ov080_021283e8;

void func_ov080_02123c24(char *c)
{
    int amt;
    unsigned int state;
    int raw;

    _ZN9Animation7AdvanceEv(c + 0x124);
    raw = *(int *)(c + 0x12c);
    amt = 0;
    state = ((unsigned int)raw << 4) >> 16;

    if (state == 6) {
        int *p150 = (int *)(((int)c + 0x150) & 0xFFFFFFFFFFFFFFFF);
        int *pb0 = (int *)(((int)c + 0xb0) & 0xFFFFFFFFFFFFFFFF);
        *p150 = *p150 & ~1;
        *pb0 = *pb0 | 0x10000000;
    }

    if (state >= 6) {
        if (state >= 0x1a) {
            amt = 0x50000;
        } else {
            amt = data_ov080_021276c4[state - 6] << 12;
            *(unsigned int *)(c + 0x188) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                *(unsigned int *)(c + 0x188), 0x29,
                *(int *)(c + 0x5c), *(int *)(c + 0x60), *(int *)(c + 0x64),
                0, 0);
        }
    }

    *(int *)(c + 0x140) = amt;

    if (_ZN9Animation8FinishedEv(c + 0x124) == 0)
        return;

    *(unsigned int *)(c + 0x188) = 0;

    if (*(unsigned char *)(c + 0x180) == 2) {
        unsigned int rv = (unsigned int)RandomIntInternal(data_0209e650) >> 8;
        unsigned int rem = (rv % 3) & 0xff;
        if (rem == 0) {
            *(int *)(c + 0x17c) = 3;
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, (void *)data_ov080_021283e0.w[1], 0x40000000, 0x1000, 0);
            return;
        }
        if (rem == 1) {
            *(int *)(c + 0x17c) = 4;
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, (void *)data_ov080_021283d0.w[1], 0x40000000, 0x1000, 0);
            return;
        }
        *(int *)(c + 0x17c) = 5;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, (void *)data_ov080_021283e8.w[1], 0x40000000, 0x1000, 0);
        return;
    }

    {
        char *player = (char *)_ZN5Actor13ClosestPlayerEv(c);
        if (player == 0) {
            *(int *)(c + 0x17c) = 3;
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, (void *)data_ov080_021283e0.w[1], 0x40000000, 0x1000, 0);
            return;
        }
        {
            struct Vector3 pos;
            short horz;
            int diff;
            int *pb = (int *)(((int)player + 0x5c) & 0xFFFFFFFFFFFFFFFF);
            pos.x = pb[0];
            pos.y = pb[1];
            pos.z = pb[2];
            horz = Vec3_HorzAngle((struct Vector3 *)(c + 0x5c), &pos);
            diff = (short)AngleDiff(*(short *)(c + 0x8e), horz);
            if (diff < 0x4000) {
                *(int *)(c + 0x17c) = 3;
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, (void *)data_ov080_021283e0.w[1], 0x40000000, 0x1000, 0);
            } else {
                *(int *)(c + 0x17c) = 4;
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, (void *)data_ov080_021283d0.w[1], 0x40000000, 0x1000, 0);
            }
        }
    }
}
