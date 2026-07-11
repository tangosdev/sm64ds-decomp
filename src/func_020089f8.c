extern void Vec3_Sub(int *dest, const int *src1, const int *src2);

void func_020089f8(int *obj)
{
    int *ptr;
    char *p;
    int tmp[3];

    ptr = *(int **)((char *)obj + 0x110);
    if (ptr == 0) {
        return;
    }
    p = (char *)(((unsigned long long)((char *)ptr + 0x5c)) & 0xFFFFFFFFFFFFFFFFu);
    obj[0x98 / 4] = *(int *)p;
    obj[0x9c / 4] = *(int *)(p + 4);
    obj[0xa0 / 4] = *(int *)(p + 8);
    Vec3_Sub(tmp, (int *)((char *)obj + 0x80), (int *)((char *)obj + 0x98));
    obj[0xa4 / 4] = tmp[0];
    obj[0xa8 / 4] = tmp[1];
    obj[0xac / 4] = tmp[2];
    obj[0x130 / 4] = 0;
}
