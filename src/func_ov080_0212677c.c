typedef unsigned int u32;
typedef unsigned short u16;
typedef short s16;
typedef unsigned char u8;

extern void func_ov080_02125fd0(char *c);
extern void MulMat4x3Mat4x3(const void *a, const void *b, void *out);
extern void func_020553a4(void *m);
extern void func_ov080_02125460(char *c);
extern int data_0209b3ec;

void func_ov080_0212677c(char *self)
{
    int tmp[12];
    int i, j, n;
    int z = 0;

    if (*(u16 *)((self + 0x100) + 0xb6) == 0) {
        func_ov080_02125fd0(self);
        return;
    }

    *(int *)0x4000444 = z;
    MulMat4x3Mat4x3((void *)(self + 0xd4), &data_0209b3ec, tmp);
    *(int *)0x4000440 = 2;
    func_020553a4(tmp);
    *(int *)0x4000440 = 1;
    func_020553a4(tmp);

    *(int *)0x40004c8 = 0xe0000000;
    *(int *)0x40004cc = 0xc0007fff;
    func_ov080_02125460(self);

    *(int *)0x400046c = 0x20000;
    *(int *)0x400046c = 0x20000;
    *(int *)0x400046c = 0x20000;

    n = (int)*(u8 *)(self + 0x1ba);
    i = z; /* mov before sub: use z then sub n */
    n = n - 1;
    if (n > 0) {
        do {
            *(int *)0x4000500 = 2;
            j = z;
            if ((int)*(u8 *)(self + 0x1bb) > 0) {
                do {
                    char *base = *(char **)(self + 0x1a0);
                    int cols = (int)*(u8 *)(self + 0x1bb);
                    char *v1 = base + (i + j * cols) * 0x18;
                    char *v2 = base + ((i + 1) + j * cols) * 0x18;
                    int v0, v1y, v2z;
                    s16 s0, s1, s2;

                    *(int *)0x4000488 = *(int *)(v1 + 0x14);
                    *(int *)0x4000484 = *(int *)(v1 + 0x10);
                    v0 = *(int *)(v1 + 0);
                    v1y = *(int *)(v1 + 4);
                    v2z = *(int *)(v1 + 8);
                    s0 = (s16)(v0 >> 8);
                    s1 = (s16)(v1y >> 8);
                    s2 = (s16)(v2z >> 8);
                    *(int *)0x400048c = (u16)s0 | ((u16)s1 << 16);
                    *(int *)0x400048c = (u16)s2;

                    *(int *)0x4000488 = *(int *)(v2 + 0x14);
                    *(int *)0x4000484 = *(int *)(v2 + 0x10);
                    v0 = *(int *)(v2 + 0);
                    v1y = *(int *)(v2 + 4);
                    v2z = *(int *)(v2 + 8);
                    s0 = (s16)(v0 >> 8);
                    s1 = (s16)(v1y >> 8);
                    s2 = (s16)(v2z >> 8);
                    *(int *)0x400048c = (u16)s0 | ((u16)s1 << 16);
                    *(int *)0x400048c = (u16)s2;

                    j++;
                } while (j < (int)*(u8 *)(self + 0x1bb));
            }
            *(int *)0x4000504 = z;
            i++;
        } while (i < (int)*(u8 *)(self + 0x1ba) - 1);
    }
    *(int *)0x4000448 = 1;
}
