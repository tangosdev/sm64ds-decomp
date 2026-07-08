// NONMATCHING: push-set / frame (div=78). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
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
extern int RNG_STATE[];

struct G { int w[2]; };
extern struct G data_ov080_021283e0;
extern struct G data_ov080_021283d0;
extern struct G data_ov080_021283e8;

void func_ov080_02123c24(char *c)
{
    int raw;
    int amt;
    unsigned short state;

    _ZN9Animation7AdvanceEv(c + 0x124);
    raw = *(int *)(c + 0x12c);
    amt = 0;
    state = (unsigned short)(raw >> 12);

    if (state == 6) {
        *(unsigned int *)(c + 0x150) &= ~1;
        *(unsigned int *)(c + 0xb0) |= 0x10000000;
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

    if (!_ZN9Animation8FinishedEv(c + 0x124))
        return;

    *(unsigned int *)(c + 0x188) = 0;

    if (*(unsigned char *)(c + 0x180) == 2) {
        unsigned int rv = (unsigned int)RandomIntInternal(RNG_STATE) >> 8;
        unsigned char rem = (unsigned char)(rv % 3);
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
        void *player = _ZN5Actor13ClosestPlayerEv(c);
        if (!player) {
            *(int *)(c + 0x17c) = 3;
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, (void *)data_ov080_021283e0.w[1], 0x40000000, 0x1000, 0);
            return;
        }
        {
            struct Vector3 pos;
            short horz;
            short diff;
            int *pb = (int *)((char *)player + 0x5c);
            pos.x = pb[0];
            pos.y = pb[1];
            pos.z = pb[2];
            horz = Vec3_HorzAngle((struct Vector3 *)(c + 0x5c), &pos);
            diff = (short)AngleDiff(*(short *)(c + 0x8e), horz);
            if (diff >= 0x4000) {
                *(int *)(c + 0x17c) = 4;
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, (void *)data_ov080_021283d0.w[1], 0x40000000, 0x1000, 0);
            } else {
                *(int *)(c + 0x17c) = 3;
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, (void *)data_ov080_021283e0.w[1], 0x40000000, 0x1000, 0);
            }
        }
    }
}
