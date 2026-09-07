//cpp
// @symbol func_ov006_020fe394
/* func_ov006_020fe394 -- dScMgPachinko_c, ov006 0x020fe394, 0x3bc bytes.
 *
 * One ball's stylus frame. While the stylus is down the ball follows the touch
 * point through its grab offset, clamped into the 8..0xf8 x 8..0xb8 box, the
 * offset is re-derived, and the drag-speed sound is re-armed at 0x17b or 0x17c
 * depending on whether the distance from the launcher grew by more than ten.
 * On release the launch fires: the pull-back vector becomes an angle at 0x4f08
 * and a speed at 0x4ef8 (the square root scaled by 1.953, clamped to 0xa000),
 * that speed drives the sine table into the velocity pair at 0x4ee0/0x4ee4, and
 * the ball is re-seated at 0x80000, 0x28000 if the pull was too short.
 *
 * FOUR THINGS ARE LOAD BEARING, ALL MEASURED, AND THEY ARE WHY THIS SAT AT 48
 * DIVERGENCES FOR THREE EARLIER RUNS.  None of them is visible in the ROM as a
 * missing or extra instruction -- every one only moves registers.
 *
 * 1. dy IS COMPUTED BEFORE dx IN BOTH DISTANCE BLOCKS.  The drag block and the
 *    launch block each take `dy = 0x80 - (x >> 12)` first.  Swapping either pair
 *    costs seven words; that one decision is 48 -> 41 and again 8 -> 2.
 *
 * 2. THE SECOND SINE READ GOES THROUGH A NAMED `const s16 *`.  Both velocity
 *    words read data_02082214 at the same angle, and 2004/b56 will otherwise
 *    share one table base across them.  A const pointer to the second entry
 *    breaks that and closes three whole windows (41 -> 24).  It has to be const
 *    -- a non-const pointer changes the size -- and it has to be the SECOND read
 *    only: doing both is 35, doing only the first is 47.
 *
 * 3. THE LAST y CLAMP STORES THROUGH A POINTER ASSIGNED MID-RUN.  `py` is set
 *    between the two x clamps and used only by `if (vy <= 8)` (24 -> 8).
 *    Assigning it before the block is 55 and reading vy through it is 76.
 *
 * 4. THE FIRST VELOCITY WORD'S TABLE INDEX IS A NAMED LOCAL.  Spelling the
 *    subscript inline leaves the ROM's two loads exchanged, r6 against r7
 *    (8 -> 0).  A block-scope `int` is what does it; the same name at function
 *    scope beside cx/cy does not compile to the cartridge.
 *
 * `#pragma opt_propagation off` is real and was measured against a delete-outright
 * control.  The class header is untouched: this function needs no new fields.
 */
#include "dScMgPachinko_c.h"
#pragma opt_propagation off

extern "C" {
extern u8 data_020a0e40;
extern u8 data_020a0de8[][4];
extern u8 data_020a0dea[][4];
extern u8 data_020a0deb[][4];
extern const s16 data_02082214[];
extern s32 _ZN4cstd4sqrtEy(u64 v);
extern s16 _ZN4cstd5atan2E5Fix12IiES1_(Fix12i y, Fix12i x);
extern s32 Sound_PlayIfNotActive(s32 handle, s32 player, s32 id, s32 unused);
extern void _ZN5Sound12PlayBank2_2DEj(u32 id);
extern void func_ov006_020fbd38(dScMgPachinko_c *self);
}

