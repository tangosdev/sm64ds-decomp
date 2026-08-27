extern short data_02082214[];
extern int data_ov002_02100120[];
extern int data_ov002_02100130[];
extern int func_ov002_020f5a94(char *c);

void func_ov002_020f40fc(char *c, int i)
{
    int off = i * 0x4c;

    if (*(unsigned short*)(c + 0x3a + off) == 0) {
        if (*(unsigned short*)(c + 0x32 + off) == 0) {
            *(unsigned short*)(c + 0x3c + off) += 1;
        }
    }

    if (*(unsigned char*)(c + 0x48 + off) == 0) {
        int idx;
        idx = *(unsigned short*)(c + 0x2e + off) >> 4;
        {
            short tv = data_02082214[idx * 2 + 1];
            int spd = *(int*)(c + 0x10 + off);
            *(int*)(c + 0 + off) = (int)(((long long)tv * spd + 0x800) >> 12) + 0x80000;
        }
        idx = *(unsigned short*)(c + 0x2e + off) >> 4;
        {
            short tv = data_02082214[idx * 2];
            int spd = *(int*)(c + 0x10 + off);
            *(int*)(c + 4 + off) = (int)(((long long)tv * spd + 0x800) >> 12) + 0x60000;
        }

        *(unsigned short*)(c + 0x2e + off) -= *(unsigned short*)(c + 0x42 + off);
        *(int*)(c + 0x24 + off) += *(unsigned short*)(c + 0x42 + off);

        if (*(unsigned char*)(c + 0x48) != 0) {
            *(unsigned short*)(c + 0x42 + off) += 8;
        } else {
            if (*(unsigned short*)(c + 0x42 + off) < 0x200) {
                *(unsigned short*)(c + 0x42 + off) += 6;
                if (*(unsigned short*)(c + 0x42 + off) > 0x200)
                    *(unsigned short*)(c + 0x42 + off) = 0x200;
            }
        }

        if ((unsigned int)*(int*)(c + 0x24 + off) >= (unsigned int)data_ov002_02100120[i]
            && *(unsigned char*)(c + 0x514) == 0) {
            *(unsigned char*)(c + 0x48 + off) += 1;
            *(unsigned short*)(c + 0x36 + off) = 0;
            *(int*)(c + 8 + off) = 0x2000;
        }

        if (func_ov002_020f5a94(c) != 3)
            return;
        if ((unsigned int)*(int*)(c + 0x24 + off) < 0xc000)
            return;
        if (i != 2)
            return;
        if (*(unsigned char*)(c + 0x514) != 0)
            return;
        *(unsigned char*)(c + 0x48 + off) += 1;
        *(unsigned short*)(c + 0x36 + off) = 0;
        *(int*)(c + 8 + off) = 0x2000;
        return;
    }

    *(unsigned short*)(c + 0x36 + off) += 1;
    *(int*)(c + 0 + off) += *(int*)(c + 8 + off);
    *(int*)(c + 8 + off) += 0x300;
    if (*(unsigned short*)(c + 0x36) < 0x70)
        return;
    {
        int off2 = (int)((long long)i) * 0x4c;
        *(unsigned char*)(c + 0x47 + off2) += 1;
        *(unsigned short*)(c + 0x3c + off2) = 0;
        *(unsigned char*)(c + 0x48 + off) = 0;
        *(int*)(c + 0x10 + off2) = 0x38000;
        *(unsigned short*)(c + 0x2e + off2) = 0;
        *(unsigned short*)(c + 0x30 + off2) = data_ov002_02100130[i] << 6;
        *(int*)(c + 0 + off) = -0x10000;
        *(int*)(c + 4 + off2) = 0;
        *(unsigned short*)(c + 0x42 + off2) = 0x100;
        *(int*)(c + 8 + off) = 0x5800;
        *(int*)(c + 0x1c + off2) = 0x80000;
        *(int*)(c + 0x20 + off2) = 0x28000;
    }
}
