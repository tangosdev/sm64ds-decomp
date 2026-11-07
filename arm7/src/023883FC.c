/* fn_023883FC: report a result word, retrying until accepted. */

extern int fn_02383024(int, int, int);

int fn_023883FC(int a1, int a2)
{
    int v2 = a1 & 0x70;
    int v4;
    int v5;
    int v6;
    int result;

    switch (v2) {
    case 0x00:
    case 0x10:
        v4 = 6;
        break;
    case 0x40:
    case 0x50:
        v4 = 9;
        break;
    case 0x60:
    case 0x70:
        v4 = 8;
        break;
    case 0x20:
    case 0x30:
        v4 = 4;
        break;
    }
    v6 = ((a1 & 0xFF | 0x80) << 8) | 0x3000000 | (a2 & 0xFF);
    do {
        v5 = 0;
        result = fn_02383024(v4, v6, v5);
    } while (result < 0);
    return result;
}
