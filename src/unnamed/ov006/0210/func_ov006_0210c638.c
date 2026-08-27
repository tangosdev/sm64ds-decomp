extern int func_ov004_020ad8b8(void);

void func_ov006_0210c638(char *thiz)
{
    *(signed char *)(thiz + 0x4706) = -1;
    *(unsigned char *)(thiz + 0x4707) = 1;
    *(unsigned char *)(thiz + 0x4708) = 0;
    *(int *)(thiz + 0xa8) = func_ov004_020ad8b8();
    *(int *)(thiz + 0xac) = *(int *)(thiz + 0xa8);
}
