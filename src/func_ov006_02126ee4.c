// @symbol func_ov006_02126ee4
// NONMATCHING: one codegen idiom (div=9 of 691 words), everything else
// byte-identical -- size, frame, every stack slot, every callee-saved register,
// all control flow, all three pool words. The ROM reads the vtable pointer
// through the argument copy (mov r0,this; ldr r1,[r0]) at all three virtual-call
// sites; this source reads it straight from the object register (ldr r1,[sl]),
// which drags the scratch register at +0x40/+0x50 and the store order at
// +0x5c..+0x68 with it. r0 == sl at every one of those points, so the two are
// semantically identical. MEASURED NEGATIVE, so the next reader does not repeat
// it: compiling this same body as C++ (-lang c++, tools/match.py --flags) is
// BITWISE IDENTICAL -- the language mode is not the lever.
// Counts as decompiled, not matched.
// @symbol func_ov006_02126ee4
/* dScMgSnowball_c course-layout generator (scene 377, "Snowball Slalom").
   Called from vtable slot 0 (InitResources, 0x0212953c) and slot 18 (0x02129250).
   Lays out the whole course into the u16 tile grid at +0x4f38 (16 columns x 0x2e0
   rows, column-major, 0x5c0 bytes per column), seeds the snowball table at
   +0xac58/+0xacd8, seeds the scenery table at +0xb358/+0xb3d8/+0xb5d8, and stores
   the starting X at +0xab5c.

   NONMATCHING: 9 words of 691 differ, all one idiom -- the ROM loads the vtable
   pointer through the argument copy (mov r0,this; ldr r1,[r0]), which is what the
   C++ front end emits for this->vf(); the C front end here loads it straight from
   the object register. Everything else -- size, frame, every stack slot, every
   callee-saved register, all control flow -- is byte-identical. */
#include "types.h"

typedef int (*VBool)(char *);

extern int RandomIntInternal(int *seed);
extern int data_0209d4b8;

#define RAND(n) ((u32)(((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * (n) >> 15)
#define TILE(col, row) (((u16 *)(c + (col) * 0x5c0 + 0x4f38))[row])

