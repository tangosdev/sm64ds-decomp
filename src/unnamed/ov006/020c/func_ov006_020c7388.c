extern void func_ov006_020c8084(void *p);
extern void func_ov006_020c76e0(char *c);
extern int data_ov006_02140418[];
extern int data_ov006_0213b010[];
extern int data_ov006_0213b018[];
extern char *data_ov006_02140420[];

void func_ov006_020c7388(void)
{
    int i;
    int off;
    i = 0;
    data_ov006_0213b018[0] = 0x100;
    data_ov006_0213b010[0] = -0x4800;
    if (data_ov006_02140418[0] <= 0)
        return;
    off = 0;
    do {
        func_ov006_020c8084(data_ov006_02140420[0] + off);
        func_ov006_020c76e0(data_ov006_02140420[0] + off);
        i++;
        off += 0xb8;
    } while (i < data_ov006_02140418[0]);
}
