#pragma opt_strength_reduction off
#pragma opt_common_subs off

extern void func_ov004_020afdd0(void* a0, int a1, int a2, int a3, int a4);

extern void *data_ov006_0213a458[];
extern void *data_ov006_0213a390[];
extern void *data_ov006_0213a32c;
extern void *data_ov006_0213a35c;
extern int data_ov006_0213b84c[13];

typedef struct { int a[13]; } T13;

void func_ov006_020d452c(void *thiz)
{
    unsigned char *c = (unsigned char *)thiz;
    int fix = -1;
    int i;
    int y;

    i = 0;
    y = 0x20;
    for (; i < 4; y += 0x40, i++) {
        int *p = (int *)(c + i * 4);
        if (*(int *)((char *)p + 0x4714) != 0) {
            int *t = (int *)(((int)p + 0x539c));
            (*t)++;
            if (*t >= 6) {
                int *u = (int *)(((int)p + 0x53ac));
                *t = 0;
                (*u)++;
                if (*u >= 14)
                    *u = 0;
            }
            func_ov004_020afdd0(data_ov006_0213a458[*(int *)(c + i * 4 + 0x53ac)],
                                *(int *)(c + i * 8 + 0x4724) >> 12,
                                (*(int *)(c + i * 8 + 0x4728) >> 12) - 4,
                                fix, 0);
        } else {
            int *cnt;
            int *cnt2;
            int tbl[13];
            if (*(c + i + 0x5398) == 0) {
                cnt = (int *)(((int)p + 0x5378));
                cnt2 = (int *)(((int)p + 0x5388));
                (*cnt)++;
                if (*cnt2 >= 12) {
                    if (*cnt >= 6) {
                        *cnt = 0;
                        *cnt2 = 0;
                    }
                } else if (*cnt >= 4) {
                    *cnt = 0;
                    (*cnt2)++;
                }
            } else {
                cnt = (int *)(((int)p + 0x5378));
                cnt2 = (int *)(((int)p + 0x5388));
                (*cnt)++;
                if (*cnt2 >= 12) {
                    if (*cnt >= 5) {
                        *cnt = 0;
                        *cnt2 = 0;
                    }
                } else if (*cnt >= 2) {
                    *cnt = 0;
                    (*cnt2)++;
                }
            }
            *(T13 *)tbl = *(T13 *)data_ov006_0213b84c;
            func_ov004_020afdd0(data_ov006_0213a390[tbl[*cnt2]],
                                y, 0xb8, fix, 0);
        }
    }
    {
        int j;
        for (j = 0; j < *(int *)(c + 0x46c8); j++) {
            func_ov004_020afdd0(data_ov006_0213a32c,
                                *(int *)(c + j * 8 + 0x4660),
                                *(int *)(c + j * 8 + 0x4664),
                                -1, 0);
        }
    }
    func_ov004_020afdd0(data_ov006_0213a35c, 8, 0x60, -1, 0);
    func_ov004_020afdd0(data_ov006_0213a35c, 0xf8, 0x60, -1, 0);
}
