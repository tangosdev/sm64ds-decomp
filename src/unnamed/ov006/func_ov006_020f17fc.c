extern int RandomIntInternal(int *seed);
extern void func_ov006_020f1dbc(void *self, int idx);
extern int data_0209d4b8;
extern int data_ov006_0212e878[];
extern short data_02082214[];

void func_ov006_020f17fc(char *c, int idx)
{
    unsigned char *counterA = (unsigned char *)(c + 0x5275);

    if (counterA[idx] == 0) {
        unsigned int r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
        unsigned short val = (unsigned short)(((r << 4) >> 15) << 12);
        *(unsigned short *)(c + idx * 2 + 0x4f7c) = val;
        counterA[idx] += 1;
        return;
    }

    {
        unsigned char *counterC = (unsigned char *)(c + 0x5365);
        int *accB = (int *)(c + 0x47f8);
        int *accD = (int *)(c + 0x49d8);
        unsigned short valB;
        int shifted;
        int tblOdd, tblEven;
        int weight;
        long long prod;
        int add12 = 0x800;

        valB = *(unsigned short *)(c + idx * 2 + 0x4f7c);
        shifted = valB >> 4;
        tblOdd = data_02082214[shifted * 2 + 1];
        weight = data_ov006_0212e878[counterC[idx]];
        prod = (long long)tblOdd * weight;
        accB[idx] += (int)((prod + add12) >> 12);

        valB = *(unsigned short *)(c + idx * 2 + 0x4f7c);
        shifted = valB >> 4;
        tblEven = data_02082214[shifted * 2];
        weight = data_ov006_0212e878[counterC[idx]];
        prod = (long long)tblEven * weight;
        accD[idx] += (int)((prod + add12) >> 12);
    }

    func_ov006_020f1dbc(c, idx);
}
