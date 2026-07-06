struct Vec3 { int x, y, z; };
extern void AddVec3(struct Vec3 *a, struct Vec3 *b, struct Vec3 *c);
extern void func_ov007_020c7560(void *self, int a, int b);
extern void func_ov007_020c7b2c(int a, int b, int c);
extern void func_ov007_020c81a0(void *node, int a, int b);
extern void func_ov007_020c831c(void *s, int a, int b);

#define DIVR(x) ((int)(((long long)(x) * -1000 + 0x800) >> 12))

void func_ov007_020c5188(char *sb, int arg1)
{
    int p;
    int oc;
    int count;
    int stride;
    int fp;
    int r8;
    int start_fp;
    int start_r8;
    int v18;
    int v1c;
    int v20;
    struct Vec3 backup;
    int *r7;
    int *r6;

    p = arg1;
    stride = *(int *)(*(char **)(sb + 0x1c) + 4);
    count = 1 << p;
    backup = *(struct Vec3 *)(sb + 0x70);
    r7 = (int *)(sb + 0xb8);
    r6 = (int *)(sb + 0xc4);
    AddVec3((struct Vec3 *)(sb + 0x88), (struct Vec3 *)(sb + 0x70), (struct Vec3 *)(sb + 0x70));

    oc = 0;
    if (count > 0) {
        start_fp = 0;
        start_r8 = 0;
        v18 = 0;
        v1c = 0x7d0;
        v20 = 0;
        do {
            fp = start_fp;
            if (*(int *)(sb + 8) > 0) {
                do {
                    int *r5 = ((int **)*(void **)(sb + 0x3c))[fp];
                    func_ov007_020c7560(r5, ((int *)*(void **)(sb + 0x18))[0], ((int *)*(void **)(sb + 0x18))[1]);
                    if (fp >= 1) {
                        func_ov007_020c7b2c((int)((void **)*(void **)(sb + 0x40))[fp], ((int *)*(void **)(sb + 0x18))[4], ((int *)*(void **)(sb + 0x18))[5]);
                        func_ov007_020c7b2c((int)((void **)*(void **)(sb + 0x44))[fp], ((int *)*(void **)(sb + 0x18))[4], ((int *)*(void **)(sb + 0x18))[5]);
                    }
                    r8 = start_r8;
                    if (r5[1] > 0) {
                        do {
                            int *node = ((int **)r5[0])[r8];
                            func_ov007_020c7b2c(((int *)(((void **)*(void **)(sb + 0x34))[fp]))[r8],
                                                ((int *)*(void **)(sb + 0x18))[2],
                                                ((int *)*(void **)(sb + 0x18))[3]);
                            func_ov007_020c81a0(node, stride, p);
                            {
                                int *pt = (int *)node[0];
                                if (r7[0] > pt[0]) { pt[0] = r7[0]; node[1] = DIVR(node[1]); }
                                else if (r6[0] < pt[0]) { pt[0] = r6[0]; node[1] = DIVR(node[1]); }
                            }
                            {
                                int *pt = (int *)node[0];
                                if (r7[1] > pt[1]) { pt[1] = r7[1]; node[2] = DIVR(node[2]); }
                                else if (r6[1] < pt[1]) { pt[1] = r6[1]; node[2] = DIVR(node[2]); }
                            }
                            r8++;
                        } while (r8 < r5[1]);
                    }
                    fp++;
                } while (fp < *(int *)(sb + 8));
            }
            if (*(int *)(sb + 4) == 2) {
                (*(int **)(sb + 0x6c))[6] = v18;
                (*(int **)(sb + 0x6c))[5] = (*(int **)(sb + 0x6c))[6];
                (*(int **)(sb + 0x6c))[4] = (*(int **)(sb + 0x6c))[5];
                func_ov007_020c7b2c(*(int *)(sb + 0xa4), 0xbb8, v1c);
                func_ov007_020c831c(*(void **)(sb + 0x6c), v20, p);
            }
            oc++;
        } while (oc < count);
    }

    *(struct Vec3 *)(sb + 0x70) = backup;
}
