/* storage 0x0239D11C runtime 0x027CF094 size 0x288 main. */

extern unsigned char *off_380FFF4; /* =0x0380FFF4 */
extern int fn_023A43D8(int); /* =0x027D6350 */
extern void fn_023970C8(int, int); /* =0x027C9040 */
extern int fn_02395C74(int); /* =0x027C7BEC */
extern int fn_02395BE0(int, int); /* =0x027C7B58 */
extern void fn_0239CFB8(unsigned short *); /* =0x027CEF30 */
extern void fn_02395640(void); /* =0x027C75B8 */

int fn_0239D11C(unsigned short *a1, int a2) {
    volatile unsigned short *pspin;
    int result = 1;
    *(unsigned short *)(a2 + 2) = result;
    if ((*(unsigned short *)(off_380FFF4 + 0x300 + 0x10) & 0xF0) == 0x10) {
        if (a1[8] > 1)
            return 5;
        if (a1[10] != 10 && a1[10] != 20)
            return 5;
        if (((1 << a1[11]) & *(unsigned short *)(off_380FFF4 + 0x200 + 0xF0)) == 0)
            return 5;
        if (a1[9] > 4)
            return 5;
        if (fn_023A43D8(0) != 0)
            return 14;
        if (a1[8] != 0) {
            if (a1[8] == 1) {
                if (*(unsigned short *)(off_380FFF4 + 0x300 + 0x10) != 16)
                    return 1;
                switch (a1[9]) {
                case 0:
                case 1:
                case 2:
                    *(unsigned short *)(off_380FFF4 + 0x300 + 0x10) = 17;
                    *(unsigned short *)0x4808040 = 0x8000;
                    fn_023970C8(a1[11], 1);
                    {
                        unsigned short *p316 = (unsigned short *)((int)off_380FFF4 + 0x316);
                        *p316 = *p316 & ~0x10 | (16 * ((((unsigned int)a1[10] << 12) >> 16) & 1));
                    }
                    {
                        int v5 = fn_02395C74(2);
                        if (a1[9] <= 1)
                            v5 |= 0x10;
                        *(unsigned short *)0x48081A4 = a1[10];
                        if (a1[9] == 1) {
                            v5 |= 0x20;
                            *(unsigned short *)0x48081A2 = 3;
                        } else {
                            *(unsigned short *)0x48081A2 = a1[9];
                        }
                        fn_02395BE0(2, v5);
                    }
                    *(unsigned short *)0x48081A0 = 0x823;
                    break;
                case 3:
                case 4:
                    fn_0239CFB8(a1);
                    break;
                }
            }
        } else {
            if (*(unsigned short *)(off_380FFF4 + 0x300 + 0x10) == 17) {
                int v8;
                fn_02395640();
                *(unsigned short *)0x48081A0 = 0;
                *(unsigned short *)0x48081A2 = 1;
                *(unsigned short *)0x4808040 = 0;
                v8 = fn_02395C74(2);
                fn_02395BE0(2, v8 & ~0x30);
            } else if (*(unsigned short *)(off_380FFF4 + 0x300 + 0x10) == 18) {
                *(unsigned short *)(off_380FFF4 + 0x300 + 0x16) &= ~0x20;
                pspin = (volatile unsigned short *)0x4808004;
                while (*pspin != 0)
                    ;
                fn_02395BE0(6, *(unsigned short *)(off_380FFF4 + 0x300 + 0xA8));
            } else {
                return 1;
            }
            *(unsigned short *)(off_380FFF4 + 0x300 + 0x10) = 16;
        }
        return 0;
    }
    return result;
}
