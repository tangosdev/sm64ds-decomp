/* fn_023889BC: handle a received mailbox command word. */

extern unsigned short word_3809424[];

extern int fn_023882B0(int, int, int, ...);
extern int fn_023883FC(int, int);
extern int fn_023886C0(unsigned short, unsigned short);

void fn_023889BC(int a1)
{
    int v0;
    int v1;
    unsigned int v4;
    int v5;
    int v6;
    int v7;
    int v8;

    if (a1 & 0x2000000) {
        v1 = 0;
        do {
            word_3809424[v1] = 0;
            ++v1;
        } while (v1 < 16);
    }
    word_3809424[(unsigned int)(a1 & 0xF0000) >> 16] = (unsigned short)a1;
    if (a1 & 0x1000000) {
        v0 = word_3809424[0];
        v4 = (v0 & 0xFF00) << 8;
        v5 = v4 >> 16;
        switch (v5) {
        case 3:
            fn_023886C0(v0 & 0xFF, word_3809424[1]);
            break;
        case 0:
            v6 = fn_023882B0(0, v5, 0);
            if (v6 == 0)
                fn_023883FC(v5, 4);
            break;
        case 1:
            if (((int *)word_3809424)[8] != 0) {
                fn_023883FC(v5, 3);
            } else {
                unsigned short v10 = word_3809424[0] & 0xFF;
                if (v10 == 0 || v10 > 4) {
                    fn_023883FC(v5, 2);
                } else {
                    if (word_3809424[1] >= 0x107) {
                        fn_023883FC(v5, 2);
                    } else {
                        v7 = fn_023882B0(0, v5, 2, v10, word_3809424[1]);
                        if (v7 != 0)
                            ((int *)word_3809424)[8] = 1;
                        else
                            fn_023883FC(v5, 4);
                    }
                }
            }
            break;
        case 2:
            if (((int *)word_3809424)[8] != 2) {
                fn_023883FC(v5, 3);
            } else {
                v8 = fn_023882B0(0, v5, 0);
                if (v8 != 0)
                    ((int *)word_3809424)[8] = 3;
                else
                    fn_023883FC(v5, 4);
            }
            break;
        default:
            fn_023883FC(v5, 1);
            break;
        }
    }
}
