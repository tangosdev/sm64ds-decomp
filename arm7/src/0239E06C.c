/* storage 0x0239E06C runtime 0x027CFFE4 linked 0x58C main ARM. */
extern void fn_02382CB4(int v, void *dst, int n);
extern unsigned short fn_023963F0(void);
extern void fn_023969A4(void *a, void *b, void *c);
extern int fn_0239F228();
extern void fn_023A0398(void);
extern void fn_0239FED0(void);
extern void fn_0239FD44(void);

void fn_0239E06C(void) {
    volatile int z0 = 0;
    int p0;
    fn_02382CB4(z0, *(unsigned char **)0x0380FFF4 + 996, 180);
    p0 = (int)0x0380FFF4;
    *(unsigned short *)(*(unsigned char **)0x0380FFF4 + 996) = 0;
    *(unsigned int *)(*(unsigned char **)0x0380FFF4 + 1008) = 0;
    *(unsigned short *)(*(unsigned char **)0x0380FFF4 + 1016) = 0;
    *(unsigned int *)(*(unsigned char **)0x0380FFF4 + 1028) = 0;
    *(unsigned short *)(*(unsigned char **)0x0380FFF4 + 1036) = 0;
    *(unsigned int *)(*(unsigned char **)0x0380FFF4 + 1048) = 0;
    *(unsigned short *)(*(unsigned char **)0x0380FFF4 + 1160) = 0xFFFF;
    *(unsigned short *)(*(unsigned char **)0x0380FFF4 + 1162) = 0xFFFF;
    switch (*(unsigned short *)(*(unsigned char **)p0 + 788)) {
    case 0: {
        unsigned short v1 = 0xB6B8;
        unsigned short v2 = 0x1D46;
        *(unsigned int *)(*(unsigned char **)0x0380FFF4 + 1004) = 0x04804170;
        *(unsigned int *)(*(unsigned char **)0x0380FFF4 + 1024) = 0x04804028;
        *(unsigned int *)(*(unsigned char **)0x0380FFF4 + 1044) = 0x04804000;
        *(void **)(*(unsigned char **)0x0380FFF4 + 1012) = (void *)fn_023A0398;
        *(void **)(*(unsigned char **)0x0380FFF4 + 1032) = (void *)fn_0239FED0;
        *(void **)(*(unsigned char **)0x0380FFF4 + 1052) = (void *)0x027D1DC4;
        *(unsigned short *)0x04804024 = v1;
        *(unsigned short *)0x04804026 = v2;
        *(unsigned short *)0x0480416C = v1;
        *(unsigned short *)0x0480416E = v2;
        *(unsigned short *)0x04804790 = v1;
        *(unsigned short *)0x04804792 = v2;
        *(unsigned short *)0x048080AE = 1;
        break;
    }
    case 1: {
        unsigned short v1 = 0xB6B8;
        unsigned short v2 = 0x1D46;
        unsigned short w;
        *(unsigned int *)(*(unsigned char **)0x0380FFF4 + 1004) = 0x04804AA0;
        *(unsigned int *)(*(unsigned char **)0x0380FFF4 + 1024) = 0x04804958;
        *(unsigned int *)(*(unsigned char **)0x0380FFF4 + 1044) = 0x04804334;
        *(void **)(*(unsigned char **)0x0380FFF4 + 1012) = (void *)fn_023A0398;
        *(void **)(*(unsigned char **)0x0380FFF4 + 1032) = (void *)fn_0239FED0;
        *(void **)(*(unsigned char **)0x0380FFF4 + 1052) = (void *)fn_0239FD44;
        *(unsigned int *)(*(unsigned char **)0x0380FFF4 + 1124) = 0x04804238;
        *(unsigned int *)(*(unsigned char **)0x0380FFF4 + 1064) = 0x04804000;
        *(unsigned short *)0x04804234 = v1;
        *(unsigned short *)0x04804236 = v2;
        *(unsigned short *)0x04804330 = v1;
        *(unsigned short *)0x04804332 = v2;
        *(unsigned short *)0x04804954 = v1;
        *(unsigned short *)0x04804956 = v2;
        *(unsigned short *)0x04804A9C = v1;
        *(unsigned short *)0x04804A9E = v2;
        *(unsigned short *)0x048050C0 = v1;
        *(unsigned short *)0x048050C2 = v2;
        if (*(unsigned short *)(*(unsigned char **)0x0380FFF4 + 760) == 0)
            *(unsigned short *)(*(unsigned char **)0x0380FFF4 + 902) = 520;
        else
            *(unsigned short *)(*(unsigned char **)0x0380FFF4 + 902) = 0x4208;
        {
            volatile int z1 = 0;
            fn_02382CB4(z1, *(unsigned char **)0x0380FFF4 + 1612, 44);
        }
        w = fn_023963F0();
        *(unsigned short *)(*(unsigned char **)0x0380FFF4 + 1628) = w;
        fn_023969A4(*(unsigned char **)0x0380FFF4 + 1642, *(unsigned char **)0x0380FFF4 + 744, *(unsigned char **)0x0380FFF4 + 864);
        *(unsigned int *)(*(unsigned char **)0x0380FFF4 + 1124) = 0x04804238;
        fn_0239F228();
        break;
    }
    case 2: {
        unsigned short v1 = 0xB6B8;
        unsigned short v2 = 0x1D46;
        unsigned short w;
        *(unsigned int *)(*(unsigned char **)0x0380FFF4 + 1004) = 0x048045D8;
        *(unsigned int *)(*(unsigned char **)0x0380FFF4 + 1024) = 0x04804490;
        *(unsigned int *)(*(unsigned char **)0x0380FFF4 + 1044) = 0x04804468;
        *(void **)(*(unsigned char **)0x0380FFF4 + 1012) = (void *)fn_023A0398;
        *(void **)(*(unsigned char **)0x0380FFF4 + 1032) = (void *)fn_0239FED0;
        *(void **)(*(unsigned char **)0x0380FFF4 + 1052) = (void *)0x027D1DC4;
        *(unsigned int *)(*(unsigned char **)0x0380FFF4 + 1084) = 0x04804000;
        *(unsigned int *)(*(unsigned char **)0x0380FFF4 + 1104) = 0x04804234;
        *(unsigned short *)0x04804230 = v1;
        *(unsigned short *)0x04804232 = v2;
        *(unsigned short *)0x04804464 = v1;
        *(unsigned short *)0x04804466 = v2;
        *(unsigned short *)0x0480448C = v1;
        *(unsigned short *)0x0480448E = v2;
        *(unsigned short *)0x048045D4 = v1;
        *(unsigned short *)0x048045D6 = v2;
        *(unsigned short *)0x04804BF8 = v1;
        *(unsigned short *)0x04804BFA = v2;
        if (*(unsigned short *)(*(unsigned char **)0x0380FFF4 + 760) == 0)
            *(unsigned short *)(*(unsigned char **)0x0380FFF4 + 902) = 264;
        else
            *(unsigned short *)(*(unsigned char **)0x0380FFF4 + 902) = 0x4108;
        {
            volatile int z2 = 0;
            fn_02382CB4(z2, *(unsigned char **)0x0380FFF4 + 1612, 44);
        }
        w = fn_023963F0();
        *(unsigned short *)(*(unsigned char **)0x0380FFF4 + 1628) = w;
        fn_023969A4(*(unsigned char **)0x0380FFF4 + 1642, *(unsigned char **)0x0380FFF4 + 744, *(unsigned char **)0x0380FFF4 + 864);
        *(unsigned short *)0x048080AE = 13;
        break;
    }
    case 3: {
        unsigned short v1 = 0xB6B8;
        unsigned short v2 = 0x1D46;
        unsigned short w;
        *(unsigned int *)(*(unsigned char **)0x0380FFF4 + 1004) = 0x04804170;
        *(unsigned int *)(*(unsigned char **)0x0380FFF4 + 1024) = 0x04804028;
        *(unsigned int *)(*(unsigned char **)0x0380FFF4 + 1044) = 0x04804000;
        *(void **)(*(unsigned char **)0x0380FFF4 + 1012) = (void *)fn_023A0398;
        *(void **)(*(unsigned char **)0x0380FFF4 + 1032) = (void *)fn_0239FED0;
        *(void **)(*(unsigned char **)0x0380FFF4 + 1052) = (void *)0x027D1DC4;
        *(unsigned short *)0x04804024 = v1;
        *(unsigned short *)0x04804026 = v2;
        *(unsigned short *)0x0480416C = v1;
        *(unsigned short *)0x0480416E = v2;
        *(unsigned short *)0x04804790 = v1;
        *(unsigned short *)0x04804792 = v2;
        if (*(unsigned short *)(*(unsigned char **)0x0380FFF4 + 760) == 0)
            *(unsigned short *)(*(unsigned char **)0x0380FFF4 + 902) = 264;
        else
            *(unsigned short *)(*(unsigned char **)0x0380FFF4 + 902) = 0x4108;
        {
            volatile int z3 = 0;
            fn_02382CB4(z3, *(unsigned char **)0x0380FFF4 + 1612, 44);
        }
        w = fn_023963F0();
        *(unsigned short *)(*(unsigned char **)0x0380FFF4 + 1628) = w;
        fn_023969A4(*(unsigned char **)0x0380FFF4 + 1642, *(unsigned char **)0x0380FFF4 + 744, *(unsigned char **)0x0380FFF4 + 864);
        *(unsigned short *)0x048080AE = 13;
        break;
    }
    default:
        break;
    }
}
