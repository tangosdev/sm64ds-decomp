extern void func_ov006_020cb030(char *p);
extern void func_ov006_020c8a64(void);
extern int data_ov006_02140594;
extern char *data_ov006_02140550;
extern int data_ov006_0214055c;
extern int data_ov006_02142f60;

void func_ov006_020cd39c(void)
{
    int i;
    for (i = 0; i < data_ov006_02140594; i++)
        func_ov006_020cb030(data_ov006_02140550 + i * 0xd0);
    if (data_ov006_0214055c != 0)
        data_ov006_02142f60 = 1;
    else
        data_ov006_02142f60 = 0;
    func_ov006_020c8a64();
}
