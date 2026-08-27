extern void func_0205c528(void *a, void *b, int n);

struct In { int *p; int *q; };
struct Out { int a; unsigned short b; unsigned short c; };
struct M12 { int w[3]; };

int func_0205c378(int *thiz)
{
    char *t = (char *)thiz;
    int *r5 = (int *)thiz[2];        /* [8] */
    unsigned short *src = (unsigned short *)(t + 0x2c);
    struct Out out;
    struct In in;

    in.p = r5;
    in.q = (int *)(r5[10] + ((int)src[2] << 3));  /* [r5+0x28] + ldrh[r4+4]*8 */
    func_0205c528(&in, &out, 8);

    *(struct M12 *)(t + 0x1c) = *(struct M12 *)src;

    if (src[3] == 0 && ((int *)src)[2] == 0) {
        *(short *)(t + 0x22) = out.b;
        *(int *)(t + 0x24) = r5[10] + out.a;
    }
    *(int *)(t + 0x28) = out.c & 0xfff;
    return 0;
}
