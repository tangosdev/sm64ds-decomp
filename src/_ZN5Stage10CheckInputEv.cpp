//cpp
#include "Stage.h"
#include "types.h"
struct TouchData { u8 touched; u8 tapped; u8 x; u8 y; };
struct PadData { u16 held; u16 pressed; };
struct Ctrl {
    s16 dx;      /* 0x00 */
    s16 dy;      /* 0x02 */
    u16 held;    /* 0x04 */
    u16 pressed; /* 0x06 */
    s16 mag;     /* 0x08 */
    s16 nx;      /* 0x0a */
    s16 ny;      /* 0x0c */
    s16 ang;     /* 0x0e */
    u8 tx;       /* 0x10 */
    u8 ty;       /* 0x11 */
    u8 delay;    /* 0x12 */
    u8 cnt;      /* 0x13 */
    u8 touching; /* 0x14 */
    u8 pad15;    /* 0x15 */
    u8 mode;     /* 0x16 */
    u8 pad17;    /* 0x17 */
};

extern "C" {
u8 DecIfAbove0_Byte(u8* p);
int _ZN4cstd4sqrtEy(unsigned long long v);
int _ZN4cstd4fdivEii(int a, int b);
s16 _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);

extern u8 data_0209f2d8;
extern int data_0209caa0[];
extern u8 data_0209d660;
extern int data_0209fc48;
extern u8 data_0209f21c;
extern u8 data_0209f350[];
extern struct Ctrl data_0209f498[];
extern struct TouchData data_020a0de8[];
extern struct PadData data_020a0e58[];
extern s16 data_020756b0[];
extern s16 data_02082214[];
extern u16* data_0209214c[];

}

void Stage::CheckInput()
{
    u8 m = data_0209f2d8;
    int b1 = (m == 1);
    if (b1 == false) {
        int b2 = (m == 2);
        if (b2 == false && (data_0209caa0[2] & 0x80)) {
            if (data_0209d660 == 0) {
                int b3 = (data_0209fc48 != 0);
                if (b3 == false) goto main_part;
            } else {
                goto main_part;
            }
        }
        {
            Ctrl* q = data_0209f498;
            int k;
            for (k = 0; k < 4; k++) {
                q->touching = 0;
                q->delay = 0;
                q->cnt = 0;
                q->mag = 0;
                q->nx = 0;
                q->ny = 0;
                q->ang = 0;
                q->held = 0;
                q->pressed = 0;
                q++;
            }
        }
        return;
    }
main_part:
    {
        Ctrl* p = data_0209f498;
        int i = 0;
        int cnt = data_0209f21c;
        int newx;
        int newy;
        u8* st;
        if (cnt <= 0) return;
        st = data_0209f350;
        do {
            u32 held = data_020a0e58[i].held;
            u32 pressed = data_020a0e58[i].pressed;
            u8 touched = data_020a0de8[i].touched;
            if ((touched != 0 && data_0209d660 != 0) || (*st != 1 && touched != 0)) {
                p->touching = 1;
                DecIfAbove0_Byte(&p->cnt);
                {
                    int b = (data_020a0de8[i].touched != 0 && data_020a0de8[i].tapped != 0);
                    if (b != false) {
                        if (p->cnt != 0) {
                            int ty2 = data_020a0de8[i].y - p->dy;
                            int tx2 = data_020a0de8[i].x - p->dx;
                            if (tx2 < 0) tx2 = 0;
                            else if (tx2 > 0xff) tx2 = 0xff;
                            if (ty2 < 0) ty2 = 0;
                            else if (ty2 > 0xbf) ty2 = 0xbf;
                            p->tx = tx2;
                            p->ty = ty2;
                        } else {
                            p->delay = 5;
                        }
                    }
                }
                if (DecIfAbove0_Byte(&p->delay) != 0) {
                    p->tx = data_020a0de8[i].x;
                    p->ty = data_020a0de8[i].y;
                    p->mag = 0;
                    p->nx = 0;
                    p->ny = 0;
                    p->ang = 0;
                } else {
                    int dx;
                    int dy;
                    int len;
                    int max;
                    newx = data_020a0de8[i].x;
                    newy = data_020a0de8[i].y;
                    dx = newx - p->tx;
                    dy = newy - p->ty;
                    p->dx = dx;
                    p->dy = dy;
                    {
                        s64 sq = (s64)dx * dx;
                        sq += (s64)dy * dy;
                        len = _ZN4cstd4sqrtEy(sq);
                    }
                    if (p->mode == 2) max = 0x30;
                    else max = 0x24;
                    if (len > max) {
                        int quot = _ZN4cstd4fdivEii(max, len);
                        dx = (int)(((s64)dx * quot + 0x800) >> 12);
                        p->tx = newx - dx;
                        dy = (int)(((s64)dy * quot + 0x800) >> 12);
                        p->ty = newy - dy;
                        len = max;
                    } else if (len < 4) {
                        len = 0;
                    }
                    p->mag = (len << 12) / max;
                    p->nx = (dx << 12) / max;
                    p->ny = (dy << 12) / max;
                    p->ang = _ZN4cstd5atan2E5Fix12IiES1_(dx, dy);
                }
            } else {
                int b2;
                int dirs;
                b2 = (touched == 0 && data_020a0de8[i].tapped != 0);
                if (b2 != false) p->cnt = 5;
                DecIfAbove0_Byte(&p->cnt);
                p->touching = 0;
                dirs = 0;
                if (p->mode == 0) dirs = data_020a0e58[i].held & 0xf0;
                p->ang = data_020756b0[dirs >> 4];
                if (dirs != 0) {
                    p->mag = 0x1000;
                    p->nx = data_02082214[((u16)p->ang >> 4) * 2];
                    p->ny = data_02082214[((u16)p->ang >> 4) * 2 + 1];
                } else {
                    p->mag = 0;
                    p->nx = 0;
                    p->ny = 0;
                }
            }
            {
                int b3 = (data_0209fc48 != 0);
                if (b3 == false) {
                    u16* map = data_0209214c[p->mode];
                    u32 j = 0;
                    do {
                        if (held & 1) p->held |= *map;
                        held >>= 1;
                        if (pressed & 1) p->pressed |= *map;
                        pressed >>= 1;
                        map++;
                        j++;
                    } while (j < 0x10);
                    if (p->mode != 2 && (p->held & 0x300) == 0x300) {
                        *(u16*)(((int)p + 4) & 0xFFFFFFFFFFFFFFFFull) |= 0x4000;
                        if (p->pressed & 0x300) {
                            *(u16*)(((int)p + 6) & 0xFFFFFFFFFFFFFFFFull) |= 0x4000;
                        }
                    }
                }
            }
            st++;
            i++;
            p++;
        } while (i < data_0209f21c);
    }
}
