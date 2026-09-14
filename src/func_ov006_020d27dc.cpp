//cpp
// @symbol func_ov006_020d27dc
/* recovered: Mario's Slides (dScMgAmida_c block): the large per-frame routine that walks the ladder objects. */
// NONMATCHING: div 17 of 914 words. mwccarm 2004/b56, --module ov006,
// @ 0x020d27dc size 0xe48. Residue class: no mechanism named; one branch shape.
// Draft from nearmiss/db.jsonl (stored divergence 17), landed 2026-09-14 under Tango's ruling that the functionally-equivalent C
// drafts live on main with an honest banner so the port and readers have source. Logic is
// verified only as far as the residue class implies: register names and instruction order
// for the shape-exact rows, NOT an independent execution audit for the others. Counts as
// decompiled, not matched; tools/enroll.py leaves it out of the ROM build, which keeps the
// original bytes for this range. A byte-exact match replaces this file and drops the banner.
#pragma opt_strength_reduction off
#pragma opt_common_subs off

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef int s32;

extern "C" {
extern void func_02012718(void *a, int b);
extern void func_02012dbc(int a);
extern int func_ov006_020d25fc(void *thiz, int idx, int val);
extern int func_020126e8(int a);
extern void func_020126ac(int a0, int a1, int a2, int a3, int a4);
extern int data_ov006_0212e1c0[];
}

struct Obj {
    virtual void m00(); virtual void m04(); virtual void m08(); virtual void m0c();
    virtual void m10(); virtual void m14(); virtual void m18(); virtual void m1c();
    virtual void m20(); virtual void m24(); virtual void m28(); virtual void m2c();
    virtual void m30(); virtual void m34(); virtual void m38(); virtual void m3c();
    virtual void m40(); virtual void m44(); virtual void m48(); virtual void m4c();
    virtual void m50(); virtual void m54(); virtual void m58(); virtual void m5c();
    virtual void m60(); virtual void m64(); virtual void m68(); virtual void m6c();
    virtual void m70(); virtual void m74(); virtual void m78(); virtual void m7c();
    virtual void m80(); virtual void m84(); virtual void m88(); virtual void m8c();
    virtual int m90();
};

