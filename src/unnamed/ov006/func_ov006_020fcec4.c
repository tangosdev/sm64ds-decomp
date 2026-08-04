extern int Sound_PlayIfNotActive(int a, int b, int c, int d);
extern short data_02082214[];

void func_ov006_020fcec4(char *c, int i)
{
    int n = i * 0x38;
    int x;
    int y;
    int f;
    short sv;

    *(int *)(c + 0x4660 + n) += *(int *)(c + 0x4668 + n);
    *(int *)(c + 0x4664 + n) += *(int *)(c + 0x466c + n);
    *(unsigned short *)(c + 0x4684 + n) += 0x800;
    if (*(int *)(c + 0x4678 + n) <= 0x30000) {
        *(int *)(c + 0x4678 + n) += 0x800;
    }
    sv = data_02082214[(*(unsigned short *)(c + 0x4684 + n) >> 4) * 2 + 1];
    *(int *)(c + 0x4670 + n) = (int)(((long long)sv * *(int *)(c + 0x4678 + n) + 0x800) >> 12);
    sv = data_02082214[(*(unsigned short *)(c + 0x4684 + n) >> 4) * 2];
    *(int *)(c + 0x4674 + n) = (int)(((long long)sv * *(int *)(c + 0x4678 + n) + 0x800) >> 12);

    x = (*(int *)(c + 0x4660 + n) + *(int *)(c + 0x4670 + n)) >> 12;
    y = (*(int *)(c + 0x4664 + n) + *(int *)(c + 0x4674 + n)) >> 12;
    *(int *)(c + 0x467c + n) = Sound_PlayIfNotActive(*(int *)(c + 0x467c + n), 2, 0x187, 0);
    f = 0;
    if (x >= 0x130 || x <= -0x30) {
        f++;
    }
    if (y >= 0xf0 || y <= -0x110) {
        f++;
    }
    if (f != 0) {
        *(unsigned char *)(c + 0x468c + n) = 0;
        *(unsigned char *)(c + 0x468d + n) = 0;
    }
}
