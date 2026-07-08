typedef unsigned char u8;

#define E_W0  (*(int*)((char*)self + 0x4bd0 + off))
#define E_W8  (*(int*)((char*)self + 0x4bd8 + off))
#define E_HC  (*(unsigned short*)((char*)self + 0x4bdc + off))
#define E_B12 (*(u8*)((char*)self + 0x4be2 + off))
#define E_B13 (*(u8*)((char*)self + 0x4be3 + off))
#define FLAG  (*(u8*)((char*)self + 0x4c1f))

void func_ov006_0211cd24(void* self, int idx) {
    int off = idx * 0x1c;

    if ((E_W0 >> 12) == 0xc0) {
        E_HC++;
        if (FLAG != 0) {
            if (E_B13 >= 3) return;
            if (E_HC < 4) return;
            E_HC = 0;
            E_B13++;
        } else {
            if (E_B13 >= 6) return;
            if (E_HC < 8) return;
            E_HC = 0;
            E_B13++;
        }
    } else {
        E_B12 = 1;
        E_W0 += E_W8;
        E_W8 -= 0x200;
        if ((E_W0 >> 12) > 0xc0) return;
        E_W0 = 0xc0000;
        E_W8 = 0;
        E_HC = 0;
        if (FLAG != 0) E_B13 = 0;
    }
}
