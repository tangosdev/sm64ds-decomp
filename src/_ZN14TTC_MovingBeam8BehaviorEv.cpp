//cpp
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;

extern "C" {
u16 DecIfAbove0_Short(u16 *p);
int RandomIntInternal(int *seed);
void _ZN8dActor_c9UpdatePosEP5dCc_c(void *self, void *clsn);
void _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void *self);
void func_ov065_0211bc88(char *c);
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void *self);

extern u8 data_0209f2c0;
extern s32 data_ov065_0211d520[];
extern s32 data_0209e650[];
}

class TTC_MovingBeam {
public:
    int Behavior();
};

/* Slot 6 of _ZTV14TTC_MovingBeam (ov065, 0x0211bd8c, 0x178 bytes).
 *
 * data_0209f2c0 is the Tick Tock Clock speed setting: 0 and 1 are the two fixed
 * speeds, 2 is the random ("hands spin") mode, 3 is stopped. It indexes the four
 * s32 speeds at data_ov065_0211d520; the beam's per-frame vertical speed at +0xa8
 * is that speed times the direction sign at +0x328.
 *
 * In mode 2 a countdown at +0x32a runs down each frame; when it expires a new
 * direction and a new countdown -- ((rand % 6) + 1) * 30 frames -- are rolled, and
 * the full length is remembered at +0x32c. Within the last 5 frames of a leg the
 * beam is held still.
 *
 * Then the beam is moved, clamped to the [+0x320, +0x324] travel range read from
 * its spawn parameters, and reversed when it leaves that range.
 */
int TTC_MovingBeam::Behavior()
{
    char *c = (char *)this;
    u8 idx = data_0209f2c0;
    s32 v = *(signed char *)(c + 0x300 + 0x28);
    *(s32 *)(c + 0xa8) = v * data_ov065_0211d520[idx];

    if (idx == 2) {
        if (DecIfAbove0_Short((u16 *)(c + 0x32a)) == 0) {
            u16 rnd = (u32)RandomIntInternal(data_0209e650) >> 16;
            if (rnd >= 0x7fff) {
                *(signed char *)(c + 0x328) = 1;
            } else {
                *(signed char *)(c + 0x328) = -1;
            }
            int r = rnd % 6 + 1;
            *(u16 *)(c + 0x300 + 0x2a) = r * 0x1e;
            *(u16 *)(c + 0x300 + 0x2c) = *(u16 *)(c + 0x300 + 0x2a);
        } else {
            int a = *(u16 *)(c + 0x300 + 0x2c);
            int b = *(u16 *)(c + 0x300 + 0x2a);
            if (b >= a - 5) {
                *(s32 *)(c + 0xa8) = 0;
            }
        }
    }

    _ZN8dActor_c9UpdatePosEP5dCc_c(c, 0);

    {
        int y = *(s32 *)(c + 0x60);
        int lo = *(s32 *)(c + 0x320);
        int hi = *(s32 *)(c + 0x324);
        int in = 0;
        if (y >= lo)
            in = (y <= hi);
        if (in == 0) {
            *(s32 *)(c + 0x60) = (y < lo) ? lo : ((y > hi) ? hi : y);
            *(signed char *)(c + 0x328) = -*(signed char *)(c + 0x300 + 0x28);
        }
    }

    _ZN10dBgActor_c21UpdateModelPosAndRotYEv(c);
    func_ov065_0211bc88(c);
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(c, 0, 0) != 0)
        _ZN10dBgActor_c19UpdateClsnPosAndRotEv(c);
    return 1;
}
