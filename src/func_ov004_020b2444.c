extern int data_ov006_02137cd8[];
extern void func_ov004_020b1d60(int a0, int a1, int a2, int a3, int a4);

void func_ov004_020b2444(int a1, int a2, int num, int a4, int a5, int sel, int idx)
{
    unsigned t = num;
    int digits = 0;
    int off;
    if (num != 0) {
        do {
            t /= 10;
            digits++;
        } while (t != 0);
    }
    off = 0;
    switch (sel) {
    case 0:
        off = (digits << 3) - 8;
        break;
    case 1:
        break;
    case 2:
        off = (digits << 4) - 0x10;
        break;
    }
    if (num == 0) {
        func_ov004_020b1d60(data_ov006_02137cd8[idx], a1, a2, a4, a5);
        return;
    }
    if (num <= 0)
        return;
    do {
        func_ov004_020b1d60(data_ov006_02137cd8[num % 10 + idx], a1 + off, a2, a4, a5);
        num /= 10;
        off -= 0x10;
    } while (num > 0);
}
