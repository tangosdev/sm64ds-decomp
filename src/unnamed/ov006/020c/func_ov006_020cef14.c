extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *fp);
extern void func_ov006_020cecc0(void *p);
extern int data_ov006_02140850[];
extern int data_ov006_02140858[];
extern int data_ov006_02140848[];
extern char *data_ov006_0214082c[];
extern int data_ov006_02140838[];

void func_ov006_020cef14(char *a, int count)
{
    int i;
    int off;
    _ZN5Model8LoadFileER13SharedFilePtr(data_ov006_02140850);
    _ZN5Model8LoadFileER13SharedFilePtr(data_ov006_02140858);
    _ZN5Model8LoadFileER13SharedFilePtr(data_ov006_02140848);
    data_ov006_0214082c[0] = a;
    data_ov006_02140838[0] = count;
    i = 0;
    if (count <= 0)
        return;
    off = 0;
    do {
        func_ov006_020cecc0(data_ov006_0214082c[0] + off);
        i++;
        off += 0x1d0;
    } while (i < data_ov006_02140838[0]);
}
