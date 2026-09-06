/* func_ov006_020e1854 -- the pen handler for the ov006 slot-machine style scene,
 * 0x020e1854, 0x300 bytes.
 *
 * While the stylus is down it drags the object: the touch point plus the stored
 * grab offset becomes the new position, clamped into the 0x20000..0xe0000 x
 * 0x94000..0xb8000 box.  A drag shorter than two units is undone outright.  What
 * survives updates the swing bookkeeping (0x4eea's three-state direction
 * machine), turns the drag vector into an angle at 0x4ede, clamps that angle
 * into the lower half turn, averages it with the previous one, and folds the
 * drag length into the 0x4ec8 speed with a decay when it falls.  Finally the
 * grab offset is re-derived from the new position so the next tick drags from
 * the same spot on the object.  With the stylus up, two flags are reset.
 *
 * THREE THINGS ARE LOAD BEARING AND ALL THREE WERE MEASURED.
 *
 * 1. THE TOUCH TABLE IS REACHED THROUGH A BYTE VIEW OF A RECORD ARRAY.
 *    data_020a0dea and data_020a0deb are four-byte records (the B4 shape that
 *    src/minigames/d_s_mg_trampoline.cpp already declares for the same
 *    globals), and this file takes a `u8 *` of each before indexing.  A plain
 *    `u8 data_020a0dea[]` extern with no cast, `&arr[0]`, `arr + 0`, or a u8*
 *    alias of a u8 array all cost eighteen more words: the cast has to cross a
 *    type boundary for 2004/b56 to keep the ROM's addressing.
 *
 * 2. dx IS COMPUTED BEFORE dy, AND NEITHER SUBTRACTION GETS A NAMED TEMPORARY.
 *    Writing dy first, or routing either difference through an `int t`, leaves
 *    the ROM's shape four words off: the cartridge loads the new x into a fresh
 *    register and puts both subtractions in the register the base pointer just
 *    vacated, while the other orders reuse the dying base for the load and
 *    subtract in place.
 *
 * 3. THE TAIL BLOCK'S SLOT INDEX IS WIDER THAN A BYTE.  `unsigned short j`
 *    (int, short and unsigned int are byte-identical to it); `u8 j` costs six
 *    words in the two table reads under it.
 *
 * `#pragma opt_common_subs off` is real: the two 0x4eb0/0x4eb4 re-reads after
 * the clamps have to stay re-reads.
 */
struct B4 { unsigned char v; unsigned char pad[3]; };

#pragma opt_common_subs off
typedef unsigned char u8;
typedef unsigned short u16;
typedef long long s64;
extern u8 data_020a0e40[];
extern u8 data_020a0de8[];
extern struct B4 data_020a0dea[];
extern struct B4 data_020a0deb[];
extern int func_0203d744(long long x);
extern void func_02012718(int a, int b);
extern u16 func_0203b4dc(int a, int b);

