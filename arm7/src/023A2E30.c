/* storage 0x023A2E30 runtime 0x027D4DA8 size 0x114 main. */

extern void fn_0239A98C(unsigned short a1, int a2);
extern int fn_0239B488(short *a1, unsigned short a2);
extern int fn_0239F9AC(int a1);
extern int fn_0239F064(short *a1, int a2);
extern int fn_0239E91C(short *a1, int a2);
extern int fn_0239F778(int a1);
extern void fn_023969F4(int a1);
extern void fn_02396A98(void);

int fn_023A2E30(unsigned short *a1) {
    int v2;
    char *v3;
    int result;
    int v5;

    v2 = a1[1];
    v3 = *(char **)0x0380FFF4;
    result = *(unsigned short *)(v3 + 0x300 + 0x14);
    switch (result) {
    case 1: {
        v5 = *(unsigned short *)(*(char **)(v3 + 736) + 28 * v2);
        if (v5 == 64) {
            fn_0239A98C((unsigned short)v2, 0x30);
            fn_0239B488((short *)((char *)a1 + 30), a1[22]);
            return fn_0239F9AC(v2);
        } else {
            if (v5 == 48)
                result = fn_0239F064((short *)((char *)a1 + 30), 7);
            else
                result = fn_0239E91C((short *)((char *)a1 + 30), 7);
            if (result != 0)
                return fn_0239F778(result);
        }
        break;
    }
    case 2:
    case 3: {
        result = (int)(v3 + 768);
        if (*(unsigned short *)(result + 16) == 64
            && a1[17] == *(unsigned short *)(result + 130)
            && a1[16] == *(unsigned short *)(result + 128)) {
            v5 = *(volatile unsigned short *)&a1[15];
            result = *(unsigned short *)(result + 126);
            if (v5 == result) {
                fn_023969F4(48);
                fn_02396A98();
                return fn_0239B488((short *)((char *)a1 + 30), a1[22]);
            }
        }
        break;
    }
    }
    return result;
}
