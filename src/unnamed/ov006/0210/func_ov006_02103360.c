extern short data_02082214[];
extern void func_ov006_02102718(char* c);
extern void func_ov006_02102864(char* c);

#pragma opt_common_subs off
void func_ov006_02103360(char *c, int i)
{
    int idx;
    int velX, velY;
    int idx2;

    idx2 = (*(unsigned short*)(c + i * 0x40 + 0x4600 + 0x94) >> 4) * 2;

    if (data_02082214[idx2] <= 0) {
        {
            short tv = data_02082214[idx2 + 1];
            int spd = *(int*)(c + i * 0x40 + 0x4000 + 0x680);
            *(int*)(c + 0x4660 + i * 0x40) += (int)(((long long)tv * spd + 0x800) >> 12);
        }

        idx = *(unsigned short*)(c + i * 0x40 + 0x4600 + 0x94) >> 4;
        {
            short tv = data_02082214[idx * 2];
            int spd = *(int*)(c + i * 0x40 + 0x4000 + 0x680);
            *(int*)(c + 0x4664 + i * 0x40) += (int)(((long long)tv * spd + 0x800) >> 12);
        }

        *(int*)(c + 0x4684 + i * 0x40) += 0x10;

        *(int*)(c + 0x4680 + i * 0x40) -= *(int*)(c + i * 0x40 + 0x4000 + 0x684);

        if (*(int*)(c + i * 0x40 + 0x4000 + 0x684) >= 0x600) {
            *(int*)(c + i * 0x40 + 0x4000 + 0x684) = 0x600;
        }

        if (*(int*)(c + i * 0x40 + 0x4000 + 0x680) < 0) {
            *(unsigned char*)(c + i * 0x40 + 0x4000 + 0x699) = 3;
            *(int*)(c + i * 0x40 + 0x4000 + 0x680) = 0;
            *(int*)(c + i * 0x40 + 0x4000 + 0x684) = 0;
            *(int*)(c + i * 0x40 + 0x4000 + 0x66c) = 0;
            *(int*)(c + i * 0x40 + 0x4000 + 0x668) = 0;
        }
    } else {
        *(int*)(c + 0x4660 + i * 0x40) += *(int*)(c + i * 0x40 + 0x4000 + 0x668);
        *(int*)(c + 0x4664 + i * 0x40) += *(int*)(c + i * 0x40 + 0x4000 + 0x66c);
        *(int*)(c + 0x4684 + i * 0x40) += 0x10;
        *(int*)(c + 0x466c + i * 0x40) += *(int*)(c + i * 0x40 + 0x4000 + 0x684);

        {
            int v = *(int*)(c + i * 0x40 + 0x4000 + 0x668);
            int nv;
            if (v > 0) {
                nv = v - 0x200;
                if (nv <= 0) nv = 0;
            } else if (v < 0) {
                nv = v + 0x200;
                if (nv >= 0) nv = 0;
            } else {
                *(unsigned char*)(c + i * 0x40 + 0x4000 + 0x699) = 3;
                *(int*)(c + i * 0x40 + 0x4000 + 0x680) = 0;
                *(int*)(c + i * 0x40 + 0x4000 + 0x684) = 0;
                *(int*)(c + i * 0x40 + 0x4000 + 0x668) = 0;
                return;
            }
            *(int*)(c + i * 0x40 + 0x4000 + 0x668) = nv;
        }
    }

    {
        int rawX = *(int*)(c + i * 0x40 + 0x4000 + 0x660);
        int rawY = *(int*)(c + i * 0x40 + 0x4000 + 0x664);
        velY = rawY >> 12;
        *(unsigned short*)(c + 0x4690 + i * 0x40) += *(unsigned short*)(c + i * 0x40 + 0x4600 + 0x92);
        velX = rawX >> 12;
    }

    if (velY <= -0x140 || velY >= 0xd0) {
        *(unsigned char*)(c + i * 0x40 + 0x4000 + 0x699) = 6;
    }
    if (velX >= 0x140 || velX <= -0x40) {
        *(unsigned char*)(c + i * 0x40 + 0x4000 + 0x699) = 6;
    }

    func_ov006_02102718(c);
    if (*(unsigned char*)(c + 0x5676) != 0) {
        func_ov006_02102864(c);
    }
}
