extern unsigned short data_ov100_02148708[];
extern int data_0209b490[];
extern int data_0209b49c[];

int func_ov100_02144f84(void)
{
    if (*data_ov100_02148708 != 0) goto fail;
    if (*data_0209b490 != 0x7f000) goto fail;
    if (*data_0209b49c == 0) return 1;
fail:
    return 0;
}
