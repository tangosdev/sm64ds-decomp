extern unsigned char data_0209d664;
extern unsigned char data_0209f2d8;
extern short data_0209d6d4;

int func_0201fb4c(void)
{
    short v;
    int b;
    if (data_0209d664 != 0) goto no;
    b = (data_0209f2d8 == 2);
    if (b != 0) goto no;
    v = data_0209d6d4;
    if (v == 0xcc) goto no;
    if (v == 0xce) goto no;
    if (v == 0xd0) goto no;
    if (v == 0xd3) goto no;
    if (v == 0xd4) goto no;
    if (v == 0xd5) goto no;
    if (v == 0xd6) goto no;
    if (v == 0xdb) goto no;
    if (v == 0xdc) goto no;
    if (v == 0xdd) goto no;
    if (v == 0xde) goto no;
    if (v == 0xe3) goto no;
    if (v == 0xe4) goto no;
    if (v == 0xe5) goto no;
    if (v == 0xe6) goto no;
    return 1;
no:
    return 0;
}
