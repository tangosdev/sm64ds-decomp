int func_ov006_020da5e8(char *a, char *b);
int func_ov006_020da5e8(char *a, char *b) {
    short arr1[6];
    short arr2[6];
    short *p1 = (short *)(((int)arr1) & 0xFFFFFFFFFFFFFFFF);
    short *p2 = (short *)(((int)arr2) & 0xFFFFFFFFFFFFFFFF);
    int max1, sec1;
    short max1i, sec1i;
    int max2, sec2;
    short max2i, sec2i;
    int i;
    short tmp;

    p1[0] = 0; p1[1] = 0; p1[2] = 0; p1[3] = 0; p1[4] = 0; p1[5] = 0;
    p2[0] = 0; p2[1] = 0; p2[2] = 0; p2[3] = 0; p2[4] = 0; p2[5] = 0;

    max1 = 0; max1i = 6; sec1 = 0; sec1i = 6;
    max2 = 0; sec2 = 0; max2i = 6; sec2i = 6;

    for (i = 0; i < 5; i++) {
        arr1[*(unsigned char *)(a + 0x2a)]++;
        a += 0x30;
        arr2[*(unsigned char *)(b + 0x2a)]++;
        b += 0x30;
    }

    for (i = 0; i < 6; i++) {
        if (max1 <= arr1[i]) { sec1 = max1; sec1i = max1i; max1 = arr1[i]; max1i = i; }
        else if (sec1 <= arr1[i]) { sec1 = arr1[i]; sec1i = i; }
        if (max2 <= arr2[i]) { sec2 = max2; sec2i = max2i; max2 = arr2[i]; max2i = i; }
        else if (sec2 <= arr2[i]) { sec2 = arr2[i]; sec2i = i; }
    }

    if (max1 == sec1 && max1i < sec1i) { tmp = max1i; max1i = sec1i; sec1i = tmp; }
    if (max2 == sec2 && max2i < sec2i) { tmp = max2i; max2i = sec2i; sec2i = tmp; }

    if (max1 > max2) return 1;
    if (max1 < max2) return -1;
    if (sec1 > sec2) return 1;
    if (sec1 < sec2) return -1;
    if (max1 == 1 && max2 == 1) return 0;
    if (max1i > max2i) return 1;
    if (max1i < max2i) return -1;
    if (sec1 == 1 && sec2 == 1) return 0;
    if (sec1i > sec2i) return 1;
    if (sec1i < sec2i) return -1;
    return 0;
}
