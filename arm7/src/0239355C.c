/* storage 0x0239355C runtime 0x027C54D4 size 0x534 (listed 0x520 + pool) main ARM.
 * MATCHED 2004/b56 -O4,s (AH38). 331-word loop nest.
 * Wall key (AR28 4-word + AR68 5-word site at +0x358): the two odd-path halfword
 * loads are volatile-expression reads. t194's volatile pins the base+0x100 ADD
 * adjacent to its load AND takes r2; v17[1]'s volatile pins H2+AND after it
 * (AND into r1). Either volatile alone reopens the wall; both match. Plausible
 * as written: these tables are ARM9-shared sound-heap memory. */

extern void *fn_0238E688(void);
extern void fn_023817E0(void *a1);
extern void fn_0238176C(void *a1);
extern int fn_02382C98(int a1, void *a2, int a3);

int fn_0239355C(int a1, int a2, unsigned short *a3, unsigned short *a4, unsigned short *a5, unsigned short *a6) {
    int v15;
    int v16;
    int v28;
    int v29;
    int v30;
    int v31;
    int v32;
    int v33;
    unsigned short *v34;
    unsigned v35;
    unsigned short *v36;
    unsigned short *v37;
    unsigned short *v38;
    int v39;
    int v40;
    int v41;
    int v42;
    int one;
    int za;
    int zb;
    int zc;
    int two;
    int zd;
    int ze;
    int zf;
    int zg;
    unsigned short *v10;
    unsigned short *p5 = a5;
    unsigned short *p6 = a6;
    int v11;
    int v13;
    unsigned short *v17;
    int v18;
    int v19;
    int v20;
    int v21;
    int v22;
    int v24;
    unsigned short v25;
    int v26;
    int v27;
    {
        int *p = (int *)((char *)fn_0238E688() + 0x1000);
        v10 = *(unsigned short **)(p[0x184]);
    }
    v11 = *v10;
    if (v11 == 9) {
        v32 = 1;
    } else if (v11 == 10) {
        v32 = 0;
    } else {
        return 1;
    }
    v33 = 0;
    *a3 = 0;
    *p5 = v33;
    *p6 = v33;
    v13 = v33;
    v30 = (unsigned short)(v32 == 1 ? v10[67] : 1);
    if (a2 < 0)
        return 1;
    v15 = 0;
    v31 = 1;
    fn_023817E0((char *)v10 + 0x918);
    if (*(int *)((char *)v10 + 0x92C) == 1) {
        fn_0238176C((char *)v10 + 0x918);
        return 1;
    }
    one = 1;
    *(int *)((char *)v10 + 0x92C) = 1;
    v34 = v10 + 378;
    v35 = v15;
    v41 = ~(v30 & a1);
    za = v35;
    zb = v35;
    zc = v35;
    two = 2;
    zd = v35;
    ze = v35;
    zf = v35;
    zg = v35;
    while (v35 < 4 && a2 > 2) {
        v36 = (unsigned short *)((char *)v10 + 0x908 + v35 * 4);
        v37 = (unsigned short *)((char *)v10 + 0x8F8 + v35 * 4);
        v38 = v36;
        v39 = 0xFFFF;
        v16 = *(unsigned short *)((char *)v10 + 0x908 + v35 * 4);
        while (v16 != 0xFFFF && a2 > 2) {
            v17 = (unsigned short *)((char *)v34 + v16 * 24);
            v18 = v17[1];
            v42 = (unsigned short)(one << v18);
            if ((unsigned short)(v15 & v42) == 0) {
                v15 |= v42;
                v19 = v17[2];
                v40 = (unsigned short)(v19 & v30);
                if (v32 == 0 || (v40 & v41 & ~1) == 0) {
                    v29 = (v18 & 8) ? one : za;
                    v28 = (v32 != 0 && (v19 | 1) != 0xFFFF) ? one : zb;
                    if ((v17[3] & 1) == 1)
                        ++v17[3];
                    v20 = v29 ? two : zc;
                    v21 = v31 ? zd : two;
                    v22 = v28 ? two : ze;
                    {
                        int s1 = v17[3] + v21;
                        if (s1 + v20 + v22 <= a2) {
                        if (v31 == 0) {
                            a3 = a4;
                            *a4++ = zf;
                            v13 = (unsigned short)(v13 + 2);
                            a2 -= 2;
                        }
                        *a3 |= (v17[1] << 8) & 0xF00;
                        *a3 |= ((unsigned)v17[3] >> 1) & 0xFF;
                        fn_02382C98(*(int *)&v17[6], a4, v17[3]);
                        {
                            int v23 = v17[3];
                            a4 = (unsigned short *)((char *)a4 + (v23 & ~1));
                            v13 = (unsigned short)(v13 + v23);
                            a2 -= v23;
                        }
                        if (v29 == 1) {
                            v24 = v17[4];
                            if ((v24 & 1) != 0) {
                                int t194 = (*(volatile unsigned short *)&v10[194]);
                                int y0 = (*(volatile unsigned short *)&v17[1]) & 7;
                                v26 = (int)((char *)v10 + t194 * 16 + y0 * 2);
                                {
                                    unsigned short *pw = (unsigned short *)((char *)v26 + 0x100);
                                    v25 = pw[0x7A];
                                }
                                *(unsigned short *)(v26 + 0x1F4) = *(unsigned short *)(v26 + 0x1F4) + 1;
                                v17[4] = 2 * v25;
                            } else {
                                v25 = 0x7FFF & (v24 >> 1);
                            }
                            *a4++ = v25;
                            v13 = (unsigned short)(v13 + 2);
                            a2 -= 2;
                        }
                        if (v28 == 1) {
                            *a3 |= 0x1000;
                            *a4++ = v40;
                            v13 = (unsigned short)(v13 + 2);
                            a2 -= 2;
                        }
                        if (v31 == 1)
                            v31 = zg;
                        if (v36[1] == v16)
                            v36[1] = v39;
                        *v38 = *v17;
                        *v17 = 0xFFFF;
                        v27 = v37[1];
                        if (v27 == 0xFFFF)
                            *v37 = v16;
                        else
                            *(unsigned short *)((char *)v34 + v27 * 24) = v16;
                        v37[1] = v16;
                        v15 = (unsigned short)(v15 & ~v42);
                        v33 |= v17[2];
                        v16 = v39;
                        if (v10[74] == 1)
                            goto done;
                    }
                    if (v16 != 0xFFFF)
                        v38 = (unsigned short *)((char *)v34 + v16 * 24);
                    else
                        v38 = v36;
                    v39 = v16;
                    }
                }
            }
            if (v16 != 0xFFFF)
                v16 = *(unsigned short *)((char *)v34 + v16 * 24);
            else
                v16 = *v36;
        }
        v35 = (unsigned short)(v35 + 1);
    }
done:
    *p5 = v33;
    *p6 = v13;
    fn_0238176C((char *)v10 + 0x918);
    return 0;
}
