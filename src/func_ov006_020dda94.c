typedef int s32;
typedef unsigned char u8;

extern int data_0209d4b8;
extern int data_ov006_0212e3a0[];
extern int data_ov006_0212e3b8[];
extern int data_ov006_0212e34c[];

extern void func_ov006_020ddcf8(char* c, int idx);
extern int RandomIntInternal(int* seed);
extern void func_020127a4(int r0, int r1, int r2, int r3);

void func_ov006_020dda94(char* self, int i)
{
    int n = i * 0x1c;
    char* pA = self + 0x4660;
    char* fp;
    char* pD = self + 0x466c;
    char* pB = self + 0x4664;
    char* pC = self + 0x4668;

    *(s32*)(pA + n) += *(s32*)(pC + n);
    *(s32*)(pB + n) += *(s32*)(pD + n);
    *(s32*)(pD + n) += 0x400;

    func_ov006_020ddcf8(self, i);

    {
        u8 g = *(u8*)(self + 0x4679 + n);
        int y = *(s32*)(pB + n) >> 12;
        u8 h = *(u8*)(self + 0x467a + n);

        if (g != 0) {
            int rnd, idx, a, v;
            u8 g3;

            fp = (char*)(data_ov006_0212e3a0[h] - data_ov006_0212e3b8[h]);
            if (y < (int)fp) return;

            rnd = RandomIntInternal(&data_0209d4b8);
            idx = ((((unsigned)rnd >> 16) & 0x7fff) * 3) >> 15;
            *(s32*)(pC + n) = data_ov006_0212e34c[idx];
            *(s32*)(pD + n) = -0x2e00;
            *(s32*)(pB + n) = (int)fp << 12;
            *(u8*)(self + 0x4679 + n) -= 1;

            a = (*(s32*)(pA + n) >> 12) - 0x80;
            v = a >> 1;
            if (v >= 0x3c) v = 0x3c;
            if (v <= -0x3c) v = -0x3c;
            func_020127a4(2, 0x197, 0xffff, v);

            g3 = *(u8*)(self + 0x4679 + n);
            if (g3 == 0) {
                *(u8*)(self + 0x467a + n) += 1;
            }
            {
                u8 hh = *(u8*)(self + 0x467a + n);
                u8 e = *(u8*)(self + 0x4674 + n);
                if (hh > e) *(u8*)(self + 0x4675 + n) = 2;
            }
        } else {
            unsigned r;
            *(u8*)(self + 0x4679 + n) = 1;
            r = (unsigned)RandomIntInternal(&data_0209d4b8);
            r = (((r >> 16) & 0x7fff) << 3) >> 15;
            if (r == 2 || r == 5) {
                *(u8*)(self + 0x4679 + n) += 1;
            }
            return;
        }
    }
}
