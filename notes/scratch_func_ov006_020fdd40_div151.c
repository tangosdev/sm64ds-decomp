typedef signed char s8;
typedef int s32;
typedef short s16;
typedef unsigned short u16;
typedef unsigned char u8;
typedef unsigned int u32;

extern short data_02082214[];
extern int data_ov006_0212eb80[];
extern unsigned char data_ov006_0213d974[];
extern int data_0209d4b8;

extern int RandomIntInternal(int *seed);

struct S32 {
    int v;
    char pad[0x34];
};
struct S16 {
    s16 v;
    char pad[0x36];
};
struct U16 {
    u16 v;
    char pad[0x36];
};
struct U8 {
    u8 v;
    char pad[0x37];
};

void func_ov006_020fdd40(char *a)
{
    int *p_f8;
    int *p_fc;
    s8 *p_f2f;
    s16 *p_f28;
    int sp10;
    int i;
    u32 t0, t1;
    s16 h;
    int lo;
    char *cursor;

    if (*(u16 *)(a + 0x5c24) != 0) {
        (*(u16 *)(((long long)(int)(a + 0x5c24))))--;
        if (*(s16 *)(a + 0x5c24) < 0)
            *(u16 *)(a + 0x5c24) = 0;
        return;
    }

    *(u16 *)(a + 0x5c24) = 0x50;
    cursor = a;
    i = 0;
    do {
        if (*(u8 *)(cursor + 0x4000 + 0x68c) == 0) {
            ((struct U8 *)(a + 0x468c))[i].v = 1;
            t0 = (u32)RandomIntInternal(&data_0209d4b8);
            t1 = (u32)RandomIntInternal(&data_0209d4b8);
            {
                int *b0 = (int *)(a + 0x4660);
                int *b4 = (int *)(a + 0x4664);
                int *b8 = (int *)(a + 0x4668);
                int *bc = (int *)(a + 0x466c);
                u8 *b2f = (u8 *)(a + 0x468f);
                int *tbl = data_ov006_0212eb80;
                u32 m = 0x7fff;
                int off = i * 0x38;
                int xv = (tbl[((m & (t1 >> 16)) * 5) >> 15] + ((((m & (t0 >> 16)) * 4) >> 15) << 3)) << 12;
                int yv = 0x100000;
                *(int *)((char *)b0 + off) = xv;
                *(int *)((char *)b4 + off) = -yv;
                *(int *)((char *)b8 + off) = 0;
                *(int *)((char *)bc + off) = 0;
                *(u8 *)((char *)b2f + off) = 0;
                p_f8 = (int *)((char *)b8 + off);
                p_fc = (int *)((char *)bc + off);
                p_f2f = (s8 *)((char *)b2f + off);
                *(u8 *)(a + off + 0x4690) = 0;
            }
            ((struct U16 *)(a + 0x4688))[i].v =
                ((u32)((0x7fff & ((u32)RandomIntInternal(&data_0209d4b8) >> 16)) * 8) >> 15) << 4;
            ((struct U16 *)(a + 0x4684))[i].v = 0;
            p_f28 = (s16 *)&((struct U16 *)(a + 0x4688))[i].v;
            ((struct S32 *)(a + 0x467c))[i].v = 0;
            ((struct U8 *)(a + 0x4696))[i].v = (u8)*(u16 *)(a + 0x5c28);
            (*(u16 *)(((long long)(int)(a + 0x5c26))))++;
            if (*(u16 *)(a + 0x5c28) != 0) {
                u32 rr = (u32)((0x7fff & ((u32)RandomIntInternal(&data_0209d4b8) >> 16)) * 0x10) >> 15;
                int idx = (int)*(u16 *)(a + 0x5c28);
                idx >>= 1;
                if (idx >= 4)
                    idx = 4;
                if (data_ov006_0213d974[rr + idx * 4] != 0) {
                    if (((u32)((0x7fff & ((u32)RandomIntInternal(&data_0209d4b8) >> 16)) * 2) >> 15) != 0) {
                        ((struct S32 *)(a + 0x4660))[i].v = 0x100000;
                        *(s16 *)(a + i * 0x38 + 0x4600 + 0x86) =
                            (s16)(0xc000 - ((((u32)((0x7fff & ((u32)RandomIntInternal(&data_0209d4b8) >> 16)) * 0xc) >> 15) + 1) << 9));
                    } else {
                        ((struct S32 *)(a + 0x4660))[i].v = 0;
                        *(s16 *)(a + i * 0x38 + 0x4600 + 0x86) =
                            (s16)(((((u32)((0x7fff & ((u32)RandomIntInternal(&data_0209d4b8) >> 16)) * 0xc) >> 15) + 1) << 9) + 0xc000);
                    }
                    ((struct S32 *)(a + 0x4678))[i].v = 0;
                    ((struct S32 *)(a + 0x4670))[i].v = 0;
                    ((struct S32 *)(a + 0x4674))[i].v = 0;
                    {
                        u16 ang = ((struct U16 *)(a + 0x4686))[i].v;
                        ((struct U16 *)(a + 0x4684))[i].v = (u16)(ang - 0x4000);
                    }
                    ((struct S32 *)(a + 0x4664))[i].v = -0x60000;
                    sp10 = 0x10;
                    {
                        u16 ang = ((struct U16 *)(a + 0x4686))[i].v;
                        lo = (int)ang >> 4;
                    }
                    h = data_02082214[lo * 2 + 1];
                    *p_f8 = (int)(((long long)h * 0xe80 + 0x800) >> 12);
                    h = data_02082214[lo * 2];
                    *p_fc = (int)(((long long)h * 0xe80 + 0x800) >> 12);
                    *p_f2f = 3;
                    ((struct U8 *)(a + 0x468d))[i].v = 1;
                    ((struct U8 *)(a + 0x4693))[i].v = 0;
                    ((struct S32 *)(a + 0x4680))[i].v = 0x1000;
                    *p_f28 = (s16)sp10;
                }
            }
        } else {
            i++;
            cursor += 0x38;
            if (i < 0x1e)
                continue;
        }
        break;
    } while (1);

    {
        int c = *(u16 *)(a + 0x5c26);
        int n = 0;
        if (c >= 0xa) {
            do {
                c -= 0xa;
                n++;
            } while (c >= 0xa);
        }
        if (n != 0 && c == 0)
            (*(u16 *)(((long long)(int)(a + 0x5c28))))++;
    }
    if ((u32)*(u16 *)(a + 0x5c28) > 0x28)
        *(u16 *)(a + 0x5c28) = 0x27;
    {
        u16 *t = (u16 *)(((long long)(int)(a + 0x5c24)));
        *t = (u16)(*t - (*(u16 *)(a + 0x5c28) << 2));
    }
    if ((s32)*(s16 *)(a + 0x5c24) <= 0x20)
        *(u16 *)(a + 0x5c24) = 0x20;
}
