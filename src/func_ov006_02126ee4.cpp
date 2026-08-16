//cpp
#pragma opt_strength_reduction off
#pragma opt_common_subs off
#include "types.h"
#include "private/ov006_v35_vtbl.h"

extern "C" {
extern int data_0209d4b8;
int RandomIntInternal(int *seed);
}

extern "C" void func_ov006_02126ee4(char *c)
{
    int va;
    int vb;
    int vc;
    int pr7;
    int pr6;
    int cool;
    int p3col;
    int p3colx;
    int colx;
    int lim2;
    int gap2;
    int z2c, z30, z34, z38, z3c, z40, z44, z48, z4c, t50, z54, z58, z5c, z60;
    int z64, t68, z6c, z70, z74, z78, z7c, t80, z84;
    int w88, w8c, w90, w94, z98, z9c, wa0, wa4, wa8, wac, wb0, wb4, wb8;
    int zbc, zc0, zc4;
    int rs;
    int rx;
    int i;
    int n;
    int row;
    /* reverse-decl for callee-saved: last->r5 ... first->sb
       want col=r5, d6=r6, d7=r7, r8v=r8, sbv=sb */
    int sbv;
    int r8v;
    int d7;
    int d6;
    int col;

    for (i = 0; i < 0x80; i++) {
        *(u8 *)(c + i + 0xac58) = 0;
        ((int *)(c + 0xb0d8))[i] = 0;
        *(u8 *)(c + i + 0xb2d8) = 0;
    }
    for (i = 0; i < 0x80; i++) {
        *(u8 *)(c + i + 0xb358) = 0;
    }
    *(int *)(c + 0xab5c) = 0;

    if (((VObj *)c)->v35() != 0) {
        vb = 0xb;
        vc = 0xe;
        va = 6;
    } else {
        vb = 0xa;
        vc = 0xd;
        va = 7;
    }

    /* ROM statement order after ba08 load */
    d7 = 0;
    col = *(int *)(c + 0xba08) - 1;
    {
        int cool_r = 0x12;
        d6 = d7;
        sbv = 2;
        r8v = 0xd;
        cool = cool_r;
    }
    if (col >= 0) {
    colx = col * 16 + 8;
    lim2 = 0x10 - va;
    w90 = 5;
    w94 = 6;
    w88 = 3;
    w8c = 4;
    wac = 0xa;
    wb0 = 0xb;
    wa4 = 8;
    wa8 = 9;
    wa0 = 7;
    wb4 = 0xc;
    z98 = d7;
    z34 = d7;
    z40 = d7;
    z48 = d7;
    z44 = d7;
    z3c = d7;
    z38 = d7;
    z30 = d7;
    z2c = d7;
    z60 = d7;
    z5c = d7;
    z58 = d7;
    z54 = d7;
    t50 = sbv;
    z4c = d7;
    z78 = d7;
    z74 = d7;
    z70 = d7;
    z6c = d7;
    t68 = sbv;
    z64 = d7;
    z7c = d7;
    t80 = sbv;
    z84 = d7;
    z9c = d7;
    wb8 = 0x10;
    zbc = d7;
    zc0 = d7;
    zc4 = d7;

    do {
        pr7 = d7;
        pr6 = d6;
        d7 = (((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 3 >> 15;
        d6 = (((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 3 >> 15;

        if (col >= *(int *)(c + 0xba08) - 8) {
            d7 = z2c;
            d6 = d7;
        } else if (col <= *(int *)(c + 0xba04) / 16) {
            d7 = z30;
            d6 = d7;
        } else {
            int gap = r8v - sbv;
            if (gap <= vb) {
                if (d7 == 1)
                    d7 = z34;
                if (d6 == 1)
                    d6 = z38;
            } else if (vb + 1 == gap && d7 == 1 && d6 == 1) {
                d7 = z3c;
                d6 = d7;
            } else if (gap >= vc) {
                if (d7 == 2)
                    d7 = z40;
                if (d6 == 2)
                    d6 = z44;
            } else if (vc - 1 == gap && d7 == 2 && d6 == 2) {
                d7 = z48;
                d6 = d7;
            }
        }

        switch (d7) {
        case 0:
            break;
        case 1:
            if (pr7 == 2) {
                d7 = z4c;
            } else if (sbv >= va) {
                if (pr7 == 0) {
                    d7 = t50;
                    sbv--;
                } else {
                    d7 = z54;
                }
            }
            break;
        case 2:
            if (pr7 == 1) {
                d7 = z58;
            } else if (sbv < 1) {
                if (pr7 == 0)
                    d7 = 1;
                else
                    d7 = z5c;
            } else {
                sbv--;
            }
            break;
        default:
            d7 = z60;
            break;
        }

        switch (d6) {
        case 0:
            break;
        case 1:
            if (pr6 == 2) {
                d6 = z64;
            } else if (r8v <= lim2) {
                if (pr6 == 0) {
                    d6 = t68;
                    r8v++;
                } else {
                    d6 = z6c;
                }
            }
            break;
        case 2:
            if (pr6 == 1) {
                d6 = z70;
            } else if (r8v >= 0xf) {
                if (pr6 == 0)
                    d6 = 1;
                else
                    d6 = z74;
            } else {
                r8v++;
            }
            break;
        default:
            d6 = z78;
            break;
        }

        {
            int i0 = z7c;
            if (i0 < sbv) {
                char *row = c + col * 2;
                do {
                    *(u16 *)(row + 0x4f38) = 1;
                    row += 0x5c0;
                    i0++;
                } while (i0 < sbv);
            }
        }

        switch (d7) {
        case 0:
            *(u16 *)(c + sbv * 0x5c0 + col * 2 + 0x4f38) = t80;
            *(u16 *)(c + (sbv + 1) * 0x5c0 + col * 2 + 0x4f38) = z84;
            break;
        case 1:
            *(u16 *)(c + sbv * 0x5c0 + col * 2 + 0x4f38) = w88;
            *(u16 *)(c + (sbv + 1) * 0x5c0 + col * 2 + 0x4f38) = w8c;
            break;
        case 2:
            *(u16 *)(c + sbv * 0x5c0 + col * 2 + 0x4f38) = w90;
            *(u16 *)(c + (sbv + 1) * 0x5c0 + col * 2 + 0x4f38) = w94;
            break;
        }

        {
            int i0 = sbv + 2;
            if (i0 < r8v - 1) {
                char *row = c + col * 2 + i0 * 0x5c0;
                do {
                    *(u16 *)(row + 0x4f38) = z98;
                    row += 0x5c0;
                    i0++;
                } while (i0 < r8v - 1);
            }
        }

        switch (d6) {
        case 0:
            *(u16 *)(c + (r8v - 1) * 0x5c0 + col * 2 + 0x4f38) = z9c;
            *(u16 *)(c + r8v * 0x5c0 + col * 2 + 0x4f38) = wa0;
            break;
        case 1:
            *(u16 *)(c + (r8v - 1) * 0x5c0 + col * 2 + 0x4f38) = wa4;
            *(u16 *)(c + r8v * 0x5c0 + col * 2 + 0x4f38) = wa8;
            break;
        case 2:
            *(u16 *)(c + (r8v - 1) * 0x5c0 + col * 2 + 0x4f38) = wac;
            *(u16 *)(c + r8v * 0x5c0 + col * 2 + 0x4f38) = wb0;
            break;
        }

        {
            int i0 = r8v + 1;
            if (i0 < 0x10) {
                char *p = c + col * 2 + i0 * 0x5c0;
                do {
                    *(u16 *)(p + 0x4f38) = 1;
                    p += 0x5c0;
                    i0++;
                } while (i0 < 0x10);
            }
        }

        if (d7 == 1)
            sbv++;
        if (d6 == 1)
            r8v--;

        if (col == *(int *)(c + 0xba04) / 16 - 1)
            *(int *)(c + 0xab5c) = (((r8v + sbv + 1) << 4) / 2) << 12;

        if (col > *(int *)(c + 0xba04) / 16 + 0x11) {
            if (cool > 0) {
                cool--;
            } else if (((VObj *)c)->v35() != 0 && col < *(int *)(c + 0xba08) / 2) {
                if (r8v - sbv >= 0xd) {
                    if ((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 8 >> 15 == 0) {
                        int r2;
                        if ((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 2 >> 15 == 0)
                            r2 = sbv + 2;
                        else
                            r2 = r8v - 2;
                        *(u16 *)(c + r2 * 0x5c0 + col * 2 + 0x4f38) = wb4;
                        cool = wb8;
                        for (i = zbc; i < 0x80; i++) {
                            u8 *q = (u8 *)(((int)c + i + 0xac58) & 0xFFFFFFFFFFFFFFFF);
                            if (*q == 0) {
                                *(int *)(c + i * 8 + 0xacd8) = ((r2 << 4) + 8) << 12;
                                *(int *)(c + i * 8 + 0xacdc) = colx << 12;
                                *q = 1;
                                break;
                            }
                        }
                    }
                }
            } else {
                gap2 = r8v - sbv;
                if (gap2 >= 0xd) {
                    if ((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 16 >> 15 == 0) {
                        int r2 = sbv + (((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * (gap2 - 3)) >> 15) + 2;
                        *(u16 *)(c + r2 * 0x5c0 + col * 2 + 0x4f38) = wb4;
                        cool = wb8;
                        for (i = zc0; i < 0x80; i++) {
                            u8 *q = (u8 *)(((int)c + i + 0xac58) & 0xFFFFFFFFFFFFFFFF);
                            if (*q == 0) {
                                *(int *)(c + i * 8 + 0xacd8) = ((r2 << 4) + 8) << 12;
                                *(int *)(c + i * 8 + 0xacdc) = colx << 12;
                                *q = 1;
                                break;
                            }
                        }
                    }
                } else if (gap2 >= 0xa) {
                    if ((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 16 >> 15 == 0) {
                        int r2;
                        if ((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 2 >> 15 == 0)
                            r2 = sbv + 2;
                        else
                            r2 = r8v - 2;
                        *(u16 *)(c + r2 * 0x5c0 + col * 2 + 0x4f38) = wb4;
                        cool = wb8;
                        for (i = zc4; i < 0x80; i++) {
                            u8 *q = (u8 *)(((int)c + i + 0xac58) & 0xFFFFFFFFFFFFFFFF);
                            if (*q == 0) {
                                *(int *)(c + i * 8 + 0xacd8) = ((r2 << 4) + 8) << 12;
                                *(int *)(c + i * 8 + 0xacdc) = colx << 12;
                                *q = 1;
                                break;
                            }
                        }
                    }
                }
            }
        }
        colx -= 0x10;
        col--;
    } while (col >= 0);
    }

    if (((VObj *)c)->v35() != 0) {
        for (i = 0; i < 0x80; i++) {
            if (*(u8 *)(c + i + 0xac58) == 1) {
                if ((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 2 >> 15 == 0)
                    *(int *)(c + i * 4 + 0xb0d8) = 1;
            }
        }
    }

    {
        int c1 = *(int *)(c + 0xba04) / 16;
        col = c1 - 1;
        i = 0xf;
        r8v = 0xe;
        d6 = 0xd;
        d7 = 0;
        for (; col <= c1; col++) {
            int y;
            y = d7;
            sbv = (int)c + col * 2;
            for (; y < 0x10; y++) {
                u16 gv = *(u16 *)((char *)sbv + 0x4f38);
                if (gv == 0)
                    *(u16 *)((char *)sbv + 0x4f38) = d6;
                else if (gv == 2)
                    *(u16 *)((char *)sbv + 0x4f38) = r8v;
                else if (gv == 7)
                    *(u16 *)((char *)sbv + 0x4f38) = i;
                sbv += 0x5c0;
            }
            c1 = *(int *)(c + 0xba04) / 16;
        }
    }

    n = 0;
    p3col = *(int *)(c + 0xba08) - 1;
    if (p3col >= 0) {
        p3colx = p3col * 16 + 8;
        rs = n;
        rx = 8;
        do {
            char *gp = c + p3col * 2;
            int rowx;
            row = rs;
            rowx = rx;
            for (; row < 0x10; row++) {
                u16 gv = *(u16 *)(gp + 0x4f38);
                if (gv == 1) {
                    int v = (((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 16 >> 15;
                    if (v < 4) {
                        *(u16 *)(gp + 0x4f38) = v + 0x1d;
                    } else if (v == 4) {
                        if (n < 0x80) {
                            *(int *)(c + n * 8 + 0xb5d8) = rowx << 12;
                            *(int *)(c + n * 8 + 0xb5dc) = p3colx << 12;
                            *(u8 *)(c + n + 0xb358) = 1;
                            *(int *)(c + n * 4 + 0xb3d8) = (((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 4 >> 15;
                            n++;
                        }
                    }
                }
                gp += 0x5c0;
                rowx += 0x10;
            }
            p3colx -= 0x10;
            p3col--;
        } while (p3col >= 0);
    }
}
