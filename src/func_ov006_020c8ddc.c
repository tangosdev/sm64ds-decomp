extern int data_ov006_0213b13c[2];
extern int data_ov006_0213b144[2];

int func_ov006_020c8ddc(char *c)
{
    int *p;
    int *d;
    int result;
    int flag;

    p = (int *)(((long long)(int)(c + 0x70)) & 0xFFFFFFFFFFFFFFFFLL);
    d = data_ov006_0213b13c;
    result = 0;
    flag = 1;
    if (p[0] == d[0]) {
        if (p[1] != d[1]) {
            if (*(int *)(c + 0x70) != 0)
                goto after1;
        }
        flag = 0;
    }
after1:
    if (flag != 0) {
        p = (int *)(((long long)(unsigned)(c + 0x70)) & 0xFFFFFFFFFFFFFFFFLL);
        d = data_ov006_0213b144;
        flag = 1;
        if (p[0] == d[0]) {
            if (p[1] != d[1]) {
                if (*(int *)(c + 0x70) != 0)
                    goto after2;
            }
            flag = 0;
        }
    after2:
        if (flag != 0)
            result = 1;
    }
    return result;
}
