typedef signed short s16;
typedef unsigned short u16;
typedef long long s64;
typedef unsigned long long u64;

extern s16 data_02082214[];

extern s64 _ZN4cstd4sqrtEy(u64 x);
extern int _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);

#pragma opt_common_subs off
void func_ov006_020d6e8c(char* self, int idx)
{
    int x, y, py, dx, dy1, ang, i2, a, dy2, sy, px;

    x = *(int*)(self + (idx << 6) + 0x4660) >> 12;
    y = *(int*)(self + (idx << 6) + 0x4664) >> 12;

    if (x + 12 > 0x100) {
        *(u16*)(self + (idx << 6) + 0x468c) = 0x8000 - *(u16*)(self + (idx << 6) + 0x468c);
        *(int*)(self + (idx << 6) + 0x4660) = 0xF4000;
    } else if (x - 12 < 0) {
        *(u16*)(self + (idx << 6) + 0x468c) = 0x8000 - *(u16*)(self + (idx << 6) + 0x468c);
        *(int*)(self + (idx << 6) + 0x4660) = 0xC000;
    }

    if (y + 12 > 0xB8) {
        *(u16*)(self + (idx << 6) + 0x468c) = 0 - *(u16*)(self + (idx << 6) + 0x468c);
        *(int*)(self + (idx << 6) + 0x4664) = 0xAC000;
    } else if (y - 12 < 0) {
        *(u16*)(self + (idx << 6) + 0x468c) = 0 - *(u16*)(self + (idx << 6) + 0x468c);
        *(int*)(self + (idx << 6) + 0x4664) = 0xC000;
    }

    px = *(int*)(self + (idx << 6) + 0x4660) >> 12;
    py = *(int*)(self + (idx << 6) + 0x4664) >> 12;

    if (px + 12 > 0xC0 && py + 12 > 0x40 && py - 12 < 0x80) {
        dx = px - 0xC0;
        dy1 = py - 0x40;
        dy2 = 0x80 - py;
        if (px <= 0xC0 && py >= 0x40 && py <= 0x80) {
            *(u16*)(self + (idx << 6) + 0x468c) = 0x8000 - *(u16*)(self + (idx << 6) + 0x468c);
            *(int*)(self + (idx << 6) + 0x4660) = 0xB4000;
        } else if (px > 0xC0 && py < 0x40) {
            *(u16*)(self + (idx << 6) + 0x468c) = 0 - *(u16*)(self + (idx << 6) + 0x468c);
            *(int*)(self + (idx << 6) + 0x4664) = 0x34000;
        } else if (px > 0xC0 && py > 0x80) {
            *(u16*)(self + (idx << 6) + 0x468c) = 0 - *(u16*)(self + (idx << 6) + 0x468c);
            *(int*)(self + (idx << 6) + 0x4664) = 0x8C000;
        } else if (px > 0xC0 && py > 0x40 && py < 0x80) {
            if (dy1 < dy2) {
                if (dx < dy1) {
                    *(u16*)(self + (idx << 6) + 0x468c) = 0x8000 - *(u16*)(self + (idx << 6) + 0x468c);
                    *(int*)(self + (idx << 6) + 0x4660) = 0xB4000;
                } else {
                    *(u16*)(self + (idx << 6) + 0x468c) = 0 - *(u16*)(self + (idx << 6) + 0x468c);
                    *(int*)(self + (idx << 6) + 0x4664) = 0x34000;
                }
            } else {
                if (dx < dy2) {
                    *(u16*)(self + (idx << 6) + 0x468c) = 0x8000 - *(u16*)(self + (idx << 6) + 0x468c);
                    *(int*)(self + (idx << 6) + 0x4660) = 0xB4000;
                } else {
                    *(u16*)(self + (idx << 6) + 0x468c) = 0 - *(u16*)(self + (idx << 6) + 0x468c);
                    *(int*)(self + (idx << 6) + 0x4664) = 0x8C000;
                }
            }
        } else {
            if (py >= 0x60) {
                sy = 0xC0 - px;
            } else {
                dy2 = 0x40 - py;
                sy = 0xC0 - px;
            }
            _ZN4cstd4sqrtEy((s64)(sy * sy + dy2 * dy2));
            ang = (u16)_ZN4cstd5atan2E5Fix12IiES1_(dy2, sy);
            i2 = (ang >> 4) * 2;
            *(int*)(self + (idx << 6) + 0x4660) =
                (0xC0 - (int)(((s64)data_02082214[i2 + 1] * 0xF + 0x800) >> 12)) << 12;
            if (py >= 0x60) {
                *(int*)(self + (idx << 6) + 0x4664) =
                    (0x80 - (int)(((s64)data_02082214[i2] * 0xF + 0x800) >> 12)) << 12;
            } else {
                *(int*)(self + (idx << 6) + 0x4664) =
                    (0x40 - (int)(((s64)data_02082214[i2] * 0xF + 0x800) >> 12)) << 12;
            }
            a = *(u16*)(self + (idx << 6) + 0x468c);
            if (data_02082214[(a >> 4) * 2 + 1] > 0) {
                *(u16*)(self + (idx << 6) + 0x468c) = 0x8000 - a;
            } else {
                *(u16*)(self + (idx << 6) + 0x468c) = 0 - a;
            }
        }
    }

    if (px - 12 >= 0x40) return;
    if (py + 12 <= 0x40) return;
    if (py - 12 >= 0x80) return;

    dy2 = 0x40 - px;
    dy1 = py - 0x40;
    sy = 0x80 - py;
    if (px >= 0x40 && py >= 0x40 && py <= 0x80) {
        *(u16*)(self + (idx << 6) + 0x468c) = 0x8000 - *(u16*)(self + (idx << 6) + 0x468c);
        *(int*)(self + (idx << 6) + 0x4660) = 0x4C000;
        return;
    }
    if (px < 0x40 && py < 0x40) {
        *(u16*)(self + (idx << 6) + 0x468c) = 0 - *(u16*)(self + (idx << 6) + 0x468c);
        *(int*)(self + (idx << 6) + 0x4664) = 0x34000;
        return;
    }
    if (px < 0x40 && py > 0x80) {
        *(u16*)(self + (idx << 6) + 0x468c) = 0 - *(u16*)(self + (idx << 6) + 0x468c);
        *(int*)(self + (idx << 6) + 0x4664) = 0x8C000;
        return;
    }
    if (px < 0x40 && py > 0x40 && py < 0x80) {
        if (dy1 < sy) {
            if (dy2 < dy1) {
                *(u16*)(self + (idx << 6) + 0x468c) = 0x8000 - *(u16*)(self + (idx << 6) + 0x468c);
                *(int*)(self + (idx << 6) + 0x4660) = 0x4C000;
                return;
            }
            *(u16*)(self + (idx << 6) + 0x468c) = 0 - *(u16*)(self + (idx << 6) + 0x468c);
            *(int*)(self + (idx << 6) + 0x4664) = 0x34000;
            return;
        }
        if (dy2 < sy) {
            *(u16*)(self + (idx << 6) + 0x468c) = 0x8000 - *(u16*)(self + (idx << 6) + 0x468c);
            *(int*)(self + (idx << 6) + 0x4660) = 0x4C000;
            return;
        }
        *(u16*)(self + (idx << 6) + 0x468c) = 0 - *(u16*)(self + (idx << 6) + 0x468c);
        *(int*)(self + (idx << 6) + 0x4664) = 0x8C000;
        return;
    }

    if (py < 0x60) {
        sy = 0x40 - py;
    }
    _ZN4cstd4sqrtEy((s64)(dy2 * dy2 + sy * sy));
    ang = (u16)_ZN4cstd5atan2E5Fix12IiES1_(sy, dy2);
    i2 = (ang >> 4) * 2;
    *(int*)(self + (idx << 6) + 0x4660) =
        (0x40 - (int)(((s64)data_02082214[i2 + 1] * 0xF + 0x800) >> 12)) << 12;
    if (py >= 0x60) {
        *(int*)(self + (idx << 6) + 0x4664) =
            (0x80 - (int)(((s64)data_02082214[i2] * 0xF + 0x800) >> 12)) << 12;
    } else {
        *(int*)(self + (idx << 6) + 0x4664) =
            (0x40 - (int)(((s64)data_02082214[i2] * 0xF + 0x800) >> 12)) << 12;
    }
    a = *(u16*)(self + (idx << 6) + 0x468c);
    if (data_02082214[(a >> 4) * 2 + 1] < 0) {
        *(u16*)(self + (idx << 6) + 0x468c) = 0x8000 - a;
    } else {
        *(u16*)(self + (idx << 6) + 0x468c) = 0 - a;
    }
}
