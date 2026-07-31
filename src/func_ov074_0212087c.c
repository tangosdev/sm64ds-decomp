typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef long long s64;

#define AT(p,off) ((void*)(int)(((long long)(int)((char*)(p)+(off)))))
#define LI(v) ((int)(((long long)(v))))

extern char* _ZN5Actor13ClosestPlayerEv(void* self);
extern short data_02082214[];

void func_ov074_0212087c(int* out, char* self, int n_)
{
    int n;

    n = LI(n_);

    out[0] = *(int*)(self + 0x5c);
    out[1] = *(int*)(self + 0x60);
    out[2] = *(int*)(self + 0x64);

    if (*(int*)(_ZN5Actor13ClosestPlayerEv(self) + 8) == 3) {
        int rem;
        int quo;
        int scale;
        int q;
        int sum;
        int prod;
        int diff;
        int idx;
        u8 mode;

        rem = n % 3;
        quo = LI(n) / 3;
        mode = *(u8*)(self + 0x604);
        if (mode == 3) {
            scale = 0x546000;
        } else if (mode == 2) {
            scale = quo * 0xb4000 + 0x4ec000;
        } else {
            scale = quo * 0xb4000 + 0x438000;
        }

        q = *(int*)(self + 0x80) / 0x1000;
        sum = (s16)(q * 0x350 + 0x500) + (s16)(rem * 0x600);
        prod = *(int*)(self + 0x5f0) * sum;
        diff = (*(s16*)(self + 0x5f4) + *(s16*)(self + 0x5f8)) - prod;
        idx = ((u16)(s16)diff >> 4) * 2;

        out[0] = (int)(((s64)scale * data_02082214[idx] + 0x800) >> 12);
        *(int*)AT(out, 4) += 0x32000;
        out[2] = (int)(((s64)scale * data_02082214[idx + 1] + 0x800) >> 12);
    } else {
        int lim;
        int one;
        int sign;
        int rem2;
        int scale2;
        int q2;
        int sum2;
        int prod2;
        int prod3;
        int diff2;
        int idx2;

        lim = 3;
        if (*(u8*)(self + 0x604) == 1)
            lim = 5;

        if (n < lim) {
            one = 1;
            sign = -1;
            rem2 = n % lim;
            scale2 = 0x4e2000 + (rem2 * 0x64 << 12);
        } else {
            one = 1;
            sign = one;
            rem2 = n % lim;
            scale2 = 0x4e2000 + (rem2 * 0x64 << 12);
        }

        q2 = *(int*)(self + 0x80) / 0x1000;
        sum2 = (s16)(q2 * 0x350 + 0x250) + (s16)(one * 0x600);
        prod2 = *(int*)(self + 0x5f0) * sum2;
        prod3 = sign * prod2;
        diff2 = (*(s16*)(self + 0x5f4) + *(s16*)(self + 0x5f6)) - prod3;
        idx2 = ((u16)(s16)diff2 >> 4) * 2;

        out[0] = (int)(((s64)scale2 * data_02082214[idx2] + 0x800) >> 12);
        *(int*)AT(out, 4) += 0x32000;
        out[2] = (int)(((s64)scale2 * data_02082214[idx2 + 1] + 0x800) >> 12);
    }
}
