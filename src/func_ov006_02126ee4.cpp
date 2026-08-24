//cpp
// @symbol func_ov006_02126ee4
// MATCHING at mwccarm 2004/b56.
//
// dScMgSnowball_c course-layout generator (scene 377, "Snowball Slalom").
// Called from vtable slot 0 (InitResources, 0x0212953c) and slot 18 (0x02129250).
// Lays out the whole course into the u16 tile grid at +0x4f38 (16 columns x 0x2e0
// rows, column-major, 0x5c0 bytes per column), seeds the snowball table at
// +0xac58/+0xacd8, seeds the scenery table at +0xb358/+0xb3d8/+0xb5d8, and stores
// the starting X at +0xab5c.
//
// HISTORY, SUPERSEDED AND KEPT ONLY AS A LEVER NOTE -- this file matches today
// and nothing below describes its current state. The body first landed with a
// residual of 9 words in 691, all one idiom: the ROM reads the vtable pointer
// through the argument copy (mov r0,this; ldr r1,[r0]) at all three call sites,
// which is what the C++ front end emits for this->vf(), while a C
// function-pointer call reads it straight from the object register. Two things
// closed it at once, and they are the same edit -- see the struct below, which
// is also the port-side correctness fix because the host's face for this slot
// takes its receiver in ecx.
//
// MEASURED NEGATIVE, kept so the lever is not retried: the language mode alone
// does nothing. Compiling the C spelling with -lang c++ is BITWISE IDENTICAL,
// residual unchanged at 9 of 691. What closes it is the CALL, not the front end.
//
// THE WORD THIS BANNER DELIBERATELY DOES NOT USE. tools/progress.py:84,
// tools/enroll.py:146, tools/ledger.py:270 and tools/chaos_db_ci.py:347 all
// classify a src file by searching its FIRST 200 CHARACTERS for the hatch token,
// so a matched file that mentions that token while describing its own history
// classifies as unmatched and drops out of progress, the chaos DB, the treemap
// and rom-stats. This file therefore says "residual" throughout and never spells
// the token, in the head or anywhere else.
#include "types.h"

/* THE VIRTUAL CALL HAS TO BE A VIRTUAL CALL, and this is the established
   spelling in this tree rather than a new one: src/func_ov006_021063a0.cpp,
   _02106168.cpp and _021057f0.cpp are the three other bodies that dispatch this
   same slot, and all three declare exactly this dummy-vtable struct and call
   through it. The port's face for slot 35 is
   `static int __fastcall mb_v35(void *s, void *)` in hal/scene_mg.cpp, which
   takes the receiver in ecx; a C function-pointer call through
   (*(int (*)(char *))(*(char **)c + 0x8c))(c) pushes it on the stack instead and
   leaves ecx holding whatever was there. That is the mg10 invisible-defect class
   -- no unresolved symbol, no arity row, no member-pointer encoding for either
   checker to see -- and it is what slice_snw.txt section 14 already records one
   slot over. Measured here: the C spelling faults on the first boot of scene 377
   with ecx = 0 inside func_ov004_020ad660 reading [0+8]. */
struct O {
  virtual void v00(); virtual void v01(); virtual void v02(); virtual void v03();
  virtual void v04(); virtual void v05(); virtual void v06(); virtual void v07();
  virtual void v08(); virtual void v09(); virtual void v10(); virtual void v11();
  virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
  virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
  virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23();
  virtual void v24(); virtual void v25(); virtual void v26(); virtual void v27();
  virtual void v28(); virtual void v29(); virtual void v30(); virtual void v31();
  virtual void v32(); virtual void v33(); virtual void v34();
  virtual int m8c();
};

extern "C" {

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

    if (((O *)c)->m8c()) {
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
                } else if (((O *)c)->m8c()
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

    if (((O *)c)->m8c()) {
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

}
