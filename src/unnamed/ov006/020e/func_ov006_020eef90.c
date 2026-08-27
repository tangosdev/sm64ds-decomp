extern void func_ov006_020eedc8(char *p);
extern int data_ov006_021421bc;
extern char *data_ov006_021421b0;

void func_ov006_020eef90(void)
{
    int i = 0;
    if (i < data_ov006_021421bc) {
        int off = 0;
        do {
            func_ov006_020eedc8(data_ov006_021421b0 + off);
            i++;
            off += 0x24;
        } while (i < data_ov006_021421bc);
    }
}
