extern int data_ov006_02140594;
extern char *data_ov006_02140550;
extern int data_ov006_0213b164[];

int func_ov006_020cd158(void)
{
    int i = 0;
    char *e;
    int first;
    int n;
    int *b;
    n = data_ov006_02140594;
    if (n > 0) {
        e = data_ov006_02140550;
        b = data_ov006_0213b164;
        first = b[0];
        do {
            int *p = (int *)(((unsigned int)e + 0x64) & 0xFFFFFFFFFFFFFFFFull);
            if (p[0] == first) {
                if (p[1] == *(volatile int *)(b + 1)) goto next;
                if (*(volatile int *)(e + 0x64) == 0) goto next;
            }
            return 0;
        next:
            e += 0xd0;
            i++;
        } while (i < n);
    }
    return 1;
}
