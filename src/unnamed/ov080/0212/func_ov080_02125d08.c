typedef int Fix12i;
typedef short s16;
extern Fix12i _ZN4cstd4fdivEii(Fix12i a, Fix12i b);

int func_ov080_02125d08(void *self, int frames)
{
    int *obj = *(int **)((char *)self + 0x1ac);
    Fix12i speed = _ZN4cstd4fdivEii(0xffff, obj[2]);
    long long prod = (long long)frames * (long long)speed;
    int result = (int)((prod + 0x800) >> 12);
    s16 cur = *(s16 *)((char *)self + 0x1b4);
    return (s16)(result - cur);
}
