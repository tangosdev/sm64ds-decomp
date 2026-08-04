extern struct P { int x, y; } data_ov006_02142ab4[];
extern struct P data_ov006_02142ab8[];
void func_ov006_02109530(int *out, int *a, int scale);

#define LNDR(e) ((int)(((long long)(e))))

struct S {
    char pad[0x18];
    int f18;
    int f1c;
    char pad2[0x2c - 0x20];
    short f2c;
};

void func_ov006_0210935c(struct S *self, int idx)
{
    int buf[7];
    int f18 = self->f18;
    int a, b, lr, t, hi;
    buf[0] = f18;
    buf[1] = self->f1c;
    if ((unsigned short)(short)(idx - 0x23) <= 1) {
        a = data_ov006_02142ab4[idx].x;
        b = data_ov006_02142ab8[idx].x;
        buf[2] = a;
        buf[3] = b;
        lr = a + 0x8000;
        hi = a + 0x18000;
        if (f18 >= lr) {
            if (f18 <= hi)
                hi = f18;
            lr = hi;
        }
        b = buf[3];
        t = buf[1];
        {
            int lo = b - 0x20000;
            buf[0] = lr;
            lr = lo;
            hi = b + 0x20000;
            if (t >= lo) {
                if (t <= hi)
                    hi = t;
                lr = hi;
            }
        }
        buf[1] = lr;
        func_ov006_02109530((int *)self, buf, 0x100);
        return;
    }
    {
        int early = (self->f2c <= 0xb) ? 1 : 0;
        if (LNDR(early) == 0)
            return;
    }
    a = data_ov006_02142ab4[idx].x;
    b = data_ov006_02142ab8[idx].x;
    buf[4] = a;
    buf[5] = b;
    lr = a - 0x10000;
    hi = a + 0x10000;
    if (buf[0] >= lr) {
        if (buf[0] <= hi)
            hi = buf[0];
        lr = hi;
    }
    b = buf[5];
    t = buf[1];
    buf[0] = lr;
    lr = b - 0xc000;
    hi = b + 0xc000;
    if (t >= lr) {
        if (t <= hi)
            hi = t;
        lr = hi;
    }
    buf[1] = lr;
    func_ov006_02109530((int *)self, buf, 0x100);
}
