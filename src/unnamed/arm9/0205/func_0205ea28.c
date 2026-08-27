extern unsigned short data_020a80cc[];
extern int data_020a80e4[];

struct V4 { unsigned short a, b, c, d; };

void func_0205ea28(struct V4 *r0, struct V4 *r1)
{
    int *p;
    int t;
    if (data_020a80cc[0x18] == 0) {
        *r0 = *r1;
        return;
    }
    p = data_020a80e4;
    r0->c = r1->c;
    r0->d = r1->d;
    if (r1->c == 0) {
        r0->a = 0;
        r0->b = 0;
        return;
    }
    r0->a = (int)(((long long)p[2] * ((long long)(r1->a << 2) - (long long)p[0])) >> 22);
    t = (short)r0->a;
    if (t < 0) { r0->a = 0; }
    else if (t > 0xff) { r0->a = 0xff; }
    r0->b = (int)(((long long)p[5] * ((long long)(r1->b << 2) - (long long)p[3])) >> 22);
    t = (short)r0->b;
    if (t < 0) { r0->b = 0; return; }
    if (t > 0xbf) { r0->b = 0xbf; }
}