extern "C" void func_ov006_020fe394(dScMgPachinko_c *self, int i)
{
    char *c = (char *)self;
    s32 cx;
    s32 *py;
    s32 cy;
    u32 idx = data_020a0e40;

    if (data_020a0de8[idx][0] != 0) {
        int off = i * 0x38;
        s32 vy, vx;

        cx = data_020a0dea[idx][0];
        cy = data_020a0deb[idx][0];
        *(s32 *)(c + 0x4ed8 + off) = (cx << 12) + *(s32 *)(c + 0x4ee8 + off);
        *(s32 *)(c + 0x4edc + off) = (cy << 12) + *(s32 *)(c + 0x4eec + off);

        vy = *(s32 *)(c + 0x4edc + off) >> 12;
        vx = *(s32 *)(c + 0x4ed8 + off) >> 12;
        if (vx >= 0xf8)
            *(s32 *)(c + 0x4ed8 + off) = 0xf8000;
        py = (s32 *)(c + 0x4edc + off);
        if (vx <= 8)
            *(s32 *)(c + 0x4ed8 + off) = 0x8000;
        if (vy >= 0xb8)
            *py = 0xb8000;
        if (vy <= 8)
            *py = 0x8000;

        {
            s32 nx = (*(s32 *)(c + 0x4ed8 + off) >> 12) - data_020a0dea[data_020a0e40][0];
            s32 ny = (*(s32 *)(c + 0x4edc + off) >> 12) - data_020a0deb[data_020a0e40][0];

            *(s32 *)(c + 0x4ee8 + off) = nx << 12;
            *(s32 *)(c + 0x4eec + off) = ny << 12;
        }

        {
            s32 dy = 0x80 - (*(s32 *)(c + 0x4ed8 + off) >> 12);
            s32 dx = 0x20 - (*(s32 *)(c + 0x4edc + off) >> 12);
            s32 dist = _ZN4cstd4sqrtEy((u64)(s64)(dy * dy + dx * dx));
            s32 prev = *(s32 *)(c + 0x4f04 + off);

            *(s32 *)(c + 0x4f04 + off) = dist;
            if (dist > prev) {
                if (dist > prev + 10)
                    *(s32 *)(c + 0x4f00 + off) = Sound_PlayIfNotActive(
                        *(s32 *)(c + 0x4f00 + off), 2, 0x17b, 0);
                else
                    *(s32 *)(c + 0x4f00 + off) = Sound_PlayIfNotActive(
                        *(s32 *)(c + 0x4f00 + off), 2, 0x17c, 0);
            }
        }
    } else {
        int off = i * 0x38;
        s32 dy, dx, dist;

        *(u8 *)(c + 0x4f0d + off) = 2;
        dy = 0x80 - (*(s32 *)(c + 0x4ed8 + off) >> 12);
        dx = 0x20 - (*(s32 *)(c + 0x4edc + off) >> 12);
        dist = _ZN4cstd4sqrtEy((u64)(s64)(dy * dy + dx * dx));
        if (dist >= 0x10) {
            {
                s16 ang = _ZN4cstd5atan2E5Fix12IiES1_(dx, dy);
                char *p = c + off;

                *(s16 *)(p + 0x4f08) = ang;
            }
            *(s32 *)(c + 0x4ef8 + off) =
                _ZN4cstd4sqrtEy((u64)(s64)(dy * dy + dx * dx)) << 8;
            *(s32 *)(c + 0x4ef8 + off) += *(s32 *)(c + 0x4ef8 + off) >> 2;
            *(s32 *)(c + 0x4ef8 + off) += *(s32 *)(c + 0x4ef8 + off) >> 2;
            *(s32 *)(c + 0x4ef8 + off) += *(s32 *)(c + 0x4ef8 + off) >> 2;
            if (*(s32 *)(c + 0x4ef8 + off) >= 0xa000)
                *(s32 *)(c + 0x4ef8 + off) = 0xa000;
            self->unk_5c1c = (u16)(*(s32 *)(c + 0x4ef8 + off) >> 10);
            if (self->unk_5c1c == 0)
                self->unk_5c1c = 1;
            *(u8 *)(c + 0x5c31) = 0;
            {
                int ai = (*(u16 *)(c + 0x4f08 + off) >> 4) * 2 + 1;
                *(s32 *)(c + 0x4ee0 + off) = (s32)(((s64)data_02082214[ai] *
                    *(s32 *)(c + 0x4ef8 + off) + 0x800) >> 12);
            }
            {
                const s16 *sp = &data_02082214[(*(u16 *)(c + 0x4f08 + off) >> 4) * 2];
                *(s32 *)(c + 0x4ee4 + off) = (s32)(((s64)*sp * *(s32 *)(c + 0x4ef8 + off) + 0x800) >> 12);
            }
            *(s16 *)(c + 0x4f0a + off) = 0;
            if (dist >= 0x40)
                _ZN5Sound12PlayBank2_2DEj(0x17e);
            else
                _ZN5Sound12PlayBank2_2DEj(0x17d);
        } else {
            *(s32 *)(c + 0x4ed8 + off) = 0x80000;
            *(s32 *)(c + 0x4edc + off) = 0x28000;
            *(u8 *)(c + 0x4f0d + off) = 0;
        }
    }
    func_ov006_020fbd38(self);
}