extern "C" void func_ov006_020d27dc(void *arg0)
{
    char *p = (char *)arg0;
    s32 var_r4;
    s32 idx;
    s32 count;

    for (var_r4 = 0; var_r4 < (count = *(s32 *)(p + 0x46c8)); ) {
        s32 *slot = (s32 *)((int)p + var_r4 * 4 + 0x46b8);
        s32 v = *slot;
        var_r4++;
        if (v > 0) {
            *slot = v - 1;
        }
    }

    idx = 0;
    if (count <= 0) {
        return;
    }

    {
        s32 v20 = 0xd7;
        s32 v1c = 0xcf;
        s32 v18 = 0xc7;
        s32 v14 = 0xbf;
        s32 v10 = 0xe0;
        s32 v34 = 2;
        s32 v30 = 5;
        s32 v38 = 7;
        s32 vC = 0;
        s32 v60 = 0;
        s32 v5c = 0;
        s32 v58 = 0;
        s32 v54 = 0;
        s32 v50 = 0;
        s32 v4c = 0;
        s32 v48 = 0;
        s32 v44 = 0;
        s32 v40 = 0;
        s32 c1 = 1;
        s32 c4 = 4;
        s32 c6 = 6;
        s32 v24 = 0x1c0;
        s32 c3 = 3;
        s32 v3c = 0;
        s32 v2c = 0;
        s32 v28 = 0;

        do {
            if (*(s32 *)(p + idx * 4 + 0x46b8) > 0) {
                continue;
            }

            {
                int ent = (int)p + idx * 8;
                s32 tmp;
                s32 *distp = (s32 *)(ent + 0x4664);
                s32 dist = *distp;
                s32 lim = *(s32 *)(p + 0x4700);

                if (dist > lim) {
                    u8 *flag = (u8 *)(p + idx + 0x46b4);
                    if (*flag != 1) {
                        s32 sl = vC;
                        s32 *dirSlot;
                        s32 dirVal;

                        if (((struct Obj *)p)->m90() != 0) {
                            u32 want = *(u32 *)(p + idx * 4 + 0x46a4);
                            dirSlot = (s32 *)(p + idx * 8 + 0x4660);
                            dirVal = *dirSlot;
                            switch (dirVal) {
                            case 0x20:
                                if (*(s32 *)(p + 0x4714) == (s32)want) {
                                    sl = c1;
                                } else {
                                    *(u8 *)(p + 0x5398) = c1;
                                }
                                break;
                            case 0x60:
                                if (*(s32 *)(p + 0x4718) == (s32)want) {
                                    sl = c1;
                                } else {
                                    *(u8 *)(p + 0x5399) = c1;
                                }
                                break;
                            case 0xa0:
                                if (*(s32 *)(p + 0x471c) == (s32)want) {
                                    sl = c1;
                                } else {
                                    *(u8 *)(p + 0x539a) = c1;
                                }
                                break;
                            case 0xe0:
                                if (*(s32 *)(p + 0x4720) == (s32)want) {
                                    sl = c1;
                                } else {
                                    *(u8 *)(p + 0x539b) = c1;
                                }
                                break;
                            }
                        } else {
                            dirSlot = (s32 *)(ent + 0x4660);
                            dirVal = *dirSlot;
                            switch (dirVal) {
                            case 0x20:
                                if (*(s32 *)(p + 0x4714) == 1) {
                                    sl = c1;
                                } else {
                                    *(u8 *)(p + 0x5398) = c1;
                                }
                                break;
                            case 0x60:
                                if (*(s32 *)(p + 0x4718) == 1) {
                                    sl = c1;
                                } else {
                                    *(u8 *)(p + 0x5399) = c1;
                                }
                                break;
                            case 0xa0:
                                if (*(s32 *)(p + 0x471c) == 1) {
                                    sl = c1;
                                } else {
                                    *(u8 *)(p + 0x539a) = c1;
                                }
                                break;
                            case 0xe0:
                                if (*(s32 *)(p + 0x4720) == 1) {
                                    sl = c1;
                                } else {
                                    *(u8 *)(p + 0x539b) = c1;
                                }
                                break;
                            }
                        }

                        if (sl == 1) {
                            if (((struct Obj *)p)->m90() != 0) {
                                if (*(u8 *)(p + 0x46d5) == 0) {
                                    func_02012718((void *) v10, *dirSlot << 0xc);
                                    u32 w2 = *(u32 *)(p + idx * 4 + 0x46a4);
                                    switch (w2) {
                                    default:
                                        break;
                                    case 0:
                                        func_02012718((void *) v14, *dirSlot << 0xc);
                                        break;
                                    case 1:
                                        func_02012718((void *) v18, *dirSlot << 0xc);
                                        break;
                                    case 2:
                                        func_02012718((void *) v1c, *dirSlot << 0xc);
                                        break;
                                    case 3:
                                        func_02012718((void *) v20, *dirSlot << 0xc);
                                        break;
                                    }
                                } else {
                                    func_02012718((void *) 0x1c3, *dirSlot << 0xc);
                                }
                                *flag = c1;
                                *(s32 *)(p + 0x46cc) += 1;
                                if (*(s32 *)(p + 0x46cc) >= *(s32 *)(p + 0x46c8)) {
                                    if (*(u8 *)(p + 0x46d5) == 0) {
                                        *(s32 *)((int)p + 0x5374) += 1;
                                        *(s32 *)(p + 0x53e8) += 1;
                                        if (*(s32 *)(p + 0x53e8) > 0x270f) {
                                            *(s32 *)(p + 0x53e8) = 0x270f;
                                        }
                                        *(s32 *)(p + 0x53e0) = 0x78;
                                        return;
                                    }
                                    *(s32 *)(p + 0x53e0) = 0x1e;
                                    return;
                                }
                                continue;
                            }

                            {
                                s32 *d2 = (s32 *)((int)(p + idx * 8) + 0x4660);
                                func_02012718((void *) v10, *d2 << 0xc);
                                func_02012718((void *) 0x1c1, *d2 << 0xc);
                            }
                            *flag = c1;
                            *(s32 *)(p + 0x46cc) += 1;
                            if (*(s32 *)(p + 0x46cc) >= *(s32 *)(p + 0x46c8)) {
                                s32 t;
                                *(s32 *)((int)p + 0x5374) += 1;
                                *(s32 *)(p + 0x53e8) += 1;
                                if (*(s32 *)(p + 0x53e8) > 0x270f) {
                                    *(s32 *)(p + 0x53e8) = 0x270f;
                                }
                                t = *(s32 *)((char *)data_ov006_0212e1c0 + *(s32 *)(p + 0x53d4) * 0x1c);
                                switch (t) {
                                case 0:
                                    break;
                                case 1:
                                    if ((*(s32 *)(p + 0x5374) % 2) == 0) {
                                        *(s32 *)(p + 0x5368) += 5;
                                    }
                                    break;
                                case 2:
                                    *(s32 *)(p + 0x5368) += 5;
                                    break;
                                }
                                if (*(s32 *)(p + 0x5368) > 0x64) {
                                    *(s32 *)(p + 0x5368) = 0x64;
                                }
                                if (*(s32 *)((int)p + 0x5374) >= 5) {
                                    *(s32 *)(p + 0x53c0) = 0x3c;
                                    *(s32 *)(p + 0x46d0) = 2;
                                    *(u8 *)(p + 0x46d4) = 1;
                                    return;
                                }
                                *(s32 *)(p + 0x46d0) = 0;
                                return;
                            }
                            continue;
                        }

                        if (((struct Obj *)p)->m90() != 0) {
                            func_02012dbc(5);
                            if (*(u8 *)(p + 0x46d5) == 0) {
                                func_02012718((void *) 0x1c2, *(s32 *)(p + idx * 8 + 0x4660) << 0xc);
                            } else {
                                func_02012718((void *) 0x1c3, *(s32 *)(p + idx * 8 + 0x4660) << 0xc);
                            }
                            *flag = 1;
                            *(u8 *)(p + 0x46d5) = 1;
                            *(s32 *)(p + 0x46cc) += 1;
                            if (*(s32 *)(p + 0x46cc) >= *(s32 *)(p + 0x46c8)) {
                                *(s32 *)(p + 0x53e0) = 0x3c;
                            }
                            return;
                        }
                        func_02012718((void *) 0xe1, *(s32 *)(p + idx * 8 + 0x4660) << 0xc);
                        *(s32 *)(p + 0x53c0) = 0x3c;
                        *(s32 *)(p + 0x46d0) = 2;
                        *(u8 *)(p + 0x46d5) = 1;
                        return;
                    }
                    continue;
                }

                if (dist < -0xc0 || dist >= 0x98) {
                    *distp += 1;
                } else {
                    u8 *flag2 = (u8 *)(p + idx + 0x4680);
                    if ((u32)*flag2 <= 1) {
                        *distp = dist + 1;
                        if (func_ov006_020d25fc(p, idx, c3) == 0) {
                            func_ov006_020d25fc(p, idx, c4);
                        }
                    } else {
                        tmp = ((dist + 0xd4) * 0x1f4) / (lim + 0xd4);
                        s32 *statePtr = (s32 *)(p + idx * 4 + 0x4684);
                        u32 state = *statePtr;
                        switch (state) {
                        default:
                            break;
                        case 0:
                            if (*(s32 *)(p + idx * 8 + 0x4660) == 0x21 || *(s32 *)(p + idx * 8 + 0x4660) == 0x61 ||
                                *(s32 *)(p + idx * 8 + 0x4660) == 0xa1 || *(s32 *)(p + idx * 8 + 0x4660) == 0xe1) {
                                {
                                    s32 *dirp = (s32 *)((int)(p + idx * 8) + 0x4660);
                                    *dirp -= 1;
                                    *statePtr = c6;
                                    *flag2 = c1;
                                    func_020126ac(v24, c6, v28, tmp, func_020126e8(*dirp << 0xc));
                                }
                            } else if (func_ov006_020d25fc(p, idx, v2c) == 0 && func_ov006_020d25fc(p, idx, c1) == 0 &&
                                       func_ov006_020d25fc(p, idx, c3) == 0 && func_ov006_020d25fc(p, idx, v30) == 0) {
                                if (func_ov006_020d25fc(p, idx, v34) == 0) {
                                    *statePtr = v38;
                                }
                            }
                            break;
                        case 1:
                            if (func_ov006_020d25fc(p, idx, c1) == 0 && func_ov006_020d25fc(p, idx, v3c) == 0 &&
                                func_ov006_020d25fc(p, idx, v34) == 0 && func_ov006_020d25fc(p, idx, c3) == 0) {
                                if (func_ov006_020d25fc(p, idx, c4) == 0) {
                                    *statePtr = c6;
                                }
                            }
                            break;
                        case 2:
                            if (*(s32 *)(p + idx * 8 + 0x4660) == 0x1f || *(s32 *)(p + idx * 8 + 0x4660) == 0x5f ||
                                *(s32 *)(p + idx * 8 + 0x4660) == 0x9f || *(s32 *)(p + idx * 8 + 0x4660) == 0xdf) {
                                {
                                    s32 *dirp = (s32 *)((int)(p + idx * 8) + 0x4660);
                                    *dirp += 1;
                                    *statePtr = c6;
                                    *flag2 = c1;
                                    func_020126ac(v24, c6, v40, tmp, func_020126e8(*dirp << 0xc));
                                }
                            } else if (func_ov006_020d25fc(p, idx, v34) == 0 && func_ov006_020d25fc(p, idx, c1) == 0 &&
                                       func_ov006_020d25fc(p, idx, c4) == 0 && func_ov006_020d25fc(p, idx, v44) == 0) {
                                if (func_ov006_020d25fc(p, idx, v38) == 0) {
                                    *statePtr = v30;
                                }
                            }
                            break;
                        case 3:
                            if (*(s32 *)(p + idx * 8 + 0x4660) == 0x21 || *(s32 *)(p + idx * 8 + 0x4660) == 0x61 ||
                                *(s32 *)(p + idx * 8 + 0x4660) == 0xa1 || *(s32 *)(p + idx * 8 + 0x4660) == 0xe1) {
                                {
                                    s32 *dirp = (s32 *)((int)(p + idx * 8) + 0x4660);
                                    *dirp -= 1;
                                    *statePtr = c6;
                                    *flag2 = c1;
                                    func_020126ac(v24, c6, v48, tmp, func_020126e8(*dirp << 0xc));
                                }
                            } else if (func_ov006_020d25fc(p, idx, c3) == 0 && func_ov006_020d25fc(p, idx, v4c) == 0 &&
                                       func_ov006_020d25fc(p, idx, v30) == 0 && func_ov006_020d25fc(p, idx, c1) == 0) {
                                if (func_ov006_020d25fc(p, idx, c6) == 0) {
                                    *statePtr = c4;
                                }
                            }
                            break;
                        case 4:
                            if (*(s32 *)(p + idx * 8 + 0x4660) == 0x1f || *(s32 *)(p + idx * 8 + 0x4660) == 0x5f ||
                                *(s32 *)(p + idx * 8 + 0x4660) == 0x9f || *(s32 *)(p + idx * 8 + 0x4660) == 0xdf) {
                                {
                                    s32 *dirp = (s32 *)((int)(p + idx * 8) + 0x4660);
                                    *dirp += 1;
                                    *statePtr = c6;
                                    *flag2 = c1;
                                    func_020126ac(v24, c6, v50, tmp, func_020126e8(*dirp << 0xc));
                                }
                            } else if (func_ov006_020d25fc(p, idx, c4) == 0 && func_ov006_020d25fc(p, idx, v34) == 0 &&
                                       func_ov006_020d25fc(p, idx, v38) == 0 && func_ov006_020d25fc(p, idx, c1) == 0) {
                                if (func_ov006_020d25fc(p, idx, c6) == 0) {
                                    *statePtr = c3;
                                }
                            }
                            break;
                        case 5:
                            if (*(s32 *)(p + idx * 8 + 0x4660) == 0x21 || *(s32 *)(p + idx * 8 + 0x4660) == 0x61 ||
                                *(s32 *)(p + idx * 8 + 0x4660) == 0xa1 || *(s32 *)(p + idx * 8 + 0x4660) == 0xe1) {
                                {
                                    s32 *dirp = (s32 *)((int)(p + idx * 8) + 0x4660);
                                    *dirp -= 1;
                                    *statePtr = c6;
                                    *flag2 = c1;
                                    func_020126ac(v24, c6, v54, tmp, func_020126e8(*dirp << 0xc));
                                }
                            } else if (func_ov006_020d25fc(p, idx, v30) == 0 && func_ov006_020d25fc(p, idx, c6) == 0 &&
                                       func_ov006_020d25fc(p, idx, c3) == 0 && func_ov006_020d25fc(p, idx, v58) == 0) {
                                if (func_ov006_020d25fc(p, idx, v38) == 0) {
                                    *statePtr = v34;
                                }
                            }
                            break;
                        case 6:
                            if (func_ov006_020d25fc(p, idx, c6) == 0 && func_ov006_020d25fc(p, idx, v30) == 0 &&
                                func_ov006_020d25fc(p, idx, v38) == 0 && func_ov006_020d25fc(p, idx, c3) == 0) {
                                if (func_ov006_020d25fc(p, idx, c4) == 0) {
                                    *statePtr = c1;
                                }
                            }
                            break;
                        case 7:
                            if (*(s32 *)(p + idx * 8 + 0x4660) == 0x1f || *(s32 *)(p + idx * 8 + 0x4660) == 0x5f ||
                                *(s32 *)(p + idx * 8 + 0x4660) == 0x9f || *(s32 *)(p + idx * 8 + 0x4660) == 0xdf) {
                                {
                                    s32 *dirp = (s32 *)((int)(p + idx * 8) + 0x4660);
                                    *dirp += 1;
                                    *statePtr = c6;
                                    *flag2 = c1;
                                    func_020126ac(v24, c6, v5c, tmp, func_020126e8(*dirp << 0xc));
                                }
                            } else if (func_ov006_020d25fc(p, idx, v38) == 0 && func_ov006_020d25fc(p, idx, c6) == 0 &&
                                       func_ov006_020d25fc(p, idx, c4) == 0 && func_ov006_020d25fc(p, idx, v30) == 0 &&
                                       func_ov006_020d25fc(p, idx, v34) == 0) {
                                *statePtr = v60;
                            }
                            break;
                        }
                    }
                }
            }
        } while (++idx < *(s32 *)(p + 0x46c8));
    }
}
