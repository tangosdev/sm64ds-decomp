/* func_ov080_02125bb0 at 0x02125bb0
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov080).
 */
typedef int Fix12i;

extern int func_ov080_02125d08(void *self, int frames);
extern int func_ov080_02125cd4(void *c, int n);
extern Fix12i _ZN4cstd4fdivEii(Fix12i a, Fix12i b);
extern int __aeabi_idiv(int a, int b);

extern short data_02082214[];

int func_ov080_02125bb0(void *self, int arg1)
{
    char *r7 = (char *)self;
    int r6 = arg1;
    unsigned short r4 = *(unsigned short *)(r7 + 0x1b6);
    int *obj;
    int r5res, r4res, sum, idx;
    Fix12i f;
    short tv;
    long long prod;

    if (r4 == 0)
        return 0;

    if (((unsigned char)((*(unsigned int *)(r7 + 8) >> 13) & 3)) == 0) {
        obj = *(int **)(r7 + 0x1ac);
        {
            int a = __aeabi_idiv(0xffff, obj[1]);
            int b = __aeabi_idiv(obj[2], a);
            if (r6 > b * (obj[4] - r4))
                return 0;
        }
    }

    r5res = func_ov080_02125d08(self, r6);
    r4res = func_ov080_02125cd4(self, *(int *)((char *)*(int **)(r7 + 0x1ac) + 0x10) - *(unsigned short *)(r7 + 0x1b6));

    f = _ZN4cstd4fdivEii(r4res, *(int *)((char *)*(int **)(r7 + 0x1ac) + 0xc));
    prod = (long long)(-f) * (long long)r6;
    sum = r4res + (int)((prod + 0x800) >> 12);
    if (sum < 0)
        sum = 0;

    idx = (unsigned short)r5res >> 4;
    tv = *(short *)((char *)data_02082214 + (idx << 2));
    prod = (long long)tv * (long long)sum;
    return (int)((prod + 0x800) >> 12);
}