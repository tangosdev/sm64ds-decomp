//cpp
extern "C" {
extern void* data_0209f5bc;
extern int data_0208a170;
extern unsigned char data_020a0e40;
extern unsigned char data_020a0de8[];
extern unsigned char data_020a0de9[];
extern int data_ov005_020c2250[];
void func_ov005_020c16e4(void* sl);
void func_02012790(int idx);

void func_ov005_020c0878(char* sl)
{
    void* g = data_0209f5bc;
    int (**vt)(void*) = *(int(***)(void*))g;
    if (vt[5](g) == 0)
        return;
    if (*(unsigned char*)(sl + 0xac) != 0)
        return;
    if (*(unsigned char*)(sl + 0x54) == 1)
        return;
    if (*(int*)(sl + 0x90) > 0)
        return;
    if (*(int*)(sl + 0x94) > 0)
        return;
    if (*(int*)(sl + 0x98) > 0)
        return;

    if (*(int*)(sl + 0xa0) > 0) {
        int* pa0 = (int*)(((int)sl + 0xa0) & 0xFFFFFFFFFFFFFFFFULL);
        *pa0 = *pa0 - 1;
        if (*(int*)(sl + 0xa0) == 0) {
            int f58 = *(int*)(sl + 0x58);
            if (f58 != data_0208a170) {
                data_0208a170 = f58;
                func_ov005_020c16e4(sl);
                *(int*)(sl + 0xa0) = 0x1e;
            }
        }
    }

    {
        int flag = 0;
        unsigned char idx = data_020a0e40;
        unsigned char v = data_020a0de8[idx * 4];
        if (v != 0) {
            if (data_020a0de9[idx * 4] != 0)
                flag = 1;
        }
        if (flag != 0 || (*(int*)(sl + 0xa0) <= 0 && v != 0)) {
            int i = 0;
            int lo = 2;
            int hi = 0x2e;
            int z = 0;
            int t = 0x1e;
            do {
                int e = data_ov005_020c2250[i];
                if (data_0208a170 != e) {
                    unsigned char ix = data_020a0e40;
                    unsigned char* p = data_020a0de8 + (ix * 4);
                    if (p[2] <= 0x30 && p[3] >= lo && p[3] <= hi) {
                        data_0208a170 = e;
                        *(int*)(sl + 0x58) = e;
                        func_02012790(z);
                        func_ov005_020c16e4(sl);
                        *(int*)(sl + 0xa0) = t;
                    }
                }
                i++;
                lo += 0x30;
                hi += 0x30;
            } while (i < 4);
            return;
        }

        if (*(int*)(sl + 0xa0) <= 0)
            return;
        if (v == 0)
            return;
        {
            int i = 0;
            int lo = 2;
            int hi = 0x2e;
            int z = 0;
            int t = 0x1e;
            do {
                int e = data_ov005_020c2250[i];
                if (*(int*)(sl + 0x58) != e) {
                    unsigned char ix = data_020a0e40;
                    unsigned char* p = data_020a0de8 + (ix * 4);
                    if (p[2] <= 0x30 && p[3] >= lo && p[3] <= hi) {
                        *(int*)(sl + 0x58) = e;
                        func_02012790(z);
                        *(int*)(sl + 0xa0) = t;
                    }
                }
                i++;
                lo += 0x30;
                hi += 0x30;
            } while (i < 4);
        }
    }
}
}
