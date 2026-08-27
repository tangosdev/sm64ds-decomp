extern void func_ov006_020cb1a8(void *p);
extern void func_ov006_020cafdc(char *o);
extern int data_ov006_02140594[];
extern int data_ov006_02140548[];
extern int data_ov006_02140544[];
extern char *data_ov006_02140550[];

void func_ov006_020cd1e0(void)
{
    int i;
    int off;
    i = 0;
    data_ov006_02140544[0] = 0x100;
    data_ov006_02140548[0] = -0x4800;
    if (data_ov006_02140594[0] <= 0)
        return;
    off = 0;
    do {
        func_ov006_020cb1a8(data_ov006_02140550[0] + off);
        func_ov006_020cafdc(data_ov006_02140550[0] + off);
        i++;
        off += 0xd0;
    } while (i < data_ov006_02140594[0]);
}
