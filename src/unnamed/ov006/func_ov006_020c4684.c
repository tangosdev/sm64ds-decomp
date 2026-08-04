extern int func_ov006_020c6e4c(char *p);
extern int data_ov006_02140328;
extern int data_ov006_02140304;
extern char *data_ov006_02140324;
extern int data_ov006_02140314;
int func_ov006_020c4684(char *ptr, int n);
int func_ov006_020c4684(char *ptr, int n){
    int i;
    int off;
    data_ov006_02140314 = 0;
    data_ov006_02140328 = n;
    data_ov006_02140304 = n;
    data_ov006_02140324 = ptr;
    if (n > 0) {
        i = 0;
        off = 0;
        do {
            if (func_ov006_020c6e4c(data_ov006_02140324 + off) == 0)
                return 0;
            i++;
            off += 0xf0;
        } while (i < data_ov006_02140328);
    }
    return 1;
}
