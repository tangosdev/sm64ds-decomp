/* storage 0x023A278C runtime 0x027D4704 size 0x1B0 main. */

extern void fn_023954C4(unsigned short a1, unsigned short a2, char *a3, int a4);
extern void fn_02396B68(unsigned short a1);
extern void fn_0239E8C8(unsigned short a1);
extern void fn_023969D8(char *a1, short *a2);
extern short fn_0239AD50(short *a1);
extern void fn_0239A98C(unsigned short a1, int a2);
extern void fn_023969F4(int a1);
extern void fn_0239477C(int a1, int a2);

void fn_023A278C(unsigned short *a1, int a2, int a3, int a4) {
    unsigned short *v5;
    char *v6;
    int v7;
    unsigned short *v8;
    short v11;

    v5 = a1 + 22;
    v6 = *(char **)0x0380FFF4;
    v7 = *(unsigned short *)(v6 + 0x300 + 0x14);
    switch (v7) {
    case 2:
    case 3:
        if (*(unsigned short *)(v6 + 0x300 + 0xBC) == 97) {
        v8 = *(unsigned short **)(v6 + 980);
        if (v8[10] == a1[17] && v8[9] == a1[16]) {
            unsigned short t1;
            unsigned short t0;
            if ((t1 = v8[8], t0 = a1[15], t1 == t0)) {
            fn_023954C4(t0, t1, (char *)v8, a4);
            if (v5[1] == 0) {
                fn_02396B68(v5[2] & 0xFFF);
                fn_0239E8C8(*(unsigned short *)(*(char **)0x0380FFF4 + 0x300 + 0x66));
                fn_023969D8(*(char **)0x0380FFF4 + 894, (short *)(a1 + 15));
                v11 = fn_0239AD50((short *)(a1 + 15));
                *(unsigned short *)(*(char **)0x0380FFF4 + 0x300 + 0x84) = v11;
                fn_0239A98C(*(unsigned short *)(*(char **)0x0380FFF4 + 0x300 + 0x84), 0x40);
                fn_0239A98C(*(unsigned short *)(*(char **)0x0380FFF4 + 0x300 + 0x84), 0x40);
                fn_023969F4(64);
            }
            if (v5[1] == 0) {
                *(unsigned short *)(*(char **)(*(char **)0x0380FFF4 + 984) + 4) = 0;
                *(unsigned short *)(*(char **)(*(char **)0x0380FFF4 + 984) + 6) = 0;
                fn_023969F4(64);
            } else {
                *(unsigned short *)(*(char **)(*(char **)0x0380FFF4 + 984) + 4) = 12;
                *(unsigned short *)(*(char **)(*(char **)0x0380FFF4 + 984) + 6) = v5[1];
            }
            *(unsigned short *)(*(char **)(*(char **)0x0380FFF4 + 984) + 8) = *(unsigned short *)(*(char **)0x0380FFF4 + 0x300 + 0x66);
            *(unsigned short *)(*(char **)0x0380FFF4 + 0x300 + 0xBC) = 99;
            fn_0239477C(2, 4);
            }
        }
        }
        break;
    }
}
