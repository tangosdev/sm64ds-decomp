extern short data_02082214[];
extern int data_ov002_02100150[];
extern int data_ov002_02100160[];

void func_ov002_020f5328(char *c, int i)
{
    int off = i * 0x4c;

    if (*(unsigned char *)(c + 0x48 + off) == 0) {
        int idx;
        {
            idx = *(unsigned short *)(c + 0x2e + off) >> 4;
            short tv = data_02082214[idx * 2 + 1];
            int spd = *(int *)(c + 0x10 + off);
            *(int *)(c + 0x00 + off) = (int)(((long long)tv * spd + 0x800) >> 12) + 0x80000;
        }
        {
            idx = *(unsigned short *)(c + 0x2e + off) >> 4;
            short tv = data_02082214[idx * 2];
            int spd = *(int *)(c + 0x10 + off);
            *(int *)(c + 0x04 + off) = (int)(((long long)tv * spd + 0x800) >> 12) + 0x60000;
        }

        *(unsigned short *)(c + 0x2e + off) += *(unsigned short *)(c + 0x42 + off);
        *(int *)(c + 0x24 + off) += *(unsigned short *)(c + 0x42 + off);

        if (*(unsigned char *)(c + 0x48) != 0) {
            *(unsigned short *)(c + 0x42 + off) += 8;
        } else if (*(unsigned short *)(c + 0x42 + off) < 0x200) {
            *(unsigned short *)(c + 0x42 + off) += 6;
            if (*(unsigned short *)(c + 0x42 + off) > 0x200)
                *(unsigned short *)(c + 0x42 + off) = 0x200;
        }

        if ((unsigned int)*(int *)(c + 0x24 + off) < (unsigned int)data_ov002_02100150[i])
            return;
        if (*(unsigned char *)(c + 0x514) != 0)
            return;

        *(unsigned char *)(c + 0x48 + off) += 1;
        *(unsigned short *)(c + 0x36 + off) = 0;

        if (i == 1 || i == 3) {
            *(int *)(c + 0x08 + off) = 0x2000;
        } else {
            *(int *)(c + 0x08 + off) = -0x2000;
        }
        return;
    }

    *(unsigned short *)(c + 0x36 + off) += 1;
    *(int *)(c + 0x00 + off) += *(int *)(c + 0x08 + off);
    if (i == 1 || i == 3) {
        *(int *)(c + 0x08 + off) += 0x300;
    } else {
        *(int *)(c + 0x08 + off) -= 0x300;
    }

    if (*(unsigned short *)(c + 0x36) < 0x60)
        return;

    {
        int off2 = (int)((unsigned long long)i) * 0x4c;
        *(unsigned char *)(c + 0x47 + off2) += 1;
        *(unsigned short *)(c + 0x3c + off2) = 0;
        *(unsigned char *)(c + 0x48 + off) = 0;
        *(int *)(c + 0x10 + off2) = 0x38000;
        *(unsigned short *)(c + 0x2e + off2) = 0;
        *(unsigned short *)(c + 0x30 + off2) = (unsigned short)(data_ov002_02100160[i] << 6);
        *(int *)(c + 0x00 + off) = -0x10000;
        *(int *)(c + 0x04 + off2) = 0;
        *(unsigned short *)(c + 0x42 + off2) = 0x100;
        *(int *)(c + 0x08 + off) = 0x5800;
        *(int *)(c + 0x1c + off2) = 0x80000;
        *(int *)(c + 0x20 + off2) = 0x28000;
    }
}
