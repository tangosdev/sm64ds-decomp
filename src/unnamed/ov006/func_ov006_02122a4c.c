extern int data_ov006_02142f74;
extern char *data_ov006_02142f7c;
extern void func_ov006_02122814(char *obj);

void func_ov006_02122a4c(void)
{
    int i;
    for (i = 0; i < data_ov006_02142f74; i++) {
        char *obj = data_ov006_02142f7c + i * 0x78;
        if (*(short *)(obj + 0x76) != 0) {
            func_ov006_02122814(obj);
        }
    }
}
