#pragma opt_common_subs off

extern void func_ov004_020b1e34(void *a, int b, int c, int d);
extern void func_ov006_020ea670(void);
extern void func_ov006_020ea350(void);
extern void func_ov006_020eac38(void *a);
extern void func_ov006_020ea914(void *a);
extern int func_ov004_020ad674(void);
extern void func_ov004_020afcf8(void *a, int b, int c, int d);
extern void func_ov004_020b2220(int a, int b, void *c, int d, int e, int f, int g);

typedef struct { int a; int b; } Pair;

#define PAIR0(s) ((Pair *)(int)(((long long)(int)((char *)(s) + 0x4660)) & 0xFFFFFFFFFFFFFFFFLL))
#define PAIR1(s) ((Pair *)(int)(((unsigned long long)(unsigned int)((char *)(s) + 0x4660)) & 0xFFFFFFFFFFFFFFFFULL))

extern Pair data_ov006_0213c9ac;
extern int data_ov006_0213c958;
extern Pair data_ov006_0213c994;
extern Pair data_ov006_0213ca3c;
extern Pair data_ov006_0213ca34;
extern Pair data_ov006_0213ca2c;
extern void *data_ov006_0213ca9c[];

int func_ov006_020ecee4(void *a0)
{
    char *self = (char *)a0;
    int i;
    char *arr;

    func_ov004_020b1e34(a0, 0xe0, 0x14, 1);
    func_ov006_020ea670();
    func_ov006_020ea350();

    {
        Pair *p = PAIR0(self);
        Pair *g = &data_ov006_0213c9ac;
        if (p->a == g->a) {
            if (p->b == g->b || *(int *)(self + 0x4660) == 0) {
                i = 0;
                if (data_ov006_0213c958 > 0) {
                    arr = self + 0x4678;
                    do {
                        func_ov006_020eac38(arr);
                        i++;
                        arr += 0x98;
                    } while (i < data_ov006_0213c958);
                }
                goto tail;
            }
        }
    }

    {
        Pair *p = PAIR1(self);
        Pair *g = &data_ov006_0213c994;
        if (p->a == g->a) {
            if (p->b == g->b || *(int *)(self + 0x4660) == 0) {
                int flag = *(unsigned char *)(self + 0x4f64);
                if (flag != 0) {
                    if (*(int *)(self + 0x466c) < 0x5a) goto do_eac;
                }
                if (flag != 0) goto count_loop;
                if (*(int *)(self + 0x466c) < 0x8a) goto do_eac;
                goto count_loop;
            do_eac:
                func_ov006_020eac38(*(void **)(self + 0x4f60));
                goto tail;
            count_loop:
                i = 0;
                if (data_ov006_0213c958 > 0) {
                    arr = self + 0x4678;
                    do {
                        func_ov006_020eac38(arr);
                        i++;
                        arr += 0x98;
                    } while (i < data_ov006_0213c958);
                }
                goto tail;
            }
        }
    }

    {
        Pair *p = PAIR0(self);
        Pair *g = &data_ov006_0213ca3c;
        if ((p->a == g->a && (p->b == g->b || *(int *)(self + 0x4660) == 0))
            || (p = PAIR0(self), g = &data_ov006_0213ca34, (p->a == g->a && (p->b == g->b || *(int *)(self + 0x4660) == 0)))) {
            func_ov006_020eac38(*(void **)(self + 0x4f60));
        }
    }

tail:
    func_ov006_020ea914(*(void **)(self + 0x4f60));

    {
        Pair *p = PAIR1(self);
        Pair *g = &data_ov006_0213ca2c;
        if (!(p->a == g->a && (p->b == g->b || *(int *)(self + 0x4660) == 0))) {
            int idx = func_ov004_020ad674();
            void *e = data_ov006_0213ca9c[idx];
            void *f = *(void **)((char *)e + 0xc);
            func_ov004_020afcf8(f, 0x80, 0x10, 0);
            func_ov004_020b2220(0x80, 0x28, *(void **)(self + 0x4670), 1, -1, 0x800, 0);
        }
    }

    return 1;
}