#pragma opt_strength_reduction off
void func_ov006_02126ee4(char *c)
{
    int a;
    int b;
    int d;
    int prev_l;
    int prev_r;
    int cnt;
    int row2;
    int z2;
    int z;
    int left;
    int right;
    int l;
    int r;
    int row;
    char *p;
    int i;
    int t;
    int n;

    for (i = 0; i < 0x80; i++) {
        *(u8 *)(c + i + 0xac58) = 0;
        *(int *)(c + 0xb0d8 + i * 4) = 0;
        *(u8 *)(c + 0xb2d8 + i) = 0;
    }
    for (i = 0; i < 0x80; i++) {
        *(u8 *)(c + 0xb358 + i) = 0;
    }
    *(int *)(c + 0xab5c) = 0;

    if ((*(VBool *)(*(char **)c + 0x8c))(c)) {
        b = 0xb;
        d = 0xe;
        a = 6;
    } else {
        b = 0xa;
        d = 0xd;
        a = 7;
    }

    l = 0;
    r = 0;
    left = 2;
    right = 0xd;
    cnt = 0x12;
    row = *(int *)(c + 0xba08) - 1;
    if (row >= 0) {
        z = row * 16 + 8;
        do {
            prev_l = l;
            prev_r = r;
            l = RAND(3);
            r = RAND(3);
            if (row >= *(int *)(c + 0xba08) - 8) {
                l = 0;
                r = 0;
            } else if (row <= *(int *)(c + 0xba04) / 16) {
                l = 0;
                r = 0;
            } else if (right - left <= b) {
                if (l == 1) l = 0;
                if (r == 1) r = 0;
            } else if (b + 1 == right - left && l == 1 && r == 1) {
                l = 0;
                r = 0;
            } else if (right - left >= d) {
                if (l == 2) l = 0;
                if (r == 2) r = 0;
            } else if (d - 1 == right - left && l == 2 && r == 2) {
                l = 0;
                r = 0;
            }

            switch (l) {
            case 0:
                break;
            case 1:
                if (prev_l == 2) {
                    l = 0;
                } else if (left >= a) {
                    if (prev_l == 0) {
                        l = 2;
                        left--;
                    } else {
                        l = 0;
                    }
                }
                break;
            case 2:
                if (prev_l == 1) {
                    l = 0;
                } else if (left < 1) {
                    if (prev_l == 0) l = 1;
                    else l = 0;
                } else {
                    left--;
                }
                break;
            default:
                l = 0;
                break;
            }

            switch (r) {
            case 0:
                break;
            case 1:
                if (prev_r == 2) {
                    r = 0;
                } else if (right <= 0x10 - a) {
                    if (prev_r == 0) {
                        r = 2;
                        right++;
                    } else {
                        r = 0;
                    }
                }
                break;
            case 2:
                if (prev_r == 1) {
                    r = 0;
                } else if (right >= 0xf) {
                    if (prev_r == 0) r = 1;
                    else r = 0;
                } else {
                    right++;
                }
                break;
            default:
                r = 0;
                break;
            }

            {
                int ia = 0;
                if (left > 0) {
                    char *pa = c + row * 2;
                    do {
                        *(u16 *)(pa + 0x4f38) = 1;
                        pa += 0x5c0;
                        ia++;
                    } while (ia < left);
                }
            }

            switch (l) {
            case 0:
                TILE(left, row) = 2;
                TILE(left + 1, row) = 0;
                break;
            case 1:
                TILE(left, row) = 3;
                TILE(left + 1, row) = 4;
                break;
            case 2:
                TILE(left, row) = 5;
                TILE(left + 1, row) = 6;
                break;
            }

            i = left + 2;
            if (i < right - 1) {
                p = c + row * 2 + i * 0x5c0;
                do {
                    *(u16 *)(p + 0x4f38) = 0;
                    i++;
                    p += 0x5c0;
                } while (i < right - 1);
            }

            switch (r) {
            case 0:
                TILE(right - 1, row) = 0;
                TILE(right, row) = 7;
                break;
            case 1:
                TILE(right - 1, row) = 8;
                TILE(right, row) = 9;
                break;
            case 2:
                TILE(right - 1, row) = 0xa;
                TILE(right, row) = 0xb;
                break;
            }

            i = right + 1;
            if (i < 0x10) {
                p = (char *)((u16 *)c + row) + i * 0x5c0;
                do {
                    *(u16 *)(p + 0x4f38) = 1;
                    p += 0x5c0;
                    i++;
                } while (i < 0x10);
            }

            if (l == 1) left++;
            if (r == 1) right--;
            if (row == *(int *)(c + 0xba04) / 16 - 1) {
                *(int *)(c + 0xab5c) = ((right + left + 1) * 16 / 2) << 12;
            }

            if (row > *(int *)(c + 0xba04) / 16 + 0x11) {
                if (cnt > 0) {
                    cnt--;
                } else if ((*(VBool *)(*(char **)c + 0x8c))(c)
                           && row < *(int *)(c + 0xba08) / 2) {
                    if (right - left >= 0xd && RAND(8) == 0) {
                        int col;
                        if (RAND(2) == 0) col = left + 2;
                        else col = right - 2;
                        TILE(col, row) = 0xc;
                        cnt = 0x10;
                        for (i = 0; i < 0x80; i++) {
                            if (*(u8 *)(c + i + 0xac58) == 0) {
                                *(int *)(c + 0xacd8 + i * 8) = (col * 16 + 8) << 12;
                                *(int *)(c + 0xacdc + i * 8) = z << 12;
                                *(u8 *)(c + i + 0xac58) = 1;
                                break;
                            }
                        }
                    }
                } else if (right - left >= 0xd) {
                    if (RAND(16) == 0) {
                        int col = left + RAND(right - left - 3) + 2;
                        TILE(col, row) = 0xc;
                        cnt = 0x10;
                        for (i = 0; i < 0x80; i++) {
                            if (*(u8 *)(c + i + 0xac58) == 0) {
                                *(int *)(c + 0xacd8 + i * 8) = (col * 16 + 8) << 12;
                                *(int *)(c + 0xacdc + i * 8) = z << 12;
                                *(u8 *)(c + i + 0xac58) = 1;
                                break;
                            }
                        }
                    }
                } else if (right - left >= 0xa) {
                    if (RAND(16) == 0) {
                        int col;
                        if (RAND(2) == 0) col = left + 2;
                        else col = right - 2;
                        TILE(col, row) = 0xc;
                        cnt = 0x10;
                        for (i = 0; i < 0x80; i++) {
                            if (*(u8 *)(c + i + 0xac58) == 0) {
                                *(int *)(c + 0xacd8 + i * 8) = (col * 16 + 8) << 12;
                                *(int *)(c + 0xacdc + i * 8) = z << 12;
                                *(u8 *)(c + i + 0xac58) = 1;
                                break;
                            }
                        }
                    }
                }
            }
            row--;
            z -= 0x10;
        } while (row >= 0);
    }

    if ((*(VBool *)(*(char **)c + 0x8c))(c)) {
        for (i = 0; i < 0x80; i++) {
            if (*(u8 *)(c + i + 0xac58) == 1) {
                if (RAND(2) == 0) {
                    *(int *)(c + 0xb0d8 + i * 4) = 1;
                }
            }
        }
    }

    for (row = *(int *)(c + 0xba04) / 16 - 1; row <= *(int *)(c + 0xba04) / 16; row++) {
        i = 0;
        p = c + row * 2;
        do {
            t = *(u16 *)(p + 0x4f38);
            if (t == 0) *(u16 *)(p + 0x4f38) = 0xd;
            else if (t == 2) *(u16 *)(p + 0x4f38) = 0xe;
            else if (t == 7) *(u16 *)(p + 0x4f38) = 0xf;
            i++;
            p += 0x5c0;
        } while (i < 0x10);
    }

    n = 0;
    row2 = *(int *)(c + 0xba08) - 1;
    if (row2 >= 0) {
        z2 = row2 * 16 + 8;
        do {
            int i2 = 0;
            char *p2 = c + row2 * 2;
            int x = 8;
            int zz = z2 << 12;
            do {
                if (*(u16 *)(p2 + 0x4f38) == 1) {
                    t = RAND(16);
                    if (t < 4) {
                        *(u16 *)(p2 + 0x4f38) = (u16)(t + 0x1d);
                    } else if (t == 4) {
                        if (n < 0x80) {
                            *(int *)(c + 0xb5d8 + n * 8) = x << 12;
                            *(int *)(c + 0xb5dc + n * 8) = zz;
                            *(u8 *)(c + 0xb358 + n) = 1;
                            *(int *)(c + 0xb3d8 + n * 4) = RAND(4);
                            n++;
                        }
                    }
                }
                i2++;
                p2 += 0x5c0;
                x += 0x10;
            } while (i2 < 0x10);
            z2 -= 0x10;
            row2--;
        } while (row2 >= 0);
    }
}
