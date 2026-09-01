//cpp
// @symbol _ZN12dScMgAmida_c9Virtual88Eiiii
/* dScMgAmida_c::Virtual88 - slot 34, and the only override in the family that
   CALLS the base rather than replacing it.

   Amida is the ghost-leg game: you draw a line and it must not cross one that
   is already there.  So this override is the collision half and the base is the
   drawing half.  Before painting it probes the 0x158-stride occupancy grid at
   +0x4710 -- the cell under (x, y), and for a diagonal step also the two cells
   the diagonal would cut between -- and sets the foul flag at +0x4709 if any of
   them is already >= 3.  Then it calls the base brush, with a size of 2 or 4
   chosen per case rather than passed in, which is why this body has one fewer
   explicit parameter than the slot's signature: the fourth argument arrives on
   the stack and is never read.

   +0x46f8 / +0x46fc are the previous point, which is how it knows a step is
   diagonal at all. */
#include "types.h"
#include "dScMgAmida_c.h"
typedef struct Ctx {
    u8 pad_0000[0x46d8];
    int unk46d8;
    int unk46dc;
    int unk46e0;
    int unk46e4;
    u8 pad_46e8[0x10];
    int unk46f8;
    int unk46fc;
    u8 pad_4700[4];
    u8 unk4704;
    u8 unk4705;
    u8 unk4706;
    u8 unk4707;
    u8 unk4708;
    u8 unk4709;
    u8 unk470a;
    u8 pad_470b;
    u8* unk470c;
    u8* unk4710;
} Ctx;

void dScMgAmida_c::Virtual88(int y, int x, int arg3, int /* size */)
{
    Ctx *ctx = (Ctx *)this;

    u8 f705;
    u8 f707;
    int x2;

    if (ctx->unk4708 == 1) {
        int py;
        int pym;
        int pyp;
        if ((ctx->unk4710 + y * 0x158)[x + 0xc0] >= 3u) {
            ctx->unk4709 = 1;
        }
        py = ctx->unk46f8;
        if (y != py || x != ctx->unk46fc) {
            pym = py - 1;
            if (y == pym && x == ctx->unk46fc - 1) {
                u8* b = ctx->unk4710;
                if ((b + (y + 1) * 0x158)[x + 0xc0] >= 3u) {
                    ctx->unk4709 = 1;
                } else if ((b + y * 0x158)[x + 0xc1] >= 3u) {
                    ctx->unk4709 = 1;
                }
            } else {
                pyp = py + 1;
                if (y == pyp && x == ctx->unk46fc - 1) {
                    u8* b = ctx->unk4710;
                    if ((b + (y - 1) * 0x158)[x + 0xc0] >= 3u) {
                        ctx->unk4709 = 1;
                    } else if ((b + y * 0x158)[x + 0xc1] >= 3u) {
                        ctx->unk4709 = 1;
                    }
                } else if (y == pym && x == ctx->unk46fc + 1) {
                    u8* b = ctx->unk4710;
                    if ((b + (y + 1) * 0x158)[x + 0xc0] >= 3u) {
                        ctx->unk4709 = 1;
                    } else if ((b + y * 0x158)[x + 0xbf] >= 3u) {
                        ctx->unk4709 = 1;
                    }
                } else if (y == pyp && x == ctx->unk46fc + 1) {
                    u8* b = ctx->unk4710;
                    if ((b + (y - 1) * 0x158)[x + 0xc0] >= 3u) {
                        ctx->unk4709 = 1;
                    } else if ((b + y * 0x158)[x + 0xbf] >= 3u) {
                        ctx->unk4709 = 1;
                    }
                }
            }
        }
        ctx->unk46f8 = y;
        ctx->unk46fc = x;
        return;
    }

    if (ctx->unk4706 == 1) {
        dScMgBase_c::Virtual88(y, x, arg3, 4);
        if (y < 0) return;
        if (y >= 0x100) return;
        if (x < -0xc0) return;
        if (x >= 0x98) return;
        (ctx->unk4710 + y * 0x158)[x + 0xc0] = ctx->unk470a;
        return;
    }

    f705 = ctx->unk4705;
    if (f705 == 1) {
        dScMgBase_c::Virtual88(y, x, arg3, 2);
        return;
    }

    f707 = ctx->unk4707;
    x2 = x + 0xc0;
    if (f707 != 0) {
        dScMgBase_c::Virtual88(y, x, arg3, 4);
        if (y >= 0 && y < 0x100 && x2 >= 0 && x2 < 0x158) {
            (ctx->unk4710 + y * 0x158)[x2] = ctx->unk470a;
        }
    } else {
        u8 b;
        if (y < 0 || y >= 0x100 || x2 < 0 || x2 >= 0x158) {
            dScMgBase_c::Virtual88(y, x, arg3, 4);
            return;
        }
        if (y == 0x20 || y == 0x60 || y == 0xa0 || y == 0xe0) {
            if (ctx->unk4704 == 0) {
                ctx->unk4704 = 1;
                ctx->unk46d8 = y;
                ctx->unk46dc = x;
                ctx->unk46e0 = y;
                ctx->unk46e4 = x;
                return;
            }
            if (f705 != 0) return;
            if (y == ctx->unk46d8) {
                ctx->unk46e4 = x;
            } else {
                ctx->unk4705 = 1;
                ctx->unk46e0 = y;
                ctx->unk46e4 = x;
            }
            return;
        }
        if (ctx->unk4704 == 0) {
            dScMgBase_c::Virtual88(y, x, arg3, 2);
            return;
        }
        b = (ctx->unk470c + y * 0x158)[x2];
        if (b != 0 && ctx->unk470a != b) goto fail;
        b = (ctx->unk4710 + y * 0x158)[x2];
        if (b != 0 && ctx->unk470a != b) {
        fail:
            ctx->unk4704 = 0;
            ctx->unk4705 = 1;
            return;
        }
    }

    dScMgBase_c::Virtual88(y, x, arg3, 2);
    (ctx->unk470c + y * 0x158)[x2] = ctx->unk470a;
}
