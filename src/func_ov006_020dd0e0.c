// @symbol func_ov006_020dd0e0
/* func_ov006_020dd0e0 -- dScMgCoin_c, ov006 0x020dd0e0, 0x1ec bytes.
 *
 * The touch test for one of the four cups. While the scene is in state 2 and the
 * stylus is both down and newly pressed, if the touch point is within 0x10 of
 * this cup's centre the cup is taken: a cup that still holds something starts
 * its hop (state 1, target y = the cup's own y, vy = -0x3000), otherwise the
 * round ends -- state 3, a 0x40 timer, the pan-positioned reveal jingle, and the
 * star count compared against the scene's target to set the win flag.
 *
 * FOUR THINGS ARE LOAD BEARING, ALL MEASURED AGAINST THE CARTRIDGE.
 *
 * 1. p1 IS ASSIGNED BEFORE p0.  The cup's y address has to be computed first;
 *    swapping the two lines costs seven words and no bytes are saved.
 * 2. THE TOUCH POINT IS RE-READ THROUGH THE GLOBAL.  ax/ay index
 *    data_020a0dea / data_020a0deb with `data_020a0e40 * 4`, not with the `i`
 *    that the pressed test above already holds.  The second read is what puts
 *    the pool base in r3 and the index byte in r2 the way the ROM does; with
 *    `i * 4` there the pair is exchanged, everything else identical.
 * 3. THERE IS NO `fld` BASE POINTER.  Each of the five cup-record stores spells
 *    its own `self + n + 0x4000 + ...`; hoisting one pointer for them exchanges
 *    r2 and r3 across the whole block.
 * 4. THE HOP'S TARGET Y RE-READS ITS ADDRESS.  `*(int *)(self + 0x4ac4 + n)`,
 *    not `*p1`, even though p1 is that address and is still live.
 *
 * No pragma: `#pragma opt_propagation off`, which earlier drafts carried, costs
 * six words here.
 */
typedef short s16;
typedef unsigned char u8;

extern u8 data_020a0e40;
extern u8 data_020a0de8[];
extern u8 data_020a0de9[];
extern u8 data_020a0dea[];
extern u8 data_020a0deb[];

extern void func_ov006_020dd4b0(char *c, int i);
extern void func_ov006_020dcb1c(char *c, int i);
extern void func_020127a4(int a, int b, int c, int d);
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int id);

extern void *data_ov004_020beb68;

void func_ov006_020dd0e0(char *self, int idx)
{
    int i;
    int ok;
    int n;
    int v;
    int w;
    int *p0;
    int *p1;
    int ax;
    int ay;
    int q0;
    int q1;
    int ang;
    int stars;
    int need;

    if (*(int *)(self + 0x5000 + 0x1c8) != 2)
        return;

    i = data_020a0e40;
    ok = 0;
    if (data_020a0de8[i * 4] != 0 && data_020a0de9[i * 4] != 0)
        ok = 1;
    if (ok == 0)
        return;

    n = idx * 0x18;
    p1 = (int *)(self + 0x4ac4 + n);
    p0 = (int *)(self + 0x4ac0 + n);
    q1 = *p1;
    ax = data_020a0dea[data_020a0e40 * 4];
    ay = data_020a0deb[data_020a0e40 * 4];
    q0 = *p0;
    v = ax - (q0 >> 12);
    w = ay - (q1 >> 12);

    if (v <= -0x10)
        return;
    if (v >= 0x10)
        return;
    if (w <= -0x10)
        return;
    if (w >= 0x10)
        return;

    *(u8 *)(self + n + 0x4000 + 0xad5) = 1;
    if (*(u8 *)(self + n + 0x4000 + 0xad3) != 0) {
        *(u8 *)(self + n + 0x4000 + 0xad0) = 1;
        *(int *)(self + n + 0x4000 + 0xac8) = *(int *)(self + 0x4ac4 + n);
        *(int *)(self + n + 0x4000 + 0xacc) = -0x3000;
        *(u8 *)(self + n + 0x4000 + 0xad6) = 0;
        func_ov006_020dd4b0(self, idx);
        return;
    }

    func_ov006_020dcb1c(self, idx);
    *(u8 *)(self + idx * 0x18 + 0x4000 + 0xad2) = 0;
    *(int *)(self + 0x5000 + 0x1c8) = 3;
    *(int *)(self + 0x5000 + 0x1cc) = 0x40;

    ang = (*p0 >> 12) - 0x80;
    ang >>= 1;
    if (ang >= 0x3c)
        ang = 0x3c;
    if (ang <= -0x3c)
        ang = -0x3c;
    func_020127a4(2, 0xee, 0xffff, ang);
    _ZN5Sound12PlayBank2_2DEj(0xf2);

    *(u8 *)(self + 0x4000 + idx * 0x18 + 0xad0) = 2;
    stars = (data_ov004_020beb68 != 0) ? *(int *)((char *)data_ov004_020beb68 + 0xa8) : 0;
    need = *(int *)(self + 0x5000 + 0x1d4);
    if (stars > need)
        *(u8 *)(self + 0x5000 + 0x1db) = 1;
    else
        *(u8 *)(self + 0x5000 + 0x1db) = 0;
}