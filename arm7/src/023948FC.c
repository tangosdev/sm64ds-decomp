#pragma opt_common_subs off
extern unsigned int dword_380FFF4;
extern unsigned int unk_27D6498[];
extern void fn_0239477C(int a1, int a2);

void fn_023948FC(void) {
    unsigned char *base = *(unsigned char **)&dword_380FFF4;
    unsigned int i;
    unsigned int j;
    *(unsigned short *)(base + 16) = 0;
    *(unsigned short *)(base + 18) = 0;
    for (i = 0; i < 4; i++) {
        *(unsigned short *)(base + 2 * i + 8) = 0xFFFF;
        *(unsigned short *)(base + 2 * i) = *(unsigned short *)(base + 2 * i + 8);
    }
    for (j = 0; j < 0x16; j++) {
        *(unsigned short *)(base + 8 * j + 144) = 0xFFFF;
        *(unsigned short *)(base + 8 * j + 146) = 0;
        *(unsigned int *)(base + 8 * j + 148) = unk_27D6498[j];
    }
    fn_0239477C(3, 12);
}
