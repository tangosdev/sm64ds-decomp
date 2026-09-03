#pragma opt_loop_invariants off
extern void _ZN14dScMgMemory2_c11UpdateCardsEv(char *p);
extern void _ZN14dScMgMemory2_c12UpdateCursorEv(char *p);
extern unsigned int func_02012790(unsigned int a);
extern void _ZN14dScMgMemory2_c10HideCursorEv(char *p);

extern unsigned char data_020a0e40[];
extern unsigned char data_020a0de8[];
extern unsigned char data_020a0de9[];
extern int data_ov006_0212e8e8[];
extern int data_ov006_0212e8f4[];

void _ZN14dScMgMemory2_c12ResultFinishEv(char *p)
{
    int i;
    int best;
    int j;
    int n;
    int idx;
    int ok;
    unsigned short *q;

    _ZN14dScMgMemory2_c11UpdateCardsEv(p);
    _ZN14dScMgMemory2_c12UpdateCursorEv(p);

    if (*(unsigned short *)(p + 0x53e2) != 0) {
        q = (unsigned short *)(unsigned int)(p + 0x53e2);
        *q = *q - 1;
        return;
    }

    ok = 0;
    idx = data_020a0e40[0];
    if (data_020a0de8[idx * 4] != 0) {
        if (data_020a0de9[idx * 4] != 0) {
            ok = 1;
        }
    }
    if (ok == 0) {
        return;
    }

    func_02012790(0x62);

    if (*(int *)(p + 0xa8) == 0) {
        for (i = 0; i < data_ov006_0212e8e8[*(unsigned char *)(p + 0x540a)]; i++) {
            best = 0xff;
            j = 0;
            n = data_ov006_0212e8f4[*(unsigned char *)(p + 0x540a)];
            for (; j < data_ov006_0212e8f4[*(unsigned char *)(p + 0x540a)]; j++) {
                if (*(unsigned char *)(p + 0x51bb + (i * n + j) * 0x18) != 0) {
                    best = j;
                    best = best + i * n;
                }
            }
            if (best != 0xff) {
                *(unsigned char *)(p + 0x51bc + best * 0x18) = 6;
            }
        }
        *(unsigned char *)(p + 0x540b) = 0;
    }

    _ZN14dScMgMemory2_c10HideCursorEv(p);
    *(int *)(p + 0x53d4) = 4;
}
