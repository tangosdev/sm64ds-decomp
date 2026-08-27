//cpp
// @symbol _ZN15dScMgHanachan_c6RenderEv
/* dScMgHanachan_c::Render -- 0x4660 is read here as a raw {int,int} pair
   (the same field Behavior calls through as a pointer-to-member-function;
   see the class header). */
#include "decl_common.h"
#pragma opt_common_subs off
#include "dScMgHanachan_c.h"

extern "C" void func_ov004_020b1e34(void *a, int b, int c, int d);
extern "C" int GetGameLanguage(void);
extern "C" void DrawOamSprite(void *a, int b, int c, int d);
extern "C" void func_ov004_020b2220(int a, int b, void *c, int d, int e, int f, int g);

typedef struct { int a; int b; } Pair;

#define PAIR0(s) ((Pair *)(int)((char *)(s) + 0x4660))
#define PAIR1(s) ((Pair *)(int)((char *)(s) + 0x4660))

extern "C" Pair data_ov006_0213c9ac;
extern "C" Pair data_ov006_0213c994;
extern "C" Pair data_ov006_0213ca3c;
extern "C" Pair data_ov006_0213ca34;
extern "C" Pair data_ov006_0213ca2c;

s32 dScMgHanachan_c::Render()
{
    char *self = (char *)this;
    int i;
    char *arr;

    func_ov004_020b1e34(self, 0xe0, 0x14, 1);
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
            int idx = GetGameLanguage();
            void *e = data_ov006_0213ca9c[idx];
            void *f = *(void **)((char *)e + 0xc);
            DrawOamSprite(f, 0x80, 0x10, 0);
            func_ov004_020b2220(0x80, 0x28, *(void **)(self + 0x4670), 1, -1, 0x800, 0);
        }
    }

    return 1;
}
