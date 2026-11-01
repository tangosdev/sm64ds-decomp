/* storage 0x02389454 runtime 0x038012EC size 0x17C + 12 pool wram ARM.
 * Three pool data refs (unk_38094E8 = 0x038094E8, unk_38094E0 = 0x038094E0,
 * unk_38094DC = 0x038094DC). Dense 6-case jump table (SUB 0x61 + CMP 5 +
 * ADDLS PC); case blocks lay out in source order a, d, e, c, f, default
 * (like fn_02389A6C's 15/14). In-pack callees fn_02389958/fn_02389898/
 * fn_02389A6C/fn_02389A04; the rest out-of-pack. All paths share one
 * epilogue; default (last) falls through without B. */

extern int fn_02382AC4(void);
extern int fn_023883E0(int a1);
extern int fn_023883C8(int a1);
extern int fn_02382AD8(int a1);
extern int fn_02389BA0(void);
extern int fn_023883FC(int a1, int a2);
extern int fn_02388384(int a1);
extern volatile unsigned short *fn_02389958(int a1, int a2);
extern int fn_02389898(int a1);
extern int *fn_02389A6C(int a1);
extern int *fn_02389A04(int a1);
extern int unk_38094E8[];
extern unsigned short unk_38094E0;
extern unsigned short unk_38094DC;

int fn_02389454(int *a1) {
    int v2 = fn_02382AC4();
    if (fn_023883E0(3) == 0) {
        fn_02382AD8(v2);
        return fn_023883FC((unsigned short)a1[1], 4);
    }
    fn_023883C8(3);
    fn_02382AD8(v2);
    {
        int v4 = a1[1];
        switch (v4) {
        case 'a':
            ((int *)unk_38094E8)[8] = 1;
            unk_38094E0 = a1[2];
            unk_38094DC = a1[3];
            fn_02389BA0();
            break;
        case 'd': {
            int v6, v7;
            ((int *)unk_38094E8)[8] = 4;
            v6 = a1[2];
            ((int *)unk_38094E8)[10] = v6;
            v7 = a1[3];
            ((int *)unk_38094E8)[9] = v7;
            fn_02389958((unsigned short)v6, v7 & 0xFF);
            fn_023883FC(0x64, 0);
            break;
        }
        case 'e': {
            int v9, v10, v11;
            ((int *)unk_38094E8)[8] = 3;
            v9 = a1[2];
            ((int *)unk_38094E8)[10] = v9;
            v10 = (unsigned short)v9;
            v11 = fn_02389898(v10);
            fn_023883FC((unsigned short)(v10 + 112), v11);
            break;
        }
        case 'c': {
            int t;
            ((int *)unk_38094E8)[8] = 2;
            t = a1[2];
            ((int *)unk_38094E8)[9] = t;
            fn_02389A6C(t);
            fn_023883FC(0x63, 0);
            break;
        }
        case 'f':
            fn_02389A04(a1[2]);
            break;
        default:
            fn_023883FC((unsigned short)v4, 1);
        }
    }
    return fn_02388384(3);
}
