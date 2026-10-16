#pragma scheduling off
struct Out {
    unsigned short w0;
    unsigned short w2;
    unsigned short w4;
    unsigned short w6;
    unsigned short w8;
    unsigned char bA;
    unsigned char bB;
    unsigned short wC;
    unsigned short wE;
};

extern int fn_02382DC0(int a1, int a2, int a3);

int fn_0238E448(struct Out *a1, unsigned short *a2) {
    int f2;
    int f1;
    int f4;
    int f8;
    unsigned short result;
    a1->w4 = a2[3];
    a1->w6 = a2[4];
    a1->w8 = a2[5];
    f2 = a2[8] != 0 ? 2 : 0;
    f1 = a2[6] != 0 ? 1 : 0;
    f4 = a2[9] != 0 ? 4 : 0;
    f8 = a2[10] != 0 ? 8 : 0;
    a1->bB = (unsigned char)(f1 | f2 | f4 | f8);
    a1->bA = (unsigned char)a2[2];
    a1->w0 = 1;
    a1->wC = a2[25];
    result = a2[26];
    a1->wE = result;
    if (a1->bA != 0)
        return fn_02382DC0(*(int *)a2, (int)((char *)a1 + 16), (a1->bA + 1) & ~1);
    return result;
}
