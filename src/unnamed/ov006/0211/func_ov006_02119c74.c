extern const short data_02082214[];
extern void* data_ov006_0213f6f0[];
void func_ov004_020b023c(void* a0, int a1, int a2, int a3, void* a4);

void func_ov006_02119c74(char* p)
{
    int i;
    int tb = data_02082214[0];
    long long ta = data_02082214[1];

    for (i = 0; i < 30; i++) {
        if (*(unsigned char*)(p + 0x51cd)) {
            int m[4];
            long long vv = *(int*)(p + 0x51c4);
            int x = *(int*)(p + 0x51b0);
            int y = *(int*)(p + 0x51b4);
            int a = (int)((ta * vv + 0x800) >> 12);
            int b = (int)(((long long)tb * (int)vv + 0x800) >> 12);
            m[0] = a;
            m[3] = a;
            m[1] = b;
            m[2] = -b;
            func_ov004_020b023c(data_ov006_0213f6f0[*(unsigned char*)(p + 0x51ce)],
                                (x >> 12) - 8, (y >> 12) - 0x10, -1, m);
        }
        tb = data_02082214[0];
        p += 0x24;
    }
}
