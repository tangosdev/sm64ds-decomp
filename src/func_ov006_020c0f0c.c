extern int data_ov006_0213acb0[2];
extern int data_ov006_0213aca8[2];

int func_ov006_020c0f0c(int *c)
{
    int *p = data_ov006_0213acb0;
    int r = 1;
    int m1 = r;
    int x = *c;
    if (x == *p) {
        if (c[1] == p[1] || x == 0)
            m1 = 0;
    }
    if (m1 != 0) {
        int m2 = 1;
        p = data_ov006_0213aca8;
        if (x == *p) {
            if (c[1] == p[1] || x == 0)
                m2 = 0;
        }
        if (m2)
            r = 0;
    }
    return r;
}
