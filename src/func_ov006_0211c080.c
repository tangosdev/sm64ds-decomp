typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

extern int RandomIntInternal(int *seed);
extern int data_0209d4b8;
extern u8 data_ov006_0212ee10[];
extern u8 data_ov006_0212ee18[];
extern u8 data_ov006_0212ee20[];
extern int data_ov006_0212ef9c[];
extern int data_ov006_0212efb0[];
extern u16 *data_ov006_0213f6fc[];

#define RND (((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff)
#define LAUNDER(p) ((int)((long long)(int)(p) & 0xffffffffffffffffLL))

void func_ov006_0211c080(char *o)
{
    int sel;
    int k;
    int count;
    int n;
    int half;
    int speed;
    int i;

    sel = *(int *)(o + 0xbc);
    *(u8 *)(o + 0x5628) = *(u8 *)(o + 0x5627);
    if (sel >= 5) {
        sel = (u32)(RND * 5) >> 15;
        if (sel == *(u8 *)(o + 0x5628)) {
            sel += ((u32)(RND << 2) >> 15) + 1;
            if (sel >= 5)
                sel -= 5;
        }
    }
    *(u8 *)(o + 0x5627) = sel;
    k = *(u8 *)(o + 0x5627);
    count = data_ov006_0212ee18[k];
    speed = data_ov006_0212ee10[k];
    if (k == 3) {
        int a = (u32)(RND << 2) >> 15;
        int b = (u32)(RND * 3) >> 15;
        *(u8 *)(o + 0x561f) = b;
        *(u8 *)(o + 0x5620) = *(u8 *)(o + 0x561f) + 1;
        if (*(u8 *)(o + 0x5620) >= 3)
            *(u8 *)LAUNDER(o + 0x5620) -= 3;
        *(u8 *)(o + 0x5621) = *(u8 *)(o + 0x5620) + 1;
        if (*(u8 *)(o + 0x5621) >= 3)
            *(u8 *)LAUNDER(o + 0x5621) -= 3;
        *(u8 *)LAUNDER(o + 0x561f) += a * 3;
        *(u8 *)LAUNDER(o + 0x5620) += a * 3;
        *(u8 *)LAUNDER(o + 0x5621) += a * 3;
    } else {
        half = count >> 1;
        for (i = 0; i < half; i++) {
            int v = (u32)(RND * speed) >> 15;
            *(u8 *)(o + i + 0x561f) = v;
            if (i != 0) {
                int dup;
                int j;
                for (;;) {
                    dup = 0;
                    j = 0;
                    for (; j < i; j++) {
                        if (*(u8 *)(o + i + 0x561f) == *(u8 *)(o + j + 0x561f)) {
                            dup = 1;
                            break;
                        }
                    }
                    if (dup == 0)
                        break;
                    *(u8 *)LAUNDER(o + i + 0x561f) = (u32)(RND * speed) >> 15;
                }
            }
        }
    }
    k = *(u8 *)(o + 0x5627);
    if (k == 1 || k == 4) {
        *(int *)(o + 0x560c) = data_ov006_0212efb0[((u32)*(int *)(o + 0xbc) >> 2) & 3];
    }
    half = count >> 1;
    for (i = 0; i < half; i++) {
        *(u8 *)LAUNDER(o + i + 0x561f) += data_ov006_0212ef9c[*(u8 *)(o + 0x5627)];
    }
    {
        int xi;
        char *p;
        int zi;
        char *q;
        u8 *slot;
        int t;

        n = 0;
        if (count <= 0)
            return;
        zi = 1;
        xi = 0;
        p = o;
        do {
            t = data_ov006_0212ee20[*(u8 *)(o + 0x5627)];
            *(int *)(p + 0x50e8) = data_ov006_0213f6fc[t][xi] << 12;
            *(int *)(p + 0x50ec) = (*(u16 * volatile *)&data_ov006_0213f6fc[t])[zi] << 12;
            *(u16 *)(p + 0x50f0) = 0;
            *(u8 *)(p + 0x50f4) = 0;
            *(u8 *)(p + 0x50f5) = 1;
            *(u8 *)(p + 0x50f6) = 1;
            *(u8 *)(p + 0x50f7) = 0;
            *(u8 *)(p + 0x50f9) = 0;
            do {
                q = o + ((u32)(RND * half) >> 15);
                slot = (u8 *)LAUNDER(q + 0x561a);
            } while (*slot >= 2);
            *(u8 *)(p + 0x50f8) = *(u8 *)(q + 0x561f);
            (*slot)++;
            xi += 2;
            p += 0x14;
            zi += 2;
            n++;
        } while (n < count);
    }
}
