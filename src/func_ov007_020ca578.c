typedef long long s64;

extern void func_ov007_020ca578(char* self, int i, int j, int t);

void func_ov007_020ca578(char* self, int i, int j, int t)
{
    char* lr;
    int lo, hi;
    int val;

    {
        char* a = *(char**)(self + 4);
        char* b = *(char**)(a + 8);
        char* c = *(char**)(b + j * 4);
        char* d = *(char**)c;
        lr = *(char**)(d + i * 4);
    }

    if (*(int*)(lr + 0x20) & 1) return;

    hi = *(int*)(self + 0x1c);
    lo = *(int*)(self + 0x18);

    if (t >= 0x1000) {
        val = hi;
    } else if (t <= 0) {
        val = lo;
    } else {
        val = (int)(((s64)(0x1000 - t) * lo + (s64)t * hi) >> 0xc);
    }

    *(int*)(((int)lr + 0x18) & 0xFFFFFFFFFFFFFFFFLL) += val;
}