void func_ov006_020e1854(void *arg)
{
    u8 *c = (u8 *)arg;
    u8 idx;
    int dx, oldx, oldy;
    int diff;
    u16 ang;

    idx = data_020a0e40[0];
    if (data_020a0de8[idx * 4] != 0) {
        int dy2, dy;

        int i4 = idx * 4;
        u8 *pa = (u8 *)data_020a0dea;
        u8 *pb = (u8 *)data_020a0deb;
        u8 bx = pa[i4];
        u8 by = pb[i4];

        oldx = *(int *)(c + 0x4eb0);
        oldy = *(int *)(c + 0x4eb4);
        *(int *)(c + 0x4eb0) = (bx << 12) + *(int *)(c + 0x4ec0);
        *(int *)(c + 0x4eb4) = *(int *)(c + 0x4ec4) + (by << 12);

        if (*(int *)(c + 0x4eb4) <= 0x94000)
            *(int *)(c + 0x4eb4) = 0x94000;
        if (*(int *)(c + 0x4eb0) <= 0x20000)
            *(int *)(c + 0x4eb0) = 0x20000;
        if (*(int *)(c + 0x4eb0) >= 0xe0000)
            *(int *)(c + 0x4eb0) = 0xe0000;
        if (*(int *)(c + 0x4eb4) >= 0xb8000)
            *(int *)(c + 0x4eb4) = 0xb8000;


        dx = (*(int *)(c + 0x4eb0) - oldx) >> 12;
        dy = (*(int *)(c + 0x4eb4) - oldy) >> 12;
        dy2 = dy * dy;

        if (func_0203d744((s64)(dx * dx + dy2)) <= 1) {
            *(int *)(c + 0x4eb0) = oldx;
            *(int *)(c + 0x4eb4) = oldy;
            return;
        }

        diff = (*(int *)(c + 0x4eb4) - *(int *)(c + 0x4ebc)) >> 12;
        if (*(u8 *)(c + 0x4eea) == 0) {
            func_02012718(0x1d6, *(int *)(c + 0x4eb0));
            *(u8 *)(c + 0x4eea) = 2;
            *(int *)(c + 0x4ed4) = (*(int *)(c + 0x4eb4) - *(int *)(c + 0x4ebc)) >> 12;
            *(int *)(c + 0x4ebc) = *(int *)(c + 0x4eb4);
        } else if (*(u8 *)(c + 0x4eea) == 1) {
            if (*(int *)(c + 0x4ed4) * diff > 0) {
                if (diff < 0)
                    diff = -diff;
                if (diff >= 0xa)
                    *(u8 *)(c + 0x4eea) = 0;
            } else {
                *(int *)(c + 0x4ed4) = diff;
                *(int *)(c + 0x4ebc) = *(int *)(c + 0x4eb4);
            }
        } else {
            if (*(int *)(c + 0x4ed4) * diff < 0)
                *(u8 *)(c + 0x4eea) = 1;
            *(int *)(c + 0x4ed4) = (*(int *)(c + 0x4eb4) - *(int *)(c + 0x4ebc)) >> 12;
            *(int *)(c + 0x4ebc) = *(int *)(c + 0x4eb4);
        }

        ang = *(u16 *)(c + 0x4ede);
        *(u16 *)(c + 0x4ede) = func_0203b4dc(dy, dx >> 1);
        {
            u16 a = *(u16 *)(c + 0x4ede);
            if (a <= 0x8000) {
                if (a >= 0x4000) {
                    *(u16 *)(c + 0x4ede) = 0x8000;
                    goto ang_done;
                }
            }
            if (a <= 0x4000)
                *(u16 *)(c + 0x4ede) = 0;
        }
    ang_done:;

        {
            int mag;
            *(u16 *)(c + 0x4ede) = (u16)((*(u16 *)(c + 0x4ede) + ang) >> 1);
            mag = func_0203d744((s64)((dx >> 1) * (dx >> 1) + dy2)) * 9;
            mag = (mag << 12) >> 4;
            if (mag >= 0xc000)
                mag = 0xc000;
            if (mag > *(int *)(c + 0x4ec8))
                *(int *)(c + 0x4ec8) = mag;
            {
                int cur = *(int *)(c + 0x4ec8);
                if (cur > mag) {
                    *(int *)(c + 0x4ec8) = *(int *)(c + 0x4ec8) - ((cur - mag) >> 1);
                }
            }
        }


        {
            int px = *(int *)(c + 0x4eb0);
            int py = *(int *)(c + 0x4eb4);
            int j = data_020a0e40[0];
            u8 jx = ((u8 *)data_020a0dea)[j * 4];
            int ax = (px >> 12) - jx;
            u8 jy = ((u8 *)data_020a0deb)[j * 4];
            int ay = (py >> 12) - jy;
            *(int *)(c + 0x4ec0) = ax << 12;
            *(int *)(c + 0x4ec4) = ay << 12;
        }

        return;
    }

    *(u8 *)(c + 0x4ee4) = 0;
    *(u8 *)(c + 0x4ee5) = 1;
}
