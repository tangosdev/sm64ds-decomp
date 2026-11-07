/* storage 0x023802D4 runtime 0x037F816C size 0xF0 + 4 pool wram ARM.
 * One pool data ref (buf_27FFC80 = 0x027FFC80, shared by 3 uses), six BL
 * calls (two in-pack). The shuffle helper returns long long: the index uses
 * the high word straight out of R1 (SUB R0,R2,R1,LSL#1), which is why the
 * multiplier is never reloaded from R6 after the call. v10 is volatile so
 * the 0 is stored and reloaded around the fill call, as in the ROM. */

extern int fn_0238D084(int a1, int a2, int *a3);
extern int fn_02382DC0(int *a1, int *a2, int a3);
extern int fn_02380214(int *a1);
extern int fn_02382CC8(int *s, int *d, int n);
extern int fn_02382CB4(int v, int *p, int n);
extern long long fn_0238D594(int a1, int a2);
extern int buf_27FFC80[];

int fn_023802D4(void) {
    int v7;
    unsigned char v8[6];
    unsigned char v9[6];
    volatile int v10;
    int v11[29];
    char v12[116];
    int v3, i, two = 2;
    fn_0238D084(32, 2, &v7);
    v7 *= 8;
    fn_0238D084(v7, 116, v11);
    fn_0238D084(v7 + 256, 116, (int *)v12);
    v3 = fn_02380214(v11);
    if (v3 != 0)
        fn_02382CC8((int *)((char *)v11 + 116 * (v3 - 1)), buf_27FFC80, 116);
    else {
        v10 = 0;
        fn_02382CB4(v10, buf_27FFC80, 116);
    }
    fn_0238D084(54, 6, (int *)v8);
    i = 0;
    do {
        long long r = fn_0238D594(i, two);
        v9[i] = v8[i + 1 - 2 * (int)(r >> 32)];
        i++;
    } while (i < 6);
    {
        int off = (int)buf_27FFC80;
        off += 116;
        return fn_02382DC0((int *)v9, (int *)off, 6);
    }
}
