//cpp
// @symbol func_ov006_02102fe8
#include "dScMgPachinko2_c.h"
/* dScMgPachinko2_c ball i under a live pen record: while the pen is down the
   ball follows the pen (x/y = pen + held offset, clamped to the field, offset
   re-derived); on release it aims the ball at (0x80, 0x20) and launches it.
   Touch record reads: data_020a0dea (pen x) is a volatile byte read and
   data_020a0deb (pen y) is read through a plain u8 pointer. Measured under
   2004/b56: both volatile 4 div (ldrb/add and literal/ldr order), both plain
   arrays 18 div (the y read sinks past the x store), both pointer reads 7 div
   (r3/r6 colouring); only this pairing reproduces the ROM's literal hoisting
   and load order at both sites. */
extern "C" {
extern u8 data_020a0e40;
extern u8 data_020a0de8[];
extern volatile u8 data_020a0dea[];
extern u8 data_020a0deb[];
extern s16 data_02082214[];
int _ZN4cstd4sqrtEy(u64 x);
s16 _ZN4cstd5atan2E5Fix12IiES1_(s32 y, s32 x);
int Sound_PlayIfNotActive(int handle, int a, int b, int c);
void func_ov006_02102864(dScMgPachinko2_c *self);
}
namespace Sound { void PlayBank2_2D(unsigned int); }

extern "C" void func_ov006_02102fe8(dScMgPachinko2_c *self, int i)
{
    int idx = data_020a0e40;
    int off = idx * 4;

    if (data_020a0de8[off] != 0) {
        int sx;
        int sy;
        int dx;
        int dy;
        int dist;
        int sq;
        int prev;
        int tx = data_020a0dea[off];
        int ty = ((u8 *)data_020a0deb)[off];

        self->mBalls[i].x = self->mBalls[i].px + (tx << 12);
        self->mBalls[i].y = self->mBalls[i].py + (ty << 12);
        sx = self->mBalls[i].x >> 12;
        sy = self->mBalls[i].y >> 12;
        if (sx >= 0xf8) {
            self->mBalls[i].x = 0xf8000;
        }
        if (sx <= 8) {
            self->mBalls[i].x = 0x8000;
        }
        if (sy >= 0xb8) {
            self->mBalls[i].y = 0xb8000;
        }
        if (sy <= 8) {
            self->mBalls[i].y = 0x8000;
        }
        {
            int nx = (self->mBalls[i].x >> 12) - data_020a0dea[data_020a0e40 * 4];
            int ny = (self->mBalls[i].y >> 12) - ((u8 *)data_020a0deb)[data_020a0e40 * 4];
            self->mBalls[i].px = nx << 12;
            self->mBalls[i].py = ny << 12;
        }

        dy = 0x80 - (self->mBalls[i].x >> 12);
        dx = 0x20 - (self->mBalls[i].y >> 12);
        dist = dy * dy + dx * dx;
        sq = _ZN4cstd4sqrtEy(dist);
        prev = self->mBalls[i].prevDist;
        self->mBalls[i].prevDist = sq;
        if (sq > prev) {
            if (sq > prev + 10) {
                self->mBalls[i].sound = Sound_PlayIfNotActive(self->mBalls[i].sound, 2, 0x17b, 0);
            } else {
                self->mBalls[i].sound = Sound_PlayIfNotActive(self->mBalls[i].sound, 2, 0x17c, 0);
            }
        }
    } else {
        int dy;
        int dx;
        int dist;
        int sq;

        self->mBalls[i].state = 2;
        dy = 0x80 - (self->mBalls[i].x >> 12);
        dx = 0x20 - (self->mBalls[i].y >> 12);
        dist = dy * dy + dx * dx;
        sq = _ZN4cstd4sqrtEy(dist);
        if (sq >= 0x10) {
            self->mBalls[i].angle = _ZN4cstd5atan2E5Fix12IiES1_(dx, dy);
            self->mBalls[i].speed = _ZN4cstd4sqrtEy(dist) << 8;
            self->mBalls[i].speed += self->mBalls[i].speed >> 3;
            self->mBalls[i].speed += 0x1800;
            if (self->mBalls[i].speed >= 0x9400) {
                self->mBalls[i].speed = 0x9400;
            }
            self->unk_566c = self->mBalls[i].speed >> 11;
            self->unk_566c += self->unk_566c >> 1;
            if (self->unk_566c == 0) {
                self->unk_566c = 1;
            }
            {
                s16 c = data_02082214[((u16)self->mBalls[i].angle >> 4) * 2 + 1];
                self->mBalls[i].vx = (s32)(((s64)c * self->mBalls[i].speed + 0x800) >> 12);
            }
            {
                s16 s = data_02082214[((u16)self->mBalls[i].angle >> 4) * 2];
                self->mBalls[i].vy = (s32)(((s64)s * self->mBalls[i].speed + 0x800) >> 12);
            }
            self->mBalls[i].unk_36 = 0;
            self->mBalls[i].unk_32 = self->mBalls[i].vx >> 2;
            if (sq >= 0x40) {
                Sound::PlayBank2_2D(0x17e);
            } else {
                Sound::PlayBank2_2D(0x17d);
            }
        } else {
            self->mBalls[i].x = 0x80000;
            self->mBalls[i].y = 0x28000;
            self->mBalls[i].state = 0;
        }
    }
    func_ov006_02102864(self);
}
