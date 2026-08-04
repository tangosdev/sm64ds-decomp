extern int data_ov006_0213c958;
extern int data_ov006_0213ca6c[];
struct W2 { int a, b; };

void func_ov006_020ed40c(char *c)
{
    int i = 0;
    if (i < data_ov006_0213c958) {
        char *p = c;
        do {
            *(unsigned char *)(p + 0x4000 + 0x70a) = 0;
            i++;
            p += 0x98;
        } while (i < data_ov006_0213c958);
    }
    if (*(unsigned char *)(c + 0x4000 + 0xf64) != 0)
        *(int *)(c + 0x4000 + 0x66c) = 0xb4;
    else
        *(int *)(c + 0x4000 + 0x66c) = 0xe4;
    *(struct W2 *)(c + 0x4000 + 0x660) = *(struct W2 *)data_ov006_0213ca6c;
}
