extern int func_ov007_020c3ba8(int r4);
#pragma opt_propagation off

void func_ov007_020c56bc(char* sl, int sb, int r2, int r3, int p4, int fp)
{
    int i, j;
    int lim, neg3, d1, negp4, d2, jinit, base3e8;
    int* r6;
    int r5;
    int r4;

    i = 0;
    if (*(int*)(sl + 8) <= 0) return;

    r4 = r2 * 10;
    neg3 = -r3;
    negp4 = -p4;
    lim = r4 - 1;
    jinit = 0;
    d1 = r3 - neg3;
    d2 = p4 - negp4;
    base3e8 = 0x3e8;

    do {
        j = jinit;
        if ((*(int***)(sl + 0x3c))[i][1] > 0) {
            r4 = j;
            do {
                int r0v;

                if (func_ov007_020c3ba8(base3e8) <= lim) {
                    r6 = (int*)((*(int***)(sl + 0x34))[i][j]) + 3;
                    r5 = fp * (neg3 + func_ov007_020c3ba8(d1));
                    r0v = fp * (negp4 + func_ov007_020c3ba8(d2));

                    switch (sb) {
                    case 0:
                        r6[0] = r5;
                        r6[1] = r0v;
                        break;
                    case 1:
                        {
                            int* q;
                            r6[0] = r6[0] + r5;
                            q = (int*)(int)(((long long)(int)((char*)r6 + 4)) & 0xFFFFFFFFFFFFFFFFLL);
                            *q = *q + r0v;
                        }
                        break;
                    case 2:
                        r6[0] = r5 + (*(int*)(sl + 0x7c) -
                                      *(int*)((char*)((*(int***)(sl + 0x2c))[i]) + r4));
                        r6[1] = r0v + (*(int*)(sl + 0x80) -
                                       *(int*)((char*)((*(int***)(sl + 0x2c))[i]) + r4 + 4));
                        break;
                    }
                }

                r4 += 0xc;
                j++;
            } while (j < (*(int***)(sl + 0x3c))[i][1]);
        }
        i++;
    } while (i < *(int*)(sl + 8));
}
