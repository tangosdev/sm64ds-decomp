typedef short s16;
typedef unsigned char u8;

int func_ov006_020da4ac(char *self, s16 *out)
{
    s16 counts[6] = {0};
    int i;
    int j;
    int result = 0;
    int maxVal = 0;
    int secVal = 0;
    s16 maxIdx = 6;
    s16 secIdx = maxIdx;

    for (i = 0; i < 5; i++) {
        counts[*(u8 *)(self + 0x2a)]++;
        self += 0x30;
    }

    for (j = 0; j < 6; j++) {
        s16 v = counts[j];
        if (maxVal <= v) {
            secVal = maxVal;
            secIdx = maxIdx;
            maxVal = v;
            maxIdx = j;
        } else if (secVal <= v) {
            secVal = v;
            secIdx = j;
        }
    }

    if (maxVal == secVal && maxIdx < secIdx) {
        s16 t = maxIdx;
        maxIdx = secIdx;
        secIdx = t;
    }

    switch (maxVal) {
    case 2:
        result = (secVal == 2) ? 2 : 1;
        break;
    case 3:
        result = (secVal == 2) ? 4 : 3;
        break;
    case 4:
        result = 5;
        break;
    case 5:
        result = 6;
        break;
    default:
        break;
    }

    if (out != 0) {
        if (maxVal > 1)
            out[0] = (s16)maxIdx;
        if (secVal > 1)
            out[1] = (s16)secIdx;
    }
    return result;
}
