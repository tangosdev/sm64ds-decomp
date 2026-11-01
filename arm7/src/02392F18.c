/* storage 0x02392F18 runtime 0x027C4E90 linked 0x210 main ARM.
 * Varargs command-list walker. Reads u16 commands starting at &a2 (the
 * va_start cursor): low byte * 2 is the payload size, bits 0x1000/0x800 each
 * add 2 bytes, and 0xF00 picks a table index. The 0x800 path updates the
 * per-a1 table at +0x1F4; the 0x1000 path gates on a bitmask field. When the
 * payload size is nonzero a record is built through fn_0238DE4C/fn_0238DEF4. */

typedef char *va_list;

extern void *fn_0238E688(void);
extern int fn_0238DE4C(void);
extern int fn_0238DEF4(int a1);

void fn_02392F18(int a1, int a2, va_list v18, int v2, int varg_r1, ...) {
    va_list varg_r1a;
    int v4;
    int v19;
    int flag;
    unsigned short *v6;
    unsigned int v7;
    int v8;
    unsigned int v12;
    int v5;
    int v9;
    int v10;
    int v11;
    char *v13;
    unsigned short *tbl;
    unsigned short *v14;
    unsigned short v16;
    va_list v20;
    int *v17;

    varg_r1a = (va_list)&a2;
    v4 = **(int **)((char *)fn_0238E688() + 0x1610);
    if (v2 != 0 && (unsigned int)v2 <= ((unsigned short *)v4)[0xC8] &&
        a1 != ((unsigned short *)v4)[0xC2] && (v2 & 1) == 0) {
        v19 = 1;
        tbl = (unsigned short *)(v4 + 0x1F4 + a1 * 16);
        while (v2 > 0) {
            v5 = 0xFFFF;
            if (v19 == 1) {
                v19 = 0;
                v6 = (unsigned short *)varg_r1a;
            } else {
                v6 = (unsigned short *)v18;
                v18 += 2;
                v2 -= 2;
            }
            v7 = *v6;
            v8 = 2 * (v7 & 0xFF);
            if (v8 == 0)
                v8 = 0x200;
            int result = (v7 & 0x1000) != 0;
            flag = (v7 & 0x800) != 0;
            v9 = result ? 2 : 0;
            v10 = flag ? 2 : 0;
            v11 = v8 + v10;
            v11 += v9;
            v2 -= v11;
            if (v2 < 0)
                return;
            v12 = ((unsigned)(v7 & 0xF00) << 8) >> 16;
            v20 = v18;
            v13 = (char *)v18 + v8;
            v18 += v11;
            if (flag == 1) {
                v14 = &tbl[v12 & 7];
                v5 = *(unsigned short *)v13;
                v13 += 2;
                v16 = *v14;
                if ((v16 & 1) != 0) {
                    *v14 = 2 * v5;
                } else {
                    v5 = (unsigned short)(2 * v5);
                    if (v16 == v5)
                        continue;
                    *v14 = v5;
                }
            }
            if (result == 1) {
                result = *(unsigned short *)v13 &
                         (1 << ((unsigned short *)(v4 + 0x100))[0x42]);
                if (result == 0)
                    continue;
            }
            if (v8 > 0) {
                unsigned short fld;
                v17 = (int *)fn_0238DE4C();
                *(unsigned short *)v17 = 130;
                *(unsigned short *)((char *)v17 + 2) = 0;
                *(unsigned short *)((char *)v17 + 4) = 21;
                *(unsigned short *)((char *)v17 + 6) = v12;
                v17[2] = varg_r1;
                v17[3] = (int)v20;
                *(unsigned short *)((char *)v17 + 0x10) = v8;
                *(unsigned short *)((char *)v17 + 0x12) = a1;
                fld = ((unsigned short *)(v4 + 0x100))[0x42];
                *(unsigned short *)((char *)v17 + 0x20) = fld;
                *(unsigned short *)((char *)v17 + 0x1A) = v5;
                fn_0238DEF4((int)v17);
            }
        }
    }
}
