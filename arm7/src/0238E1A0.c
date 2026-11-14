/* storage 0x0238E1A0 runtime 0x027C0118 linked 0x12C main ARM. */

extern long long fn_0238D594(int a, int b);

int fn_0238E1A0(unsigned int a1) {
    int two;
    int j;
    int i;
    int mid;
    int k;
    int mask;
    int span;
    int one;
    long long quot;

    mask = (unsigned short)(a1 & 0x1FFF);
    if (mask == 0)
        return 0;
    i = 0;
    while (i < 16) {
        if ((mask & (1 << i)) != 0)
            break;
        i++;
    }
    j = 15;
    while (j != 0) {
        if ((mask & (1 << j)) != 0)
            break;
        j--;
    }
    span = j - i;
    if (span < 5)
        return (unsigned short)(1 << i);
    mid = (int)fn_0238D594(j + i, 2);
    k = 0;
    two = 2;
    one = 1;
    while (k < span) {
        quot = fn_0238D594(k, two);
        mid += k * (((int)(quot >> 32) << 1) - 1);
        if ((mask & (one << mid)) != 0)
            break;
        k++;
    }
    if (j - mid < 5)
        goto ends;
    if (mid - i >= 5)
        goto middle;
ends:
    return (unsigned short)((1 << i) | (1 << j));
middle:
    return (unsigned short)((1 << j) | (1 << mid) | (1 << i));
}
