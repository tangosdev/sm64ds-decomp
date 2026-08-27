#include "types.h"
typedef struct V2 { int x, y; } V2;

typedef struct Thing
{
    int a;              /* 0x00 */
    int b;              /* 0x04 */
    char pad0[0x10];    /* 0x08 */
    V2 pos;             /* 0x18 */
    char pad1[0x78];    /* 0x20 */
} Thing;                /* 0x98 */

extern u8 data_020a0e40;
extern u8 data_020a0de8[];
extern u8 data_020a0de9[];
extern u8 data_020a0dea[];
extern u8 data_020a0deb[];
extern int data_ov006_0213ca4c[];
extern int data_ov006_0213ca74[];
extern int data_ov006_0213c958;
extern s16 data_ov006_02141fd8;

extern int func_ov006_020eb7b0(Thing *p);
extern int func_ov006_020eb768(Thing *p, V2 *v);
extern void func_02012790(int a);
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int a);
extern void func_ov006_020ea5f0(int x, int y);
extern void func_ov006_020ed40c(char *c);
extern int func_0203d5dc(int *a, V2 *b);
extern void func_ov006_020eb9dc(Thing *p, int d);
extern void func_ov006_020ea81c(int x, int y);
extern int _Z14ApproachLinearRiii(int *r, int a, int b);

void func_ov006_020ed494(char *c)
{
    V2 a;
    int vec[4];
    int b = 0;
    int idx = data_020a0e40;

    if (data_020a0de8[idx * 4] != 0 && data_020a0de9[idx * 4] != 0)
        b = 1;

    if (b != 0)
    {
        Thing *p;
        V2 *src;
        a.x = data_020a0dea[(unsigned int)idx * 4] << 12;
        a.y = data_020a0deb[(unsigned int)idx * 4] << 12;
        p = *(Thing **)(c + 0x4f60);
        src = (V2 *)(int)((char *)p + 0x18);
        vec[0] = src->x;
        vec[1] = src->y;

        if (func_ov006_020eb7b0(*(Thing **)(c + 0x4f60)) != 0 &&
            func_ov006_020eb768(*(Thing **)(c + 0x4f60), &a) != 0)
        {
            *(u8 *)(c + 0x4f64) = 1;
            func_02012790(0x26);
            p = *(Thing **)(c + 0x4f60);
            {
            int *g = data_ov006_0213ca4c;
            if (p->a == g[0] &&
                (p->b == g[1] || p->a == 0))
                _ZN5Sound12PlayBank2_2DEj(0x1ef);
            else
                _ZN5Sound12PlayBank2_2DEj(0x1ee);
            if (data_ov006_02141fd8 == 0)
                vec[0] -= 0x28000;
            func_ov006_020ea5f0(vec[0], vec[1]);
            func_ov006_020ed40c(c);
            return;
            }
        }

        {
            int found = -1;
            int i = 0;
            Thing *ptr;

            if (data_ov006_0213c958 > 0)
            {
                ptr = (Thing *)(c + 0x4678);
                do
                {
                    if (func_ov006_020eb7b0(ptr) != 0 &&
                        func_ov006_020eb768(ptr, &a) != 0)
                    {
                        found = i;
                        break;
                    }
                    i++;
                    ptr++;
                }
                while (i < data_ov006_0213c958);
            }

            if (found != -1)
            {
                int j;
                Thing *ptr2;
                Thing *sel = (Thing *)(c + 0x4678) + found;
                {
                int *g2 = data_ov006_0213ca74;
                if (sel->a == g2[0] &&
                    (sel->b == g2[1] || sel->a == 0))
                    _ZN5Sound12PlayBank2_2DEj(0x1eb);
                else
                    _ZN5Sound12PlayBank2_2DEj(0x1ed);
                }

                {
                    j = 0;
                    if (data_ov006_0213c958 > 0)
                    {
                        ptr2 = (Thing *)(c + 0x4678);
                        do
                        {
                            V2 *s2 = (V2 *)(int)((char *)ptr2 + 0x18);
                            vec[0] = s2->x;
                            vec[1] = s2->y;
                            func_ov006_020eb9dc(ptr2, func_0203d5dc(vec, &a));
                            j++;
                            ptr2++;
                        }
                        while (j < data_ov006_0213c958);
                    }
                }

                func_02012790(0xe);
                {
                    V2 *s3 = (V2 *)(int)((char *)sel + 0x18);
                    vec[2] = s3->x;
                    vec[3] = s3->y;
                }
                if (data_ov006_02141fd8 == 0)
                    vec[2] -= 0x28000;
                func_ov006_020ea81c(vec[2], vec[3]);

                {
                    int t = *(int *)(c + 0x4670) - 10;
                    if (t < 0)
                        t = 0;
                    *(int *)(c + 0x4670) = t;
                }
            }
        }
    }

    if (_Z14ApproachLinearRiii((int *)(c + 0x4674), 0, 1) == 0)
        return;

    *(int *)(c + 0x4674) = 0x3c;
    if (_Z14ApproachLinearRiii((int *)(c + 0x4670), 0, 1) != 0)
    {
        func_02012790(0xe);
        func_ov006_020ed40c(c);
    }

    if (*(int *)(c + 0x4670) > 2)
        _ZN5Sound12PlayBank2_2DEj(0xa7);
    else
        _ZN5Sound12PlayBank2_2DEj(0xa6);
}
