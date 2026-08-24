void func_ov006_020d99ec(char *card, int slot);

int func_ov006_020da174(char *hand);
int func_ov006_020da174(char *hand)
{
    short counts[6];
    short *cp = (short *)(((int)counts));
    int maxVal;
    int secVal;
    int maxIdx;
    int secIdx;
    short i;
    char *card;
    int face;
    int k;

    cp[0] = 0;
    cp[1] = 0;
    cp[2] = 0;
    cp[3] = 0;
    cp[4] = 0;
    cp[5] = 0;
    maxVal = 0;
    secVal = 0;
    maxIdx = 6;
    secIdx = 6;
    for (i = 0, card = hand; i < 5; i++) {
        counts[*(unsigned char *)(card + 0x2a)]++;
        card += 0x30;
    }
    for (i = 0; i < 6; i++) {
        if (maxVal <= counts[i]) {
            secVal = maxVal;
            secIdx = maxIdx;
            maxVal = counts[i];
            maxIdx = i;
        } else if (secVal <= counts[i]) {
            secVal = counts[i];
            secIdx = i;
        }
    }
    if (maxVal == secVal && maxVal > 1 && secVal > 1) {
        int m;
        card = hand;
        for (m = 0; m < 5; m++) {
            face = *(unsigned char *)(card + 0x2a);
            if (maxIdx == face)
                break;
            if (secIdx == face) {
                face = secIdx;
                secIdx = maxIdx;
                maxIdx = face;
                break;
            }
            card += 0x30;
        }
    }
    {
        int order[5] = {0, 1, 2, 3, 4};
        int n;
        int j;
        for (n = 4; n > 0; n--) {
            for (j = 0; j < n; j++) {
                int keyA, keyB, a, b;
                a = order[j];
                keyA = *(unsigned char *)(hand + a * 0x30 + 0x2a);
                b = order[j + 1];
                keyB = *(unsigned char *)(hand + b * 0x30 + 0x2a);

                if (keyA == maxIdx && maxVal > 1)
                    keyA = -2;
                else if (keyA == secIdx && secVal > 1)
                    keyA = -1;
                else
                    keyA = 0;
                if (keyB == maxIdx && maxVal > 1)
                    keyB = -2;
                else if (keyB == secIdx && secVal > 1)
                    keyB = -1;
                else
                    keyB = 0;
                if (keyA > keyB) {
                    order[j] = b;
                    order[j + 1] = a;
                }
            }
        }
        card = hand;
        for (k = 0; k < 5; k++) {
            face = *(unsigned char *)(card + 0x2a);
            if ((maxIdx == face && maxVal > 1) || (secIdx == face && secVal > 1))
                *(unsigned char *)(card + 0x2d) = 1;
            else
                *(unsigned char *)(card + 0x2d) = 0;
            card += 0x30;
        }
        for (k = 0; k < 5; k++)
            func_ov006_020d99ec(hand + order[k] * 0x30, k);
        for (k = 0; k < 5; k++) {
            if (k != order[k])
                return 1;
        }
    }
    return 0;
}
